#ifndef LED_SCENE_GOING_TO_HELL_HPP
#define LED_SCENE_GOING_TO_HELL_HPP

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "led/LedBase.h"
#include "led/LedHandler.h"

static const char *TAG_GOINGTOHELL = "GoingToHell";

class GoingToHell : public SceneBase {
  public:
    GoingToHell(LedHandler &ledHandler, int updatesPerSecond = 5, CRGB ulFirstColor = CRGB::Orange,
                CRGB ulSecondColor = CRGB::Red, CRGB hsFirstColor = CRGB::Orange,
                CRGB hsSecondColor = CRGB::Purple)
        : SceneBase()
        , state(State::UL_FORWARD)
        , currentIndex(0)
        , lastUpdateTime(0)
        , updatesPerSecond(updatesPerSecond)
        , interval(666 / updatesPerSecond)
        , ulFirstColor(ulFirstColor)
        , ulSecondColor(ulSecondColor)
        , hsFirstColor(hsFirstColor)
        , hsSecondColor(hsSecondColor)
        , leds(ledHandler) {
        resetState();
    };

    void tick() override {
        if (millis() - lastUpdateTime >= interval) {
            lastUpdateTime = millis();
            updateState();
        }
    }

  private:
    enum class State {
        // Upper and lower board LEDs
        UL_FORWARD,
        UL_REVERSE,

        // Handshake LEDs
        HS_FORWARD,
        HS_REVERSE,
    };
    State state;

    int currentIndex;
    unsigned long lastUpdateTime;
    int updatesPerSecond;
    int interval;

    CRGB ulFirstColor;
    CRGB ulSecondColor;
    CRGB hsFirstColor;
    CRGB hsSecondColor;

    LedHandler &leds;

    // Reset the state machine to the initial state
    void resetState() {
        state          = State::UL_FORWARD;
        currentIndex   = 0;
        lastUpdateTime = millis();
    }

    // State machine to update the LEDs and transition between states
    void updateState() {
        switch (state) {
            case State::UL_FORWARD:
                if (currentIndex < std::max(UPPER_LED_COUNT, LOWER_LED_COUNT)) {
                    updateUpperLower();
                    currentIndex++;
                } else {
                    state        = State::UL_REVERSE;
                    currentIndex = std::max(UPPER_LED_COUNT, LOWER_LED_COUNT) - 1;
                }
                break;
            case State::UL_REVERSE:
                if (currentIndex >= 0) {
                    updateUpperLower();
                    currentIndex--;
                } else {
                    state        = State::HS_FORWARD;
                    currentIndex = 0;
                }
                break;
            case State::HS_FORWARD:
                if (currentIndex < TOUCH_LED_COUNT) {
                    updateHandshake();
                    currentIndex++;
                } else {
                    state        = State::HS_REVERSE;
                    currentIndex = TOUCH_LED_COUNT - 1;
                }
                break;
            case State::HS_REVERSE:
                if (currentIndex >= 0) {
                    updateHandshake();
                    currentIndex--;
                } else {
                    resetState();
                }
                break;
        }
    }

    // Method to update the upper and lower board LEDs
    void updateUpperLower() {
        // Show the alternating LED colors
        if (currentIndex < UPPER_LED_COUNT) {
            updateLED(UPPER, currentIndex);
        }
        if (currentIndex < LOWER_LED_COUNT) {
            updateLED(LOWER, currentIndex);
        }
        leds.show();

        // Clear the LEDs after a short delay
        vTaskDelay(interval / 2);
        if (currentIndex < UPPER_LED_COUNT) {
            updateLED(UPPER, currentIndex, currentIndex % 2 == 0);
        }
        if (currentIndex < LOWER_LED_COUNT) {
            updateLED(LOWER, currentIndex, currentIndex % 2 == 0);
        }
    }

    // Method to update the handshake LEDs
    void updateHandshake() {
        // Show the LED colors
        updateLED(TOUCH, currentIndex);
        leds.show();

        // Clear the LEDs after a short delay
        vTaskDelay(interval / 2);
        updateLED(TOUCH, currentIndex, true);
    }

    // Helper method to update a single LED's color
    void updateLED(AddressableStrip strip, int index, bool clear = false) {
        if (currentIndex % 2 == 0) {
            leds.setAddressable(strip, index,
                                clear ? CRGB::Black
                                      : (state < State::HS_FORWARD ? ulFirstColor : hsFirstColor));
        } else {
            leds.setAddressable(
                strip, index,
                clear ? CRGB::Black : (state < State::HS_FORWARD ? ulSecondColor : hsSecondColor));
        }
    }
};

#endif // LED_SCENE_GOING_TO_HELL_HPP