#ifndef MESSAGE_MODE_HPP
#define MESSAGE_MODE_HPP

#include <set>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <SPIFFS.h>
#include "esp_log.h"
#include "badge/BadgeMode.h"
#include "sync.h"

#define MESSAGE_SPEED        40
#define MESSAGE_FONT         u8g2_font_ncenB10_tr
#define PROMPT_FONT          u8g2_font_ncenB08_tr
#define NSFW_ACTIVATION_TIME 1000

static const char *TAG_MESSAGEMODE = "MessageMode";

class MessageMode : public BadgeMode {
  public:
    MessageMode(ModeType type, const char *normalFile, const char *nsfwFile,
                const std::set<int> &nsfwPins = {HANDSHAKE_2, HANDSHAKE_3})
        : BadgeMode(type)
        , taskHandle(NULL)
        , nsfwMode(false)
        , normalFile(normalFile)
        , nsfwFile(nsfwFile)
        , inputStates(touch->getInputStates())
        , latestEvents(touch->getLatestEvents()) {
        // Initialize NSFW pins
        NSFWPins.insert(nsfwPins.begin(), nsfwPins.end());

        // Calculate non-NSFW pins
        std::set<int> allPins = handshakePins.right.keys();
        std::set_difference(allPins.begin(), allPins.end(), NSFWPins.begin(), NSFWPins.end(),
                            std::inserter(nonNSFWPins, nonNSFWPins.begin()));

        // Load messages
        loadMessages(normalFile);
    }

    void enter() override {
        start();
    }

    void exit() override {
        stop();
    }

    void handleTouch(TouchEvent event) override {
        // Exit early if the event is not for one of the NSFW pins
        if (event.type != TOUCH_DOWN ||
            std::none_of(std::begin(NSFWPins), std::end(NSFWPins), [event](int pin) { return pin == event.pin; })) {
            return;
        }

        // Ensure no non-NSFW pins are touched
        if (std::any_of(nonNSFWPins.begin(), nonNSFWPins.end(),
                        [this](int pin) { return inputStates[handshakePins.right(pin)] == TOUCH_DOWN; })) {
            return;
        }

        // If all NSFW pins are touched and the duration is long enough, activate NSFW mode
        if (std::all_of(std::begin(NSFWPins), std::end(NSFWPins), [this](int pin) {
                int index = handshakePins.right[pin];
                return inputStates[index] == TOUCH_DOWN && latestEvents[index].duration > NSFW_ACTIVATION_TIME;
            })) {
            showNSFWPrompt();
        }
    }

  protected:
    TaskHandle_t taskHandle;
    std::vector<std::string> messages;
    bool nsfwMode;
    const char *normalFile;
    const char *nsfwFile;
    TouchEventType (&inputStates)[HANDSHAKE_COUNT];
    TouchEvent (&latestEvents)[HANDSHAKE_COUNT];
    std::set<int> NSFWPins;
    std::set<int> nonNSFWPins;

    virtual void start() = 0;
    virtual void stop()  = 0;

    void loadMessages(const char *path) {
        messages.clear();

        // Lock the peripheral mutex to prevent concurrent access
        if (xSemaphoreTake(peripheralSync, portMAX_DELAY) == pdTRUE) {

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

            // Release the peripheral mutex
            xSemaphoreGive(peripheralSync);
        }

        if (messages.empty()) {
            ESP_LOGE(TAG_MESSAGEMODE, "No messages loaded from %s", path);
        } else {
            ESP_LOGI(TAG_MESSAGEMODE, "Loaded %d messages from %s", messages.size(), path);
        }
    }

    void showNSFWPrompt() {
        stop();
        const char *options[] = {"Activate", "Deactivate"};
        display->setFont(PROMPT_FONT);
        display->showPrompt("NSFW mode", options, 2, nsfwMode ? 1 : 0, [this](int index) {
            // Set NSFW mode based on the selected index
            bool previousNsfwMode = nsfwMode;
            nsfwMode              = index == 0;

            if (previousNsfwMode != nsfwMode) {
                // Update the messages and display them accordingly
                const char *message = nsfwMode ? "NSFW loading..." : "Back to safety";
                display->setFont(PROMPT_FONT);
                display->showTextCentered(message);
                vTaskDelay(1000 / portTICK_PERIOD_MS); // Wait briefly so the message can be
                                                       // read
                loadMessages(nsfwMode ? nsfwFile : normalFile);
            }
            start();
        });
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
