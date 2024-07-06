#include "BadgeManager.h"

// Include all modes
#include "modes/Home.hpp"
#include "modes/Truth.hpp"
#include "modes/Revelation.hpp"
#include "modes/Magic8Ball.hpp"
#include "modes/MadHatterMeter.hpp"
#include "modes/Handle.hpp"

static const char *TAG = "BadgeManager";

const std::unordered_map<ModeType, const char *> ModeTitle = {
    {ModeType::HOME, "Home"},
    {ModeType::TRUTH, "Truth"},
    {ModeType::REVELATION, "Revelation"},
    {ModeType::MAGIC_8BALL, "Magic 8 Ball"},
    {ModeType::MAD_HATTER_METER, "Mad Hatter Meter"},
    {ModeType::LED_MODES, "LED Modes"},
    {ModeType::DISPLAY_HANDLE, "Display Handle"},
    {ModeType::ABOUT, "About"},
};

// Queue to handle a single mode change at a time
QueueHandle_t modeQueue = xQueueCreate(1, sizeof(ModeType));

void Badge::init() {
    // Initialize the rest of the badge components
    display.init();
    touch.init();
    // LedHandler &leds = LedHandler::getInstance();
    leds.init();

    // Initialize all modes here
    modes[ModeType::HOME]             = std::make_unique<HomeMode>();
    modes[ModeType::TRUTH]            = std::make_unique<TruthMode>();
    modes[ModeType::REVELATION]       = std::make_unique<RevelationMode>();
    modes[ModeType::MAGIC_8BALL]      = std::make_unique<Magic8BallMode>();
    modes[ModeType::MAD_HATTER_METER] = std::make_unique<MadHatterMeterMode>();
    modes[ModeType::DISPLAY_HANDLE]   = std::make_unique<HandleMode>();

    // DEBUG: Print all modes and their addresses
    if (CORE_DEBUG_LEVEL >= 4) {
        ESP_LOGD(TAG, "Modes:");
        for (const auto &mode : modes) {
            ESP_LOGD(TAG, "  %s: %p", ModeTitle.at(mode.first), mode.second.get());
        }
    }

    // Set the default mode to HOME
    setMode(ModeType::HOME);

    // Create the mode task
    xTaskCreate(modeInputTask, "ModeInputTask", 4096, this, 6, NULL);
    xTaskCreate(modeManagerTask, "ModeManagerTask", 2048, this, 5, NULL);
}

void Badge::flashLedStrip(AddressableStrip strip, CRGB color, int count, int duration) {
    for (int i = 0; i < count; ++i) {
        // Turn the LEDs on
        for (int j = 0; j < LedCounts[strip]; ++j) {
            leds.lockLed(strip, handshakeLeds.right(j), color);
        }
        leds.show();
        vTaskDelay(duration / count / 2 / portTICK_PERIOD_MS);

        // Turn the LEDs off
        for (int j = 0; j < LedCounts[strip]; ++j) {
            leds.lockLed(strip, handshakeLeds.right(j), CRGB::Black);
        }
        leds.show();
        vTaskDelay(duration / count / 2 / portTICK_PERIOD_MS);
    }
    // Unlock the LEDs once we're done flashing them
    for (int j = 0; j < LedCounts[strip]; ++j) {
        leds.unlockLed(strip, handshakeLeds.right(j));
    }
}

void Badge::modeInputTask(void *pvParameters) {
    Badge *self = static_cast<Badge *>(pvParameters);
    TouchEvent event;
    TouchEvent(&latestEvents)[HANDSHAKE_COUNT] = self->touch.getLatestEvents();

    const int exitPin          = HANDSHAKE_1;
    bool holdingToExit         = false;
    unsigned long exitHoldTime = 1000;

    while (true) {
        if (touchQueue && xQueueReceive(touchQueue, &event, portMAX_DELAY) == pdTRUE) {
            // Check if _only_ handshake 1 is being held to exit the current mode
            if (event.pin == exitPin) {
                if (std::all_of(latestEvents, latestEvents + HANDSHAKE_COUNT,
                                [](TouchEvent e) { return e.pin == exitPin || e.type == TOUCH_UP; })) {
                    if (!holdingToExit) {
                        holdingToExit = true;
                    } else if (event.duration >= exitHoldTime) {
                        holdingToExit = false;
                        ESP_LOGD(TAG, "Exit to HOME requested");
                        {
                            std::lock_guard<std::mutex> lock(self->modeMutex);
                            if (self->currentMode) {
                                if (self->currentMode->getType() == ModeType::HOME) {
                                    ESP_LOGD(TAG, "Current mode is already HOME, not exiting");
                                    continue;
                                }
                                self->currentMode->exit();
                                self->currentMode = nullptr;
                            }
                        }
                        self->display.setFont(u8g2_font_crox5tb_tr);
                        self->display.showTextCentered("Exiting...");
                        self->display.setFont(NULL);

                        // Flash handshake LEDs blue to indicate exiting
                        self->flashLedStrip(AddressableStrip::TOUCH, CRGB::Blue, 3, 900);

                        // Return to menu
                        self->setMode(ModeType::HOME);
                    }
                } else {
                    holdingToExit = false;
                }
            }

            // Pass the event along to the DisplayManager
            bool suppressModeHandler = self->display.handleTouch(event);

            // Pass the event to the current mode if not suppressed
            if (!suppressModeHandler) {
                std::lock_guard<std::mutex> lock(self->modeMutex);
                if (self->currentMode) {
                    self->currentMode->handleTouch(event);
                }
            }
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

// Handles mode switching asynchronously to avoid modeMutex deadlocks due to touch event handling
void Badge::modeManagerTask(void *pvParameters) {
    Badge *self = static_cast<Badge *>(pvParameters);
    ModeType mode;

    while (true) {
        if (modeQueue && xQueueReceive(modeQueue, &mode, portMAX_DELAY) == pdTRUE) {
            ESP_LOGD(TAG, "Switching to mode %s", ModeTitle.at(mode));
            {
                std::lock_guard<std::mutex> lock(self->modeMutex);
                if (self->currentMode) {
                    self->currentMode->exit();
                }
                self->currentMode = self->modes[mode].get();
                if (self->currentMode) {
                    // Clear any touch events that may have been queued up
                    self->currentMode->enter();
                }
            }
        }
    }
}

void Badge::setMode(ModeType mode) {
    xQueueSend(modeQueue, &mode, portMAX_DELAY);
}

BadgeMode *Badge::getMode() {
    std::lock_guard<std::mutex> lock(modeMutex);
    return currentMode;
}

bool Badge::hasMode(ModeType mode) {
    ESP_LOGD(TAG, "hasMode(%d: %s)", static_cast<int>(mode), ModeTitle.at(mode));
    return modes.find(mode) != modes.end();
}
