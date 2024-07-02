#ifndef MODE_TRUTH_HPP
#define MODE_TRUTH_HPP

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <SPIFFS.h>
#include "badge/BadgeMode.h"
#include "esp_log.h"

static const char *TAG_TRUTHMODE = "TruthMode";

class TruthMode : public BadgeMode {
  public:
    TruthMode(DisplayManager *display, LedHandler *leds, TouchHandler *touch)
        : truthTaskHandle(NULL), nsfwMode(false) {
        this->display = display;
        this->leds    = leds;
        this->touch   = touch;

        // Initialize input state and hold start time
        for (int i = 0; i < HANDSHAKE_COUNT; i++) {
            inputState[i]    = TOUCH_UP;
            holdStartTime[i] = 0;
        }

        // Load truths
        loadTruths("/truths.txt");
    }

    void enter() override {
        ESP_LOGD(TAG_TRUTHMODE, "Entering Truth mode");
        startTruthTask();
    }

    void exit() override {
        ESP_LOGD(TAG_TRUTHMODE, "Exiting Truth mode");
        stopTruthTask();
    }

    static void truthTask(void *pvParameters) {
        TruthMode *self = static_cast<TruthMode *>(pvParameters);
        ScrollEvent event;

        while (true) {
            if (self->truths.empty()) {
                ESP_LOGE(TAG_TRUTHMODE, "No truths loaded");
                self->truthTaskHandle = NULL;
                vTaskDelete(NULL);
                return;
            }

            // Randomly pick a truth
            int index         = random(0, self->truths.size());
            const char *truth = self->truths[index].c_str();

            ESP_LOGD(TAG_TRUTHMODE, "Displaying truth: %s", truth);

            // Display the truth
            self->display->setFont(u8g2_font_ncenB08_tr);
            self->display->setScrollSpeed(10);
            self->display->setScrollIterations(1);
            self->display->scrollText(truth);

            // Wait for the scroll to end
            while (true) {
                if (xQueueReceive(textScrollEvents, &event, portMAX_DELAY) == pdTRUE) {
                    if (event.type == ScrollEventType::SCROLL_END) {
                        self->display->clear();
                        vTaskDelay(2000 / portTICK_PERIOD_MS); // Wait briefly before displaying the
                                                               // next truth
                        break;
                    }
                }
            }
        }
    }

    void handleTouch(TouchEvent event) override {
        if (inputState[event.pin] != event.type) {
            // Update the input state and hold start time
            inputState[event.pin]    = event.type;
            holdStartTime[event.pin] = event.type == TOUCH_UP ? 0 : millis();
            ESP_LOGD(TAG_TRUTHMODE, "Touch event: %d %s", event.pin,
                     event.type == TOUCH_DOWN ? "DOWN" : "UP");
        }

        handleNSFWToggle();
    }

  private:
    TaskHandle_t truthTaskHandle;
    std::vector<std::string> truths;
    bool nsfwMode;
    TouchEventType inputState[HANDSHAKE_COUNT];
    unsigned long holdStartTime[HANDSHAKE_COUNT];

    void startTruthTask() {
        if (!truthTaskHandle) {
            ESP_LOGD(TAG_TRUTHMODE, "Starting truth task");
            xTaskCreate(truthTask, "Truth Task", 2048, this, 5, &truthTaskHandle);
        } else {
            ESP_LOGW(TAG_TRUTHMODE, "Truth task appears to be already running");
        }
    }

    void stopTruthTask() {
        if (truthTaskHandle) {
            ESP_LOGD(TAG_TRUTHMODE, "Stopping truth task");
            vTaskDelete(truthTaskHandle);
            truthTaskHandle = NULL;
        }
    }

    void loadTruths(const char *path) {
        truths.clear();
        if (!SPIFFS.exists(path)) {
            ESP_LOGE(TAG_TRUTHMODE, "Truths file not found");
            return;
        }
        File file = SPIFFS.open(path);
        if (!file) {
            ESP_LOGE(TAG_TRUTHMODE, "Failed to open truths file");
            return;
        }

        while (file.available()) {
            String line = file.readStringUntil('\n');
            if (line.length() > 0) {
                truths.push_back(line.c_str());
            }
        }
        file.close();

        if (truths.empty()) {
            ESP_LOGE(TAG_TRUTHMODE, "No truths loaded from %s", path);
        } else {
            ESP_LOGI(TAG_TRUTHMODE, "Loaded %d truths from %s", truths.size(), path);
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
                stopTruthTask();
                const char *options[] = {"Activate", "Deactivate"};
                display->setFont(u8g2_font_ncenB08_tr);
                display->showPrompt("NSFW mode", options, 2, nsfwMode ? 1 : 0, [this](int index) {
                    // Set NSFW mode based on the selected index
                    bool previousNsfwMode = nsfwMode;
                    nsfwMode              = index == 0;

                    // Make sure we reset the hold start times
                    holdStartTime[HANDSHAKE_2] = 0;
                    holdStartTime[HANDSHAKE_3] = 0;

                    if (previousNsfwMode != nsfwMode) {
                        // Update the truths and display them accordingly
                        ESP_LOGD(TAG_TRUTHMODE, "NSFW mode %s",
                                 nsfwMode ? "activated" : "deactivated");
                        display->setFont(u8g2_font_ncenB08_tr);
                        display->showTextCentered(nsfwMode ? "NSFW loading..." : "Back to safety");
                        vTaskDelay(1500 / portTICK_PERIOD_MS); // Wait briefly so the message can be
                                                               // read
                        if (nsfwMode) {
                            loadTruths("/truths_nsfw.txt");
                        } else {
                            loadTruths("/truths.txt");
                        }
                    }
                    startTruthTask();
                });
            }
        }
    }
};

#endif // MODE_TRUTH_HPP
