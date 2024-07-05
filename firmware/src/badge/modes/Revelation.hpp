#ifndef MODE_REVELATION_HPP
#define MODE_REVELATION_HPP

#include "MessageMode.hpp"

static const char *TAG_REVELATIONMODE = "RevelationMode";

class RevelationMode : public MessageMode {
  public:
    RevelationMode(DisplayManager *display, LedHandler *leds, TouchHandler *touch)
        : MessageMode(display, leds, touch, "/revelations.txt", "/revelations_nsfw.txt") {
    }

  protected:
    void startTask() override {
        if (!taskHandle) {
            xTaskCreate(revelationTask, "Revelation Task", 2048, this, 5, &taskHandle);
        }
    }

    void stopTask() override {
        if (taskHandle) {
            vTaskDelete(taskHandle);
            taskHandle = NULL;
        }
    }

    static void revelationTask(void *pvParameters) {
        RevelationMode *self = static_cast<RevelationMode *>(pvParameters);
        ScrollEvent event;

        while (true) {
            std::string revelation = self->getRandomMessage();

            self->display->setFont(MESSAGE_FONT);
            self->display->setScrollSpeed(50);
            self->display->setScrollIterations(1);
            self->display->scrollText(revelation.c_str());

            while (true) {
                if (xQueueReceive(textScrollEvents, &event, portMAX_DELAY) == pdTRUE) {
                    if (event.type == ScrollEventType::SCROLL_END) {
                        self->display->clear();
                        vTaskDelay(2000 / portTICK_PERIOD_MS);
                        break;
                    }
                }
            }
        }
    }
};

#endif // MODE_REVELATION_HPP
