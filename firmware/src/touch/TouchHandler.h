#ifndef TOUCH_HANDLER_H
#define TOUCH_HANDLER_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "config.h"

typedef enum {
    TOUCH_DOWN,
    TOUCH_UP,
} TouchEventType;

typedef struct {
    TouchEventType type;
    int pin;
    int value;
    bool changed;
} TouchEvent;

extern QueueHandle_t touchQueue;

class TouchHandler {
  public:
    static TouchHandler &getInstance() {
        static TouchHandler instance;
        return instance;
    }
    void init();
    static void touchTask(void *pvParameters);

  private:
    TouchHandler()                                = default;
    TouchHandler(const TouchHandler &)            = delete;
    TouchHandler &operator=(const TouchHandler &) = delete;
    bool initialized                              = false;
};

#endif // TOUCH_HANDLER_H
