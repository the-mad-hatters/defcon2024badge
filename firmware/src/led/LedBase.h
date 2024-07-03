#ifndef LED_BASE_H
#define LED_BASE_H

#define FASTLED_INTERNAL // Disable pragma message

#include <unordered_map>
#include "FastLED.h"
#include "config.h"

/**
 * @brief Addressable LED strips
 */
enum AddressableStrip {
    UPPER, // Upper "Mad Hatter" PCB LEDs
    LOWER, // Lower "Auto Revelator" PCB LEDs
    MAIN,  // Main PCB ring
    TOUCH, // Touch button LEDs
};
#define ADDRESSABLE_STRIP_COUNT 4
const uint8_t LedCounts[ADDRESSABLE_STRIP_COUNT] = {
    UPPER_LED_COUNT,
    LOWER_LED_COUNT,
    MAIN_LED_COUNT,
    TOUCH_LED_COUNT,
};

/**
 * @brief Non-addressable LEDs
 */
enum NonAddressableLed {
    MAD_HATTER_HAT,   // LED behind the mad hatter's hat
    PAP_FIG_HAT,      // LED behind the hat on the papyrus figure
    ANGEL_OF_GOD_HAT, // LED behind the hat on the angel of god figure
    BOOK_EYE,         // LED behind the eye on the book
};
#define NON_ADDRESSABLE_LED_COUNT 4

/**
 * @brief Addressable LED strip type alias
 */
using AddressableStrips = std::unordered_map<AddressableStrip, CPixelView<CRGB> *>;

/**
 * @brief Non-addressable LED type alias
 */
using NonAddressableLeds = std::unordered_map<NonAddressableLed, uint8_t>;

#endif // LED_BASE_H