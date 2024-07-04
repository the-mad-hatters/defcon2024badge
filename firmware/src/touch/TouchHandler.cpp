#include <Arduino.h>
#include "TouchHandler.h"
#include "led/LedHandler.h"

static const char *TAG = "TouchHandler";

// Queue to communicate touch events to BadgeManager
QueueHandle_t touchQueue;

void TouchHandler::init() {
    if (initialized) {
        return;
    }

    // Initialize touch inputs
    pinMode(HANDSHAKE_1, INPUT);
    pinMode(HANDSHAKE_2, INPUT);
    pinMode(HANDSHAKE_3, INPUT);
    pinMode(HANDSHAKE_4, INPUT);

    // Initialize touch queue
    touchQueue = xQueueCreate(10, sizeof(TouchEvent));

    // Create touch task
    xTaskCreate(touchTask, "TouchTask", 2048, this, 6, NULL);

    initialized = true;
}

void TouchHandler::touchTask(void *pvParameters) {
    TouchHandler *self                       = static_cast<TouchHandler *>(pvParameters);
    int touchThreshold                       = TOUCH_THRESHOLD;
    int touchValues[HANDSHAKE_COUNT]         = {0};
    unsigned long lastTouch[HANDSHAKE_COUNT] = {0};

    LedHandler &leds                              = LedHandler::getInstance();
    TouchEventType lastTouchType[HANDSHAKE_COUNT] = {TOUCH_UP, TOUCH_UP, TOUCH_UP, TOUCH_UP};

    while (true) {
        for (int i = 0; i < HANDSHAKE_COUNT; i++) {
            touchValues[i] = touchRead(handshakePins.right(i));
            if (lastTouch[i] == 0) {
                lastTouch[i] = millis();
            } else if (millis() - lastTouch[i] > HANDSHAKE_DEBOUNCE) {
                bool changed = false;
                if (touchValues[i] > touchThreshold) {
                    // Turn on the LED for this handshake
                    if (lastTouchType[i] != TOUCH_DOWN) {
                        leds.lockLed(TOUCH, handshakeLeds.right(i), CRGB::Green);
                        lastTouchType[i] = TOUCH_DOWN;
                        changed          = true;
                    }

                    // Send a touch event
                    TouchEvent event = {
                        .type    = TOUCH_DOWN,
                        .pin     = handshakePins.right(i),
                        .value   = touchValues[i],
                        .changed = changed,
                    };
                    xQueueSend(touchQueue, &event, portMAX_DELAY);
                } else {
                    // Turn off the LED for this handshake
                    if (lastTouchType[i] != TOUCH_UP) {
                        leds.unlockLed(TOUCH, handshakeLeds.right(i));
                        lastTouchType[i] = TOUCH_UP;
                        changed          = true;
                    }

                    // Send a touch event
                    TouchEvent event = {
                        .type    = TOUCH_UP,
                        .pin     = handshakePins.right(i),
                        .value   = touchValues[i],
                        .changed = changed,
                    };
                    xQueueSend(touchQueue, &event, portMAX_DELAY);
                }
                lastTouch[i] = 0;
            }
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}
