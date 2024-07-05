#ifndef MESSAGE_MODE_HPP
#define MESSAGE_MODE_HPP

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <SPIFFS.h>
#include "badge/BadgeMode.h"
#include "esp_log.h"

#define MESSAGE_FONT u8g2_font_ncenB10_tr
#define PROMPT_FONT  u8g2_font_ncenB08_tr

static const char *TAG_MESSAGEMODE = "MessageMode";

class MessageMode : public BadgeMode {
  public:
    MessageMode(DisplayManager *display, LedHandler *leds, TouchHandler *touch,
                const char *normalFile, const char *nsfwFile)
        : taskHandle(NULL), nsfwMode(false), normalFile(normalFile), nsfwFile(nsfwFile) {
        this->display = display;
        this->leds    = leds;
        this->touch   = touch;

        // Initialize input state and hold start time
        for (int i = 0; i < HANDSHAKE_COUNT; i++) {
            inputState[i]    = TOUCH_UP;
            holdStartTime[i] = 0;
        }

        // Load messages
        loadMessages(normalFile);
    }

    void enter() override {
        startTask();
    }

    void exit() override {
        stopTask();
    }

    void handleTouch(TouchEvent event) override {
        if (inputState[event.pin] != event.type) {
            // Update the input state and hold start time
            inputState[event.pin]    = event.type;
            holdStartTime[event.pin] = event.type == TOUCH_UP ? 0 : millis();
        }

        handleNSFWToggle();
    }

  protected:
    TaskHandle_t taskHandle;
    std::vector<std::string> messages;
    bool nsfwMode;
    const char *normalFile;
    const char *nsfwFile;
    TouchEventType inputState[HANDSHAKE_COUNT];
    unsigned long holdStartTime[HANDSHAKE_COUNT];

    virtual void startTask() = 0;
    virtual void stopTask()  = 0;

    void loadMessages(const char *path) {
        messages.clear();
        if (!SPIFFS.exists(path)) {
            ESP_LOGE(TAG_MESSAGEMODE, "Messages file not found: %s", path);
            return;
        }
        File file = SPIFFS.open(path);
        if (!file) {
            ESP_LOGE(TAG_MESSAGEMODE, "Failed to open messages file: %s", path);
            return;
        }

        while (file.available()) {
            String line = file.readStringUntil('\n');
            if (line.length() > 0) {
                messages.push_back(line.c_str());
            }
        }
        file.close();

        if (messages.empty()) {
            ESP_LOGE(TAG_MESSAGEMODE, "No messages loaded from %s", path);
        } else {
            ESP_LOGI(TAG_MESSAGEMODE, "Loaded %d messages from %s", messages.size(), path);
        }
    }

    void handleNSFWToggle() {
        if (inputState[HANDSHAKE_2] == TOUCH_DOWN && inputState[HANDSHAKE_3] == TOUCH_DOWN) {
            if (holdStartTime[HANDSHAKE_2] == 0 || holdStartTime[HANDSHAKE_3] == 0) {
                return;
            }
            unsigned long currentTime = millis();
            if (currentTime - holdStartTime[HANDSHAKE_2] >= 1000 &&
                currentTime - holdStartTime[HANDSHAKE_3] >= 1000) {
                stopTask();
                const char *options[] = {"Activate", "Deactivate"};
                touch->clearEvents();
                display->setFont(PROMPT_FONT);
                display->showPrompt("NSFW mode", options, 2, nsfwMode ? 1 : 0, [this](int index) {
                    // Set NSFW mode based on the selected index
                    bool previousNsfwMode = nsfwMode;
                    nsfwMode              = index == 0;

                    // Make sure we reset the hold start times
                    holdStartTime[HANDSHAKE_2] = 0;
                    holdStartTime[HANDSHAKE_3] = 0;

                    if (previousNsfwMode != nsfwMode) {
                        // Update the messages and display them accordingly
                        const char *message = nsfwMode ? "NSFW loading..." : "Back to safety";
                        display->setFont(PROMPT_FONT);
                        display->showTextCentered(message);
                        vTaskDelay(1500 / portTICK_PERIOD_MS); // Wait briefly so the message can be
                                                               // read
                        loadMessages(nsfwMode ? nsfwFile : normalFile);
                    }
                    startTask();
                });
            }
        }
    }

    std::string getRandomMessage() {
        if (messages.empty()) {
            ESP_LOGE(TAG_MESSAGEMODE, "No messages loaded");
            return "No messages available";
        }
        int index = random(0, messages.size());
        ESP_LOGD(TAG_MESSAGEMODE, "Selected random message index: %d", index);
        return messages[index];
    }
};

#endif // MESSAGE_MODE_HPP
