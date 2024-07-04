#include "BadgeManager.h"

// Include all modes
#include "modes/Home.hpp"
#include "modes/Truth.hpp"

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
    modes[ModeType::HOME]  = std::make_unique<HomeMode>(&display, &leds, &touch);
    modes[ModeType::TRUTH] = std::make_unique<TruthMode>(&display, &leds, &touch);

    modes[ModeType::DISPLAY_HANDLE] = std::make_unique<HandleMode>(&display, &leds, &touch);

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

void Badge::modeInputTask(void *pvParameters) {
    Badge *self = static_cast<Badge *>(pvParameters);
    TouchEvent event;
    bool holdingToExit          = false;
    unsigned long holdStartTime = 0;
    unsigned long exitHoldTime  = 2000;

    while (true) {
        if (touchQueue && xQueueReceive(touchQueue, &event, portMAX_DELAY) == pdTRUE) {
            // Check if the handshake 1 is being held to exit the current mode
            if (event.pin == HANDSHAKE_1) {
                if (event.type == TOUCH_DOWN) {
                    if (!holdingToExit) {
                        holdingToExit = true;
                        holdStartTime = millis();
                    } else if (millis() - holdStartTime >= exitHoldTime) {
                        ESP_LOGD(TAG, "Force exiting current mode");
                        {
                            std::lock_guard<std::mutex> lock(self->modeMutex);
                            if (self->currentMode) {
                                self->currentMode->exit();
                            }
                        }
                        self->display.setFont(u8g2_font_crox5tb_tr);
                        self->display.showTextCentered("Exiting...");
                        self->display.setFont(NULL);

                        // Flash the handshake LEDs blue 3 times over 1 second
                        for (int j = 0; j < 3; ++j) {
                            for (int i = 0; i < HANDSHAKE_COUNT; ++i) {
                                self->leds.lockLed(TOUCH, handshakeLeds.right(i), CRGB::Blue);
                            }
                            self->leds.show();
                            vTaskDelay((1000 / 3 / 2) / portTICK_PERIOD_MS); // On duration
                            for (int i = 0; i < HANDSHAKE_COUNT; ++i) {
                                self->leds.lockLed(TOUCH, handshakeLeds.right(i), CRGB::Black);
                            }
                            self->leds.show();
                            vTaskDelay((1000 / 3 / 2) / portTICK_PERIOD_MS); // Off duration
                        }
                        // Unlock the LEDs
                        for (int i = 0; i < HANDSHAKE_COUNT; ++i) {
                            self->leds.unlockLed(TOUCH, handshakeLeds.right(i));
                        }

                        // Return to menu
                        self->setMode(ModeType::HOME);

                        holdingToExit = false;
                    }
                } else if (event.type == TOUCH_UP) {
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
