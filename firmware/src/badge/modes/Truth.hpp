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
    TruthMode(DisplayManager *display, LedHandler *leds, TouchHandler *touch) {
        this->display = display;
        this->leds    = leds;
        this->touch   = touch;

        // Load truths from file
        if (!SPIFFS.exists("/truths.txt")) {
            ESP_LOGE(TAG_TRUTHMODE, "Truths file not found");
            return;
        }
        File file = SPIFFS.open("/truths.txt");
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
            ESP_LOGE(TAG_TRUTHMODE, "No truths loaded");
        } else {
            ESP_LOGI(TAG_TRUTHMODE, "Loaded %d truths", truths.size());
        }
    }

    void enter() override {
        ESP_LOGD(TAG_TRUTHMODE, "Entering Truth mode");
        xTaskCreate(truthTask, "Truth Task", 2048, this, 5, &truthTaskHandle);
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
            self->display->scrollText(u8g2_font_ncenB08_tr, truth, 10, 1);

            // Wait for the scroll to end
            while (true) {
                if (xQueueReceive(textScrollEvents, &event, portMAX_DELAY) == pdTRUE) {
                    if (event.type == ScrollEventType::SCROLL_END) {
                        self->display->clear();
                        vTaskDelay(2000 / portTICK_PERIOD_MS); // Wait for a while before displaying
                                                               // the next truth
                        break;
                    }
                }
            }
        }
    }

    void stopTruthTask() {
        if (truthTaskHandle) {
            ESP_LOGD(TAG_TRUTHMODE, "Stopping truth task");
            vTaskDelete(truthTaskHandle);
            truthTaskHandle = NULL;
        }
    }

    void handleTouch(TouchEvent event) override {
    }

  private:
    TaskHandle_t truthTaskHandle;
    std::vector<std::string> truths;
};

#endif // MODE_TRUTH_HPP
