#ifndef MODE_REVELATION_HPP
#define MODE_REVELATION_HPP

#include "MessageMode.hpp"

static const char *TAG_REVELATIONMODE = "RevelationMode";

class RevelationMode : public MessageMode {
  public:
    RevelationMode()
        : MessageMode(ModeType::REVELATION, "/revelations.txt", "/revelations_nsfw.txt"), taskHandle(nullptr) {
    }

  protected:
    void enter() override {
        if (!taskHandle) {
            xTaskCreate(revelationTask, "Revelation Task", 2048, this, 5, &taskHandle);
        }
    }

    void leave() override {
        if (taskHandle) {
            vTaskDelete(taskHandle);
            taskHandle = NULL;
        }
    }

  private:
    TaskHandle_t taskHandle;

    static void revelationTask(void *pvParameters) {
        RevelationMode *self = static_cast<RevelationMode *>(pvParameters);
        ScrollEvent event;

        while (true) {
            std::string revelation = self->getRandomMessage();

            self->display->setFont(MESSAGE_FONT);
            self->display->setScrollSpeed(MESSAGE_SPEED);
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
