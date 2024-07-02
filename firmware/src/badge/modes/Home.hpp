#ifndef MODE_HOME_HPP
#define MODE_HOME_HPP

#include <map>
#include "esp_log.h"

#include "badge/BadgeMode.h"
#include "badge/BadgeManager.h"

static const char *TAG_HOMEMODE = "HomeMode";

class HomeMode : public BadgeMode {
  public:
    HomeMode(DisplayManager *display, LedHandler *leds, TouchHandler *touch)
        : homeTaskHandle(NULL), inMenu(false), menuIndex(0) {
        this->display = display;
        this->leds    = leds;
        this->touch   = touch;

        // Menu items is everything in ModeTitle except HOME in the order of ModeType
        for (int i = (int)ModeType::HOME + 1; i < ModeTitle.size(); i++) {
            menuItems.push_back(ModeTitle.at(static_cast<ModeType>(i)));
        }
    };

    void enter() override {
        ESP_LOGD(TAG_HOMEMODE, "Entering Home mode");
        xTaskCreate(homeInitTask, "Home Init Task", 2048, this, 5, &homeTaskHandle);
    }

    void exit() override {
        ESP_LOGD(TAG_HOMEMODE, "Exiting Home mode");
        stopInitTask();
        leds->unlockNonAddressable(BOOK_EYE); // In case we were in (or switched modes from) the
                                              // menu
    }

    static void homeInitTask(void *pvParameters) {
        HomeMode *self = static_cast<HomeMode *>(pvParameters);
        ScrollEvent event;

        // Start the LEDs
        ESP_LOGD(TAG_HOMEMODE, "Starting LED scene: GOING_TO_HELL");
        self->leds->clear(true);
        self->leds->setScene(SceneType::GOING_TO_HELL);

        // Show the title
        self->display->setFont(u8g2_font_lubB14_tf);
        self->display->showTextCentered("Mad Hatter\nAuto\nRevelator");

        // Wait for 5 seconds
        vTaskDelay(5000 / portTICK_PERIOD_MS);

        // Show the rock image
        ESP_LOGD(TAG_HOMEMODE, "Showing rock image");
        self->display->drawImage(ImageID::ROCK, 0, 0);

        // We're done now so clean up and delete the task
        ESP_LOGD(TAG_HOMEMODE, "Home init task complete");
        self->homeTaskHandle = NULL;
        vTaskDelete(NULL);
    }

    void stopInitTask() {
        if (homeTaskHandle) {
            ESP_LOGD(TAG_HOMEMODE, "Stopping home init task");

            // Clear the display to make sure the text scroll task gets stopped and cleaned up if
            // it's still running
            display->clear();

            // Delete the task
            vTaskDelete(homeTaskHandle);
            homeTaskHandle = NULL;
        }
    }

    void updateDisplay() {
        stopInitTask();
        ESP_LOGD(TAG_HOMEMODE, "Updating display");
        if (inMenu) {
            ESP_LOGD(TAG_HOMEMODE, "Showing menu");
            leds->lockNonAddressable(BOOK_EYE, true);
            display->setFont(u8g2_font_ncenB08_tr);
            display->showList(menuItems.data(), menuItems.size(), menuIndex, [this](int index) {
                menuIndex = index;

                // Select the current menu item if we're in the menu
                Badge &badge     = Badge::getInstance();
                ModeType newMode = static_cast<ModeType>(menuIndex + 1);
                if (inMenu) {
                    ESP_LOGD(TAG_HOMEMODE, "HANDSHAKE_4 pressed - switching to mode %s",
                             ModeTitle.at(newMode));
                    if (badge.hasMode(newMode)) {
                        badge.setMode(newMode);
                    } else {
                        ESP_LOGE(TAG_HOMEMODE, "Mode not found: %d", newMode);
                    }
                }
            });
        } else {
            ESP_LOGD(TAG_HOMEMODE, "Showing rock image");
            leds->unlockNonAddressable(BOOK_EYE);
            display->drawImage(ImageID::ROCK, 0, 0);
            menuIndex = 0;
        }
    }

    void handleTouch(TouchEvent event) override {
        // Only react to touch down events
        if (!event.changed || event.type != TOUCH_DOWN) {
            return;
        }

        switch (event.pin) {
            case HANDSHAKE_1:
                inMenu = !inMenu;
                updateDisplay();
                break;
        }
    }

  private:
    TaskHandle_t homeTaskHandle;
    bool inMenu;
    int menuIndex;
    std::vector<const char *> menuItems;
};

#endif // MODE_HOME_HPP