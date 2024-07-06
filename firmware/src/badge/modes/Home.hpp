#ifndef MODE_HOME_HPP
#define MODE_HOME_HPP

#include <map>
#include "esp_log.h"
#include "badge/BadgeMode.h"
#include "badge/BadgeManager.h"

static const char *TAG_HOMEMODE = "HomeMode";

struct MenuItem {
    std::string name;             // Name of the menu item
    std::function<bool()> action; // Action to perform when the item is selected
                                  // Return true if the menu should navigate back, false to stay where it is
};

class HomeMode : public BadgeMode {
  public:
    HomeMode()
        : BadgeMode(ModeType::HOME)
        , homeTaskHandle(nullptr)
        , badgeStartup(true)
        , inMenu(false)
        , switchingModes(false)
        , menuIndex(0) {
        initializeMenu();
    }

    void enter() override {
        ESP_LOGD(TAG_HOMEMODE, "Entering Home mode");
        switchingModes = false;
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
        leds->unlockNonAddressable(BOOK_EYE); // In case we were in (or switched modes from) the menu
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
    bool switchingModes;
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
            {"Tapir Joyride",       [this]() { return setScene(SceneType::TAPIR_JOYRIDE); }},
            {"Suppressive Badge",   [this]() { return setScene(SceneType::SUPPRESSIVE_BADGE); }},
            {"Thetan Possession",   [this]() { return setScene(SceneType::THETAN_POSSESSION); }},
            {"Seer Stones",         [this]() { return setScene(SceneType::SEER_STONES); }},
            {"Drunk as Hell",       [this]() { return setScene(SceneType::DRUNK_AS_HELL); }},
            {"Gone Clear",          [this]() { return setScene(SceneType::GONE_CLEAR);}},
            {"American Jesus",      [this]() { return setScene(SceneType::AMERICAN_JESUS);}},
            {"DC 32",               [this]() { return setScene(SceneType::DC32_Y2K_AESTHETIC); }},
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
            // Restore previous menu and index
            std::tie(currentMenu, menuIndex) = menuStack.back();
            menuStack.pop_back();
            updateDisplay();
        } else {
            inMenu = false;
            updateDisplay();
        }
    }

    bool setScene(SceneType scene) {
        leds->setScene(scene);
        return false;
    }

    bool setMode(ModeType mode) {
        Badge &badge = Badge::getInstance();
        if (badge.hasMode(mode)) {
            switchingModes = true;
            badge.setMode(mode);
        } else {
            ESP_LOGE(TAG_HOMEMODE, "Mode not found: %d", static_cast<int>(mode));
            badge.flashLedStrip(AddressableStrip::TOUCH, CRGB::Yellow, 3, 900);
            return false;
        }

        return true;
    }

    void updateDisplay() {
        ESP_LOGD(TAG_HOMEMODE, "Updating display");

        if (switchingModes) {
            ESP_LOGD(TAG_HOMEMODE, "Switching modes, waiting for mode to change");
            display->clear();
        } else {
            if (inMenu) {
                ESP_LOGD(TAG_HOMEMODE, "Showing menu");
                leds->lockNonAddressable(BOOK_EYE, true);
                display->setFont(u8g2_font_ncenB08_tr);

                std::vector<const char *> menuItems;
                for (const auto &item : currentMenu) {
                    menuItems.push_back(item.name.c_str());
                }

                display->showList(menuItems.data(), menuItems.size(), menuIndex, [this](int index) {
                    if (index == -1) {
                        navigateBack();
                    } else if (index >= 0 && index < currentMenu.size()) {
                        menuIndex = index;
                        if (currentMenu[index].action()) {
                            navigateBack();
                        } else {
                            updateDisplay();
                        }
                    }
                });
            } else {
                ESP_LOGD(TAG_HOMEMODE, "Showing seer stones");
                leds->unlockNonAddressable(BOOK_EYE);
                display->drawImage(ImageID::ROCK, 0, 0);
            }
        }

        // Make sure the init task is stopped
        stopInitTask();
    }

    void stopInitTask() {
        if (homeTaskHandle) {
            ESP_LOGD(TAG_HOMEMODE, "Stopping home init task");

            // If called from the task itself
            if (xTaskGetCurrentTaskHandle() == homeTaskHandle) {
                ESP_LOGD(TAG_HOMEMODE, "Deleting self");
                homeTaskHandle = nullptr;
                vTaskDelete(nullptr);
            }
            // If called from outside the task
            else {
                ESP_LOGD(TAG_HOMEMODE, "Deleting task");
                vTaskDelete(homeTaskHandle);
                homeTaskHandle = nullptr;
            }
        }
    }

    static void homeInitTask(void *pvParameters) {
        HomeMode *self = static_cast<HomeMode *>(pvParameters);
        self->runInit();
    }

    void runInit() {
        // Set the menu flag to false in case we're coming back from somewhere else
        inMenu = false;

        // Start the LEDs
        ESP_LOGD(TAG_HOMEMODE, "DC 32");
        leds->clear(true);
        leds->setScene(SceneType::DC32_Y2K_AESTHETIC);

        // Show the title
        display->setFont(u8g2_font_lubB14_tf);
        display->showTextCentered("Mad Hatter\nAuto\nRevelator");

        // Wait for 3 seconds
        vTaskDelay(3000 / portTICK_PERIOD_MS);

        // Show the seer stones
        //   NOTE: as a side effect, updateDisplay() will delete this task via stopInitTask()
        updateDisplay();
    }
};

#endif // MODE_HOME_HPP
