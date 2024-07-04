#ifndef MODE_HOME_HPP
#define MODE_HOME_HPP

#include <map>
#include "esp_log.h"
#include "badge/BadgeMode.h"
#include "badge/BadgeManager.h"

static const char *TAG_HOMEMODE = "HomeMode";

struct MenuItem {
    std::string name;
    std::function<bool()> action;
    std::vector<MenuItem> subMenu;
};

class HomeMode : public BadgeMode {
  public:
    HomeMode(DisplayManager *display, LedHandler *leds, TouchHandler *touch)
        : homeTaskHandle(nullptr), badgeStartup(true), inMenu(false), menuIndex(0) {
        this->display = display;
        this->leds    = leds;
        this->touch   = touch;

        // Initialize the menu structure
        initializeMenu();
    }

    void enter() override {
        ESP_LOGD(TAG_HOMEMODE, "Entering Home mode");
        if (badgeStartup) {
            badgeStartup = false;
            ESP_LOGD(TAG_HOMEMODE, "Badge startup detected, starting home init task");
            xTaskCreate(homeInitTask, "Home Init Task", 2048, this, 5, &homeTaskHandle);
        } else {
            updateDisplay();
        }
    }

    void exit() override {
        ESP_LOGD(TAG_HOMEMODE, "Exiting Home mode");
        stopInitTask();
        leds->unlockNonAddressable(BOOK_EYE); // In case we were in (or switched modes from) the
                                              // menu
    }

    void handleTouch(TouchEvent event) override {
        // Only react to touch down events
        if (!event.changed || event.type != TOUCH_DOWN) {
            return;
        }

        if (event.pin == HANDSHAKE_1) {
            if (!inMenu) {
                showMainMenu();
            } else {
                navigateBack();
            }
        }
    }

  private:
    TaskHandle_t homeTaskHandle;
    bool badgeStartup;
    bool inMenu;
    int menuIndex;
    std::vector<MenuItem> currentMenu;
    std::vector<MenuItem> mainMenu;
    std::vector<std::pair<std::vector<MenuItem>, int>> menuStack; // Stack with menu and index

    void initializeMenu() {
        // clang-format off

        // Define the LED Modes submenu
        std::vector<MenuItem> ledModesMenu = {
            {"Mad Hatter",          [this]() { return setScene(SceneType::MAD_HATTER); }},
            {"Celestial Clouds",    [this]() { return setScene(SceneType::CELESTIAL_CLOUDS); }},
            {"Gone to Hell",        [this]() { return setScene(SceneType::GONE_TO_HELL); }},
            {"Secret Combinations", [this]() { return setScene(SceneType::SECRET_COMBINATIONS); }},
            {"The Salamander",      [this]() { return setScene(SceneType::THE_SALAMANDER); }},
            {"Seer Stones",         [this]() { return setScene(SceneType::SEER_STONES); }},
            {"Drunk as Hell",       [this]() { return setScene(SceneType::DRUNK_AS_HELL); }},
            {"DEF CON 32",          [this]() { return setScene(SceneType::DC32_Y2K_AESTHETIC); }},
        };

        // Define the top-level menu
        mainMenu = {
            {"Truth",               [this]() { return setMode(ModeType::TRUTH); }},
            {"Revelation",          [this]() { return setMode(ModeType::REVELATION); }},
            {"Magic 8 Ball",        [this]() { return setMode(ModeType::MAGIC_8BALL); }},
            {"Mad Hatter Meter",    [this]() { return setMode(ModeType::MAD_HATTER_METER); }},
            {"LED Modes",           [this, ledModesMenu]() { return enterSubMenu(ledModesMenu); }},
            {"Display Handle",      [this]() { return setMode(ModeType::DISPLAY_HANDLE); }},
            {"About",               [this]() { return setMode(ModeType::ABOUT); }},
        };

        // clang-format on
    }

    void showMainMenu() {
        currentMenu = mainMenu;
        inMenu      = true;
        updateDisplay();
    }

    bool enterSubMenu(const std::vector<MenuItem> &subMenu) {
        menuStack.emplace_back(currentMenu, menuIndex); // Save current menu and index
        currentMenu = subMenu;
        menuIndex   = 0;
        updateDisplay();
        return false;
    }

    void navigateBack() {
        if (!menuStack.empty()) {
            std::tie(currentMenu, menuIndex) = menuStack.back(); // Restore previous menu and index
            menuStack.pop_back();
            updateDisplay();
        } else {
            inMenu = false;
            updateDisplay();
        }
    }

    bool setScene(SceneType scene) {
        leds->setScene(scene);
        return true;
    }

    bool setMode(ModeType mode) {
        Badge &badge = Badge::getInstance();
        if (badge.hasMode(mode)) {
            badge.setMode(mode);
        } else {
            ESP_LOGE(TAG_HOMEMODE, "Mode not found: %d", static_cast<int>(mode));

            // Flash all 4 handshake LEDs red to indicate an error
            const int LED_FLASH_DURATION = 900;
            for (int j = 0; j < 3; ++j) {
                for (int i = 0; i < HANDSHAKE_COUNT; ++i) {
                    leds->lockLed(TOUCH, handshakeLeds.right(i), CRGB::Yellow);
                }
                leds->show();
                vTaskDelay((LED_FLASH_DURATION / 3 / 2) / portTICK_PERIOD_MS); // On duration
                for (int i = 0; i < HANDSHAKE_COUNT; ++i) {
                    leds->lockLed(TOUCH, handshakeLeds.right(i), CRGB::Black);
                }
                leds->show();
                vTaskDelay((LED_FLASH_DURATION / 3 / 2) / portTICK_PERIOD_MS); // Off duration
            }
            // Unlock the LEDs
            for (int i = 0; i < HANDSHAKE_COUNT; ++i) {
                leds->unlockLed(TOUCH, handshakeLeds.right(i));
            }

            return false;
        }

        return true;
    }

    void updateDisplay() {
        stopInitTask();
        ESP_LOGD(TAG_HOMEMODE, "Updating display");

        if (inMenu) {
            ESP_LOGD(TAG_HOMEMODE, "Showing menu");
            leds->lockNonAddressable(BOOK_EYE, true);
            display->setFont(u8g2_font_ncenB08_tr);

            std::vector<const char *> menuItems;
            for (const auto &item : currentMenu) {
                menuItems.push_back(item.name.c_str());
            }

            display->showList(menuItems.data(), menuItems.size(), menuIndex, [this](int index) {
                menuIndex = index;
                if (index == -1) {
                    navigateBack();
                } else if (index >= 0 && index < currentMenu.size()) {
                    bool goBack = currentMenu[index].action();
                    if (goBack) {
                        navigateBack();
                    } else {
                        updateDisplay();
                    }
                }
            });
        } else {
            ESP_LOGD(TAG_HOMEMODE, "Showing rock image");
            leds->unlockNonAddressable(BOOK_EYE);
            display->drawImage(ImageID::ROCK, 0, 0);
        }
    }

    void stopInitTask() {
        if (homeTaskHandle) {
            ESP_LOGD(TAG_HOMEMODE, "Stopping home init task");

            // Delete the task
            vTaskDelete(homeTaskHandle);
            homeTaskHandle = NULL;
        }
    }

    static void homeInitTask(void *pvParameters) {
        HomeMode *self = static_cast<HomeMode *>(pvParameters);

        // Set the menu flag to false in case we're coming back from somewhere else
        self->inMenu = false;

        // Start the LEDs
        ESP_LOGD(TAG_HOMEMODE, "Starting LED scene: GOING_TO_HELL");
        self->leds->clear(true);
        self->leds->setScene(SceneType::DC32_Y2K_AESTHETIC);

        // Show the title
        self->display->setFont(u8g2_font_lubB14_tf);
        self->display->showTextCentered("Mad Hatter\nAuto\nRevelator");

        // Wait for 3 seconds
        vTaskDelay(3000 / portTICK_PERIOD_MS);

        // Show the rock image
        ESP_LOGD(TAG_HOMEMODE, "Showing rock image");
        self->display->drawImage(ImageID::ROCK, 0, 0);

        // We're done now so clean up and delete the task
        ESP_LOGD(TAG_HOMEMODE, "Home init task complete");
        self->homeTaskHandle = NULL;
        vTaskDelete(nullptr);
    }
};

#endif // MODE_HOME_HPP
