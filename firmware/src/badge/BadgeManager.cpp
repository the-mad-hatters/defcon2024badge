#include "BadgeManager.h"

// Include all modes
#include "modes/Home.hpp"
#include "modes/Truth.hpp"

static const char *TAG = "BadgeManager";

const std::unordered_map<ModeType, const char *> ModeTitle = {
    {ModeType::HOME, "Home"},
    {ModeType::TRUTH, "Truth"},
    {ModeType::REVELATION, "Revelation"},
    {ModeType::MAD_HATTER_METER, "Mad Hatter Meter"},
    {ModeType::MAD_HATTER_8BALL, "Mad Hatter 8 Ball"},
};

void Badge::init() {
    // Initialize the rest of the badge components
    display.init();
    touch.init();
    // LedHandler &leds = LedHandler::getInstance();
    leds.init();

    // Initialize all modes here
    modes[ModeType::HOME]  = std::make_unique<HomeMode>(&display, &leds, &touch);
    modes[ModeType::TRUTH] = std::make_unique<TruthMode>(&display, &leds, &touch);

    // Set the default mode to HOME
    setMode(ModeType::HOME);

    // Create the mode task
    xTaskCreate(modeTask, "ModeTask", 2048, this, 6, NULL);
}

void Badge::modeTask(void *pvParameters) {
    Badge *self = static_cast<Badge *>(pvParameters);
    TouchEvent event;
    bool holdingToExit          = false;
    unsigned long holdStartTime = 0;
    unsigned long exitHoldTime  = 3000;

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
                        self->display.showTextCentered(u8g2_font_crox5tb_tr, "Exiting...");

                        // Flash the other handshake LEDs blue 3 times over 1 second
                        for (int j = 0; j < 3; ++j) {
                            for (int i = 1; i < HANDSHAKE_COUNT; ++i) {
                                self->leds.lockLed(TOUCH, TOUCH_LED_COUNT - (i + 1), CRGB::Blue);
                            }
                            self->leds.show();
                            vTaskDelay((1000 / 3 / 2) / portTICK_PERIOD_MS); // On duration
                            for (int i = 1; i < HANDSHAKE_COUNT; ++i) {
                                self->leds.lockLed(TOUCH, TOUCH_LED_COUNT - (i + 1), CRGB::Black);
                            }
                            self->leds.show();
                            vTaskDelay((1000 / 3 / 2) / portTICK_PERIOD_MS); // Off duration
                        }
                        // Unlock the LEDs
                        for (int i = 1; i < HANDSHAKE_COUNT; ++i) {
                            self->leds.unlockLed(TOUCH, TOUCH_LED_COUNT - (i + 1));
                        }

                        // Return to menu
                        self->setMode(ModeType::HOME);

                        holdingToExit = false;
                    }
                } else if (event.type == TOUCH_UP) {
                    holdingToExit = false;
                }
            }

            // Pass the event to the current mode
            if (self->currentMode) {
                self->currentMode->handleTouch(event);
            }
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void Badge::setMode(ModeType mode) {
    ESP_LOGD(TAG, "setMode(%d: %s)", static_cast<int>(mode), ModeTitle.at(mode));
    {
        std::lock_guard<std::mutex> lock(modeMutex);
        if (currentMode) {
            ESP_LOGD(TAG, "Calling exit on current mode");
            currentMode->exit();
        } else {
            ESP_LOGD(TAG, "currentMode is NULL");
        }
        currentMode = modes[mode].get();
    }
    ESP_LOGD(TAG, "currentMode set to: %p", currentMode);
    ESP_LOGD(TAG, "Switching to mode %s", ModeTitle.at(mode));
    ESP_LOGD(TAG, "currentMode: %p", currentMode);
    {
        std::lock_guard<std::mutex> lock(modeMutex);
        if (currentMode) {
            ESP_LOGD(TAG, "Calling enter on new mode");
            currentMode->enter();
        }
    }
}

BadgeMode *Badge::getMode() {
    std::lock_guard<std::mutex> lock(modeMutex);
    return currentMode;
}

bool Badge::hasMode(ModeType mode) {
    ESP_LOGD(TAG, "hasMode(%d: %s)", static_cast<int>(mode), ModeTitle.at(mode));
    return modes.find(mode) != modes.end();
}
