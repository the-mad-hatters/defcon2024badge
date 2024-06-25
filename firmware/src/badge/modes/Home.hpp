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
        : inMenu(false), menuIndex(0) {
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
        // display->clear();
        xTaskCreate(homeInitTask, "Home Init Task", 2048, this, 5, &homeTaskHandle);
    }

    void exit() override {
        ESP_LOGD(TAG_HOMEMODE, "Exiting Home mode");
        // display->clear();
        // ESP_LOGD(TAG_HOMEMODE, "Display cleared");
        stopInitTask();
    }

    static void homeInitTask(void *pvParameters) {
        HomeMode *self = static_cast<HomeMode *>(pvParameters);
        ScrollEvent event;

        // Start the LEDs
        ESP_LOGD(TAG_HOMEMODE, "Starting LED scene: GOING_TO_HELL");
        self->leds->clear(true);
        self->leds->setScene(SceneType::GOING_TO_HELL);

        // First scroll the title
        self->display->scrollText(u8g2_font_crox5tb_tr, "Mad Hatter Auto Revelator", 10, 1);

        // Wait for the scroll to end
        while (true) {
            if (xQueueReceive(textScrollEvents, &event, portMAX_DELAY) == pdTRUE) {
                if (event.type == ScrollEventType::SCROLL_END) {
                    break;
                }
            }
        }

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
            display->showList(u8g2_font_ncenB08_tr, menuItems.data(), menuItems.size(), menuIndex);
        } else {
            ESP_LOGD(TAG_HOMEMODE, "Showing rock image");
            display->drawImage(ImageID::ROCK, 0, 0);
            menuIndex = 0;
        }
    }

    void handleTouch(TouchEvent event) override {
        // Only handle touch events if the touch event type has changed
        if (lastTouchType[event.pin] == event.type) {
            return;
        }
        lastTouchType[event.pin] = event.type;

        // Only react to touch down events
        if (event.type != TOUCH_DOWN) {
            return;
        }

        switch (event.pin) {
            case HANDSHAKE_1:
                inMenu = !inMenu;
                updateDisplay();
                break;
            case HANDSHAKE_2:
                // Move to the previous menu item if we're in the menu (wrap around to the end if at
                // the beginning)
                if (inMenu) {
                    menuIndex = (menuIndex == 0) ? menuItems.size() - 1 : menuIndex - 1;
                    updateDisplay();
                }
                break;
            case HANDSHAKE_3:
                // Move to the next menu item if we're in the menu and there is a next item
                if (inMenu) {
                    menuIndex = (menuIndex + 1) % menuItems.size();
                    updateDisplay();
                }
                break;
            case HANDSHAKE_4:
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
                break;
        }
    }

  private:
    TaskHandle_t homeTaskHandle;
    bool inMenu;
    int menuIndex;
    std::vector<const char *> menuItems;
    TouchEventType lastTouchType[HANDSHAKE_COUNT] = {TOUCH_UP, TOUCH_UP, TOUCH_UP, TOUCH_UP};
};

#endif // MODE_HOME_HPP