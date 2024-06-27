#ifndef LED_SCENE_GOING_TO_HELL_HPP
#define LED_SCENE_GOING_TO_HELL_HPP

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "led/LedBase.h"
#include "led/LedHandler.h"

static const char *TAG_GONETOHELL = "GoneToHell";

/*
 * NOTE: These aren't used yet but I copied them from the arduino v5 code
 */

const TProgmemPalette16 gone_to_hell_p PROGMEM = {
    CRGB::DarkRed, CRGB::Black, CRGB::Red,    CRGB::Black, CRGB::Maroon, CRGB::Black,
    CRGB::Black,   CRGB::Black, CRGB::Orange, CRGB::Black, CRGB::Red,    CRGB::Black,
    CRGB::Maroon,  CRGB::Black, CRGB::Black,  CRGB::Black

};

const TProgmemPalette16 gone_to_hell_main_p PROGMEM = {
    CRGB::DarkRed, CRGB::Black, CRGB::Red,    CRGB::Black, CRGB::Maroon, CRGB::Black,
    CRGB::Black,   CRGB::Black, CRGB::Orange, CRGB::Black, CRGB::Red,    CRGB::Black,
    CRGB::Maroon,  CRGB::Black, CRGB::Black,  CRGB::Black

};

class GoneToHell : public SceneBase {
  public:
    GoneToHell(LedHandler &ledHandler, int outerUpdatesPerSecond = 5, int mainUpdatesPerSecond = 15,
               int nonAddrUpdatesPerSecond = 4, CRGB ulFirstColor = CRGB::Orange,
               CRGB ulSecondColor = CRGB::Red, CRGB hsFirstColor = CRGB::Orange,
               CRGB hsSecondColor = CRGB::Purple)
        : SceneBase()
        , state(State::UL_FORWARD)
        , outerIndex(0)
        , mainIndex(0)
        , lastOuterUpdateTime(0)
        , lastMainUpdateTime(0)
        , lastNonAddrUpdateTime(0)
        , outerInterval(1000 / outerUpdatesPerSecond)
        , mainInterval(1000 / mainUpdatesPerSecond)
        , nonAddrInterval(1000 / nonAddrUpdatesPerSecond)
        , ulFirstColor(ulFirstColor)
        , ulSecondColor(ulSecondColor)
        , hsFirstColor(hsFirstColor)
        , hsSecondColor(hsSecondColor)
        , leds(ledHandler) {
        resetState();
    };

    void tick() override {
        if (millis() - lastOuterUpdateTime >= outerInterval) {
            lastOuterUpdateTime = millis();
            updateOuter();
        }
        if (millis() - lastMainUpdateTime >= mainInterval) {
            lastMainUpdateTime = millis();
            updateMain();
        }
        if (millis() - lastNonAddrUpdateTime >= nonAddrInterval) {
            lastNonAddrUpdateTime = millis();
            updateNonAddressable();
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

    int outerIndex;
    int mainIndex;
    unsigned long lastOuterUpdateTime;
    unsigned long lastMainUpdateTime;
    unsigned long lastNonAddrUpdateTime;
    int outerInterval;
    int mainInterval;
    int nonAddrInterval;

    CRGB ulFirstColor;
    CRGB ulSecondColor;
    CRGB hsFirstColor;
    CRGB hsSecondColor;

    LedHandler &leds;

    // Reset the state machine to the initial state
    void resetState() {
        state               = State::UL_FORWARD;
        outerIndex          = 0;
        lastOuterUpdateTime = millis();
    }

    // State handling for all LED animations
    void updateOuter() {
        // State machine for upper/lower and handshake LED sequences
        switch (state) {
            case State::UL_FORWARD:
                if (outerIndex < std::max(UPPER_LED_COUNT, LOWER_LED_COUNT)) {
                    updateUpperLower();
                    outerIndex++;
                } else {
                    state      = State::UL_REVERSE;
                    outerIndex = std::max(UPPER_LED_COUNT, LOWER_LED_COUNT) - 1;
                }
                break;
            case State::UL_REVERSE:
                if (outerIndex >= 0) {
                    updateUpperLower();
                    outerIndex--;
                } else {
                    state      = State::HS_FORWARD;
                    outerIndex = 0;
                }
                break;
            case State::HS_FORWARD:
                if (outerIndex < TOUCH_LED_COUNT) {
                    updateHandshake();
                    outerIndex++;
                } else {
                    state      = State::HS_REVERSE;
                    outerIndex = TOUCH_LED_COUNT - 1;
                }
                break;
            case State::HS_REVERSE:
                if (outerIndex >= 0) {
                    updateHandshake();
                    outerIndex--;
                } else {
                    resetState();
                }
                break;
        }
    }

    // Method to update the upper and lower board LEDs
    void updateUpperLower() {
        // Show the alternating LED colors
        if (outerIndex < UPPER_LED_COUNT) {
            updateOuterLED(UPPER, outerIndex);
        }
        if (outerIndex < LOWER_LED_COUNT) {
            updateOuterLED(LOWER, outerIndex);
        }
        leds.show();

        // Clear the LEDs after a short delay
        vTaskDelay(outerInterval / 2);
        if (outerIndex < UPPER_LED_COUNT) {
            updateOuterLED(UPPER, outerIndex, outerIndex % 2 == 0);
        }
        if (outerIndex < LOWER_LED_COUNT) {
            updateOuterLED(LOWER, outerIndex, outerIndex % 2 == 0);
        }
    }

    // Method to update the handshake LEDs
    void updateHandshake() {
        // Show the LED colors
        updateOuterLED(TOUCH, outerIndex);
        leds.show();

        // Clear the LEDs after a short delay
        vTaskDelay(outerInterval / 2);
        updateOuterLED(TOUCH, outerIndex, true);
    }

    // Method to update the main PCB ring LEDs - for now I'm just rendering a rainbow pattern so I
    // could test the main ring since I wasn't before
    void updateMain() {
        for (int i = 0; i < MAIN_LED_COUNT; ++i) {
            int ledIndex = (mainIndex + i) % MAIN_LED_COUNT;
            // Calculate the brightness based on the distance from the front LED
            uint8_t brightness = 255 - (i * (255 / MAIN_LED_COUNT));
            // Calculate the color based on color order (rainbow colors basically)
            CRGB color = CHSV((ledIndex * 256 / MAIN_LED_COUNT), 255, brightness);
            leds.setAddressable(MAIN, ledIndex, color);
        }
        leds.show();

        if (++mainIndex >= MAIN_LED_COUNT) {
            mainIndex = 0;
        }
    }

    // Picks a random non-addressable LED to turn on
    void updateNonAddressable() {
        int index = random(NON_ADDRESSABLE_LED_COUNT);
        for (int i = 0; i < NON_ADDRESSABLE_LED_COUNT; ++i) {
            leds.setNonAddressable(static_cast<NonAddressableLed>(i), i == index);
        }
    }

    // Helper method to update a single LED's color on the state machine LEDs
    void updateOuterLED(AddressableStrip strip, int index, bool clear = false) {
        if (outerIndex % 2 == 0) {
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
