#ifndef MODE_HANDLE_HPP
#define MODE_HANDLE_HPP

#include "EEPROM.h"
#include "badge/BadgeMode.h"
#include "esp_log.h"

static const char *TAG_HANDLEMODE = "HandleMode";

#define HANDLE_EEPROM_ADDR 0
#define HANDLE_EEPROM_SIZE 512
#define MAX_HANDLE_LENGTH  20

class HandleMode : public BadgeMode {
  public:
    HandleMode() : BadgeMode(ModeType::DISPLAY_HANDLE), handleTaskHandle(NULL), handle{0} {
        initEEPROM();
    }

    void enter() override {
        ESP_LOGD(TAG_HANDLEMODE, "Entering Handle mode");
        loadHandle();
        ESP_LOGD(TAG_HANDLEMODE, "Loaded handle: %s", handle);

        // If the loaded handle is too long, clear it
        if (handle.length() > MAX_HANDLE_LENGTH) {
            ESP_LOGW(TAG_HANDLEMODE, "Clearing handle because it is too long");
            for (int i = 0; i < MAX_HANDLE_LENGTH; i++) {
                handle[i] = 0;
            }
            saveHandle();
        }

        // If the handle is empty, prompt for it
        if (handle[0] == 0) {
            editHandle();
        } else {
            showHandle();
        }
    }

    void exit() override {
        ESP_LOGD(TAG_HANDLEMODE, "Exiting Handle mode");
        display->clear();
    }

    void editHandle() {
        // Get the last character of the handle
        char lastChar = handle.length() ? handle.back() : CHAR_SET_ALL[0];

        // Show the prompt
        display->setFont(u8g2_font_ncenB08_tr, u8g2_font_ncenB10_tr);
        display->showTextEntry("Enter your handle", handle.c_str(), lastChar, [this](std::string newHandle) {
            if (newHandle.length() > MAX_HANDLE_LENGTH) {
                ESP_LOGW(TAG_HANDLEMODE, "Handle too long... truncating to %d characters", MAX_HANDLE_LENGTH);
                newHandle = newHandle.substr(0, MAX_HANDLE_LENGTH);
            }
            handle = newHandle;
            saveHandle();
            showHandle();
        });
    }

    void showHandle() {
        DisplayManager::TextBounds bounds = display->getTextBounds(u8g2_font_lubB14_tf, handle.c_str());
        display->setFont(u8g2_font_lubB14_tf);

        // If the handle is too long for the display, scroll it instead
        if (bounds.width > display->getDisplayWidth()) {
            display->setScrollAlignment(SCROLL_ALIGN_MIDDLE);
            display->setScrollSpeed(10);
            display->setScrollIterations(SCROLL_FOREVER);
            display->scrollText(handle.c_str());
        }
        // Otherwise, show it centered on screen
        else {
            display->showTextCentered(handle.c_str());
        }
    }

    void handleTouch(TouchEvent event) override {
        if (!event.changed || event.type != TOUCH_DOWN) {
            return;
        }
        if (event.pin == HANDSHAKE_4) {
            editHandle();
        }
    }

  private:
    TaskHandle_t handleTaskHandle;
    std::string handle;

    void initEEPROM() {
        EEPROM.begin(HANDLE_EEPROM_SIZE);
        if (EEPROM.read(HANDLE_EEPROM_ADDR + MAX_HANDLE_LENGTH) != 0) {
            ESP_LOGI(TAG_HANDLEMODE, "Initializing EEPROM");
            for (int i = 0; i < HANDLE_EEPROM_SIZE; i++) {
                EEPROM.write(i, 0);
            }
            EEPROM.commit();
        }
        EEPROM.end();
    }

    void saveHandle() {
        EEPROM.begin(HANDLE_EEPROM_SIZE);
        for (int i = 0; i < MAX_HANDLE_LENGTH; i++) {
            EEPROM.write(HANDLE_EEPROM_ADDR + i, handle[i]);
        }
        EEPROM.commit();
        EEPROM.end();
    }

    void loadHandle() {
        handle.clear();
        EEPROM.begin(HANDLE_EEPROM_SIZE);
        for (int i = 0; i < MAX_HANDLE_LENGTH; i++) {
            handle += char(EEPROM.read(HANDLE_EEPROM_ADDR + i));
        }
        EEPROM.end();
    }
};

#endif // MODE_HANDLE_HPP