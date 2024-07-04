#ifndef TOUCH_HANDLER_H
#define TOUCH_HANDLER_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "config.h"
#include "util.h"

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

// Map of handshake index to GPIO pin
using HandshakePins               = BiMap<int, int>;
const HandshakePins handshakePins = {
    {0, HANDSHAKE_1},
    {1, HANDSHAKE_2},
    {2, HANDSHAKE_3},
    {3, HANDSHAKE_4},
};

// Map of handshake index to LED index
using HandshakeLeds               = BiMap<int, int>;
const HandshakeLeds handshakeLeds = {
    {0, 3},
    {1, 2},
    {2, 1},
    {3, 0},
};

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
