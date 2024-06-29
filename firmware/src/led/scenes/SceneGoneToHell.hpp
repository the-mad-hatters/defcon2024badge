#ifndef LED_SCENE_GONETOHELL_HPP
#define LED_SCENE_GONETOHELL_HPP

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "led/LedBase.h"
#include "led/LedHandler.h"

static const char *TAG_GONETOHELL = "GoneToHell";

const TProgmemPalette16 gone_to_hell_p PROGMEM = {
    CRGB::DarkRed, CRGB::Black, CRGB::Red,    CRGB::Black, CRGB::Maroon, CRGB::Black,
    CRGB::Black,   CRGB::Black, CRGB::Orange, CRGB::Black, CRGB::Red,    CRGB::Black,
    CRGB::Maroon,  CRGB::Black, CRGB::Black,  CRGB::Black,
};

const TProgmemPalette16 gone_to_hell_main_p PROGMEM = {
    CRGB::DarkRed, CRGB::Black, CRGB::Red,    CRGB::Black, CRGB::Maroon, CRGB::Black,
    CRGB::Black,   CRGB::Black, CRGB::Orange, CRGB::Black, CRGB::Red,    CRGB::Black,
    CRGB::Maroon,  CRGB::Black, CRGB::Black,  CRGB::Black,
};

class GoneToHell : public SceneBase {
  public:
    GoneToHell(LedHandler &ledHandler, int outerUpdatesPerSecond = 5, int mainUpdatesPerSecond = 15,
               int nonAddrUpdatesPerSecond = 4)
        : SceneBase(ledHandler)
        , state(State::UL_FORWARD)
        , outerIndex(0)
        , mainIndex(0)
        , lastOuterUpdateTime(0)
        , lastMainUpdateTime(0)
        , lastNonAddrUpdateTime(0)
        , outerInterval(1000 / outerUpdatesPerSecond)
        , mainInterval(1000 / mainUpdatesPerSecond)
        , nonAddrInterval(1000 / nonAddrUpdatesPerSecond)
        , outerPalette(gone_to_hell_p)
        , mainPalette(gone_to_hell_main_p) {
        resetState();
    }

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

    CRGBPalette16 outerPalette;
    CRGBPalette16 mainPalette;

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
        updateLedsWithPalette(UPPER, outerPalette, outerIndex, UPPER_LED_BASE_BRIGHTNESS);
        updateLedsWithPalette(LOWER, outerPalette, outerIndex, LOWER_LED_BASE_BRIGHTNESS);
        leds.show();
    }

    // Method to update the handshake LEDs
    void updateHandshake() {
        updateLedsWithPalette(TOUCH, outerPalette, outerIndex, TOUCH_LED_BASE_BRIGHTNESS);
        leds.show();
    }

    // Method to update the main PCB ring LEDs
    void updateMain() {
        updateLedsWithPalette(MAIN, mainPalette, mainIndex, MAIN_LED_BASE_BRIGHTNESS);
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
};

#endif // LED_SCENE_GONETOHELL_HPP
