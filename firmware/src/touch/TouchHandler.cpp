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
    unsigned long currentTime                = 0;

    LedHandler &leds = LedHandler::getInstance();

    while (true) {
        for (int i = 0; i < HANDSHAKE_COUNT; i++) {
            touchValues[i] = touchRead(handshakePins.left[i]);
            if (lastTouch[i] == 0) {
                lastTouch[i] = millis();
            } else if ((currentTime = millis()) - lastTouch[i] > HANDSHAKE_DEBOUNCE) {
                bool changed = false;
                if (touchValues[i] > touchThreshold) {
                    // Turn on the LED for this handshake
                    if (self->inputStates[i] != TOUCH_DOWN) {
                        leds.lockLed(TOUCH, handshakeLeds.left[i], CRGB::Green);
                        self->inputStates[i]    = TOUCH_DOWN;
                        self->holdStartTimes[i] = currentTime;
                        changed                 = true;
                    }

                    // Send a touch event
                    TouchEvent event = {
                        .type     = TOUCH_DOWN,
                        .pin      = handshakePins.left[i],
                        .value    = touchValues[i],
                        .changed  = changed,
                        .duration = currentTime - self->holdStartTimes[i],
                    };
                    self->latestEvents[i] = event;
                    xQueueSend(touchQueue, &event, portMAX_DELAY);
                } else {
                    // Turn off the LED for this handshake
                    if (self->inputStates[i] != TOUCH_UP) {
                        leds.unlockLed(TOUCH, handshakeLeds.left[i]);
                        self->inputStates[i]    = TOUCH_UP;
                        self->holdStartTimes[i] = 0;
                        changed                 = true;
                    }

                    // Send a touch event
                    TouchEvent event = {
                        .type     = TOUCH_UP,
                        .pin      = handshakePins.left[i],
                        .value    = touchValues[i],
                        .changed  = changed,
                        .duration = 0,
                    };
                    self->latestEvents[i] = event;
                    xQueueSend(touchQueue, &event, portMAX_DELAY);
                }
                lastTouch[i] = 0;
            }
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void TouchHandler::clearEvents(bool downOnly) {
    // Clear only touch down events from the queue
    if (downOnly) {
        TouchEvent event;
        while (xQueueReceive(touchQueue, &event, 0) == pdTRUE) {
            if (event.type == TOUCH_DOWN) {
                xQueueSendToFront(touchQueue, &event, portMAX_DELAY);
            }
        }
    }
    // Clear all touch events from the queue
    else {
        xQueueReset(touchQueue);
    }

    // Clear input states and last events
    for (int i = 0; i < HANDSHAKE_COUNT; i++) {
        if (!downOnly || inputStates[i] == TOUCH_DOWN) {
            inputStates[i]  = TOUCH_UP;
            latestEvents[i] = {TOUCH_UP, handshakePins.left[i], 0, false, 0};
        }
    }
}

TouchEvent (&TouchHandler::getLatestEvents())[HANDSHAKE_COUNT] {
    return latestEvents;
}

TouchEventType (&TouchHandler::getInputStates())[HANDSHAKE_COUNT] {
    return inputStates;
}