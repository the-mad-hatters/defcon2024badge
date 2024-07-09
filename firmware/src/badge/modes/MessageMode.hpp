#ifndef MODE_MESSAGE_HPP
#define MODE_MESSAGE_HPP

#include <set>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <SPIFFS.h>
#include "esp_log.h"
#include "badge/BadgeMode.h"
#include "sync.h"

#define MESSAGE_SPEED 40
#define MESSAGE_FONT  u8g2_font_ncenB10_tr

static const char *TAG_MESSAGEMODE = "MessageMode";

class MessageMode : public BadgeMode {
  public:
    friend class Badge;

    MessageMode(ModeType type, const char *normalFile, const char *nsfwFile,
                const std::set<int> &nsfwPins = {HANDSHAKE_2, HANDSHAKE_3})
        : BadgeMode(type, nsfwPins), normalFile(normalFile), nsfwFile(nsfwFile) {
        loadMessages();
    }

    const char *getBaseType() const override {
        return "MessageMode";
    }

  protected:
    std::vector<std::string> messages;
    const char *normalFile;
    const char *nsfwFile;

    void loadMessages() {
        std::vector<std::string> paths = Badge::getInstance().getNSFWMode()
                                             ? std::vector<std::string>{normalFile, nsfwFile}
                                             : std::vector<std::string>{normalFile};

        // Clear the existing messages and load from the file(s)
        messages.clear();
        for (const auto &path : paths) {
            int count = 0;

            // Lock the peripheral mutex to prevent concurrent access
            if (xSemaphoreTake(peripheralSync, portMAX_DELAY) == pdTRUE) {

                if (!SPIFFS.exists(path.c_str())) {
                    ESP_LOGE(TAG_MESSAGEMODE, "Messages file not found: %s", path);
                    return;
                }
                File file = SPIFFS.open(path.c_str(), "r");
                if (!file) {
                    ESP_LOGE(TAG_MESSAGEMODE, "Failed to open messages file: %s", path);
                    return;
                }

                while (file.available()) {
                    String line = file.readStringUntil('\n');
                    line.trim();
                    if (line.length() > 0) {
                        messages.push_back(line.c_str());
                        count++;
                    }
                }
                file.close();

                // Release the peripheral mutex
                xSemaphoreGive(peripheralSync);
            }

            ESP_LOGI(TAG_MESSAGEMODE, "Loaded %d messages from %s", count, path.c_str());
        }

        if (messages.empty()) {
            ESP_LOGE(TAG_MESSAGEMODE, "No messages loaded");
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

#endif // MODE_MESSAGE_HPP
