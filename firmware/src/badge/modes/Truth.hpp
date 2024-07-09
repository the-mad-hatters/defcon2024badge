#ifndef MODE_TRUTH_HPP
#define MODE_TRUTH_HPP

#include "MessageMode.hpp"

static const char *TAG_TRUTHMODE = "TruthMode";

class TruthMode : public MessageMode {
  public:
    TruthMode() : MessageMode(ModeType::TRUTH, "/truths.txt", "/truths_nsfw.txt"), taskHandle(nullptr) {
    }

  protected:
    void enter() override {
        if (!taskHandle) {
            ESP_LOGD(TAG_TRUTHMODE, "Starting truth task");
            xTaskCreate(truthTask, "Truth Task", 2048, this, 5, &taskHandle);
        }
    }

    void leave() override {
        if (taskHandle) {
            ESP_LOGD(TAG_TRUTHMODE, "Stopping truth task");
            vTaskDelete(taskHandle);
            taskHandle = NULL;
        }
    }

  private:
    TaskHandle_t taskHandle;

    static void truthTask(void *pvParameters) {
        TruthMode *self = static_cast<TruthMode *>(pvParameters);
        ScrollEvent event;

        while (true) {
            ESP_LOGD(TAG_TRUTHMODE, "Selecting random truth");
            std::string truth = self->getRandomMessage();

            ESP_LOGD(TAG_TRUTHMODE, "Displaying truth: %s", truth.c_str());

            self->display->setFont(MESSAGE_FONT);
            self->display->setScrollSpeed(MESSAGE_SPEED);
            self->display->setScrollIterations(1);
            self->display->scrollText(truth.c_str());

            ESP_LOGD(TAG_TRUTHMODE, "Waiting for scroll event");
            while (true) {
                if (xQueueReceive(textScrollEvents, &event, portMAX_DELAY) == pdTRUE) {
                    ESP_LOGD(TAG_TRUTHMODE, "Received scroll event: %d", event.type);
                    if (event.type == ScrollEventType::SCROLL_END) {
                        self->display->clear();
                        ESP_LOGD(TAG_TRUTHMODE, "Scroll complete, waiting before next display");
                        vTaskDelay(2000 / portTICK_PERIOD_MS);
                        break;
                    }
                }
            }
            ESP_LOGD(TAG_TRUTHMODE, "Looping to next truth");
        }
    }
};

#endif // MODE_TRUTH_HPP
