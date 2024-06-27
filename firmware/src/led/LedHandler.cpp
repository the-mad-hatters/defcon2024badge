#include "LedHandler.h"
#include "sync.h"

static const char *TAG = "LedHandler";

CRGBArray<UPPER_LED_COUNT> upperStrip;
CRGBArray<LOWER_LED_COUNT> lowerStrip;
CRGBArray<MAIN_LED_COUNT> mainStrip;
CRGBArray<TOUCH_LED_COUNT> touchStrip;

void LedHandler::init() {
    if (initialized) {
        return;
    }

    // Initialize LED strips
    FastLED
        .addLeds<UPPER_LED_TYPE, UPPER_LED_PIN, UPPER_LED_COLOR_ORDER>(upperStrip.leds,
                                                                       UPPER_LED_COUNT)
        .setCorrection(TypicalLEDStrip);
    FastLED
        .addLeds<LOWER_LED_TYPE, LOWER_LED_PIN, LOWER_LED_COLOR_ORDER>(lowerStrip.leds,
                                                                       LOWER_LED_COUNT)
        .setCorrection(TypicalLEDStrip);
    FastLED
        .addLeds<MAIN_LED_TYPE, MAIN_LED_PIN, MAIN_LED_COLOR_ORDER>(mainStrip.leds, MAIN_LED_COUNT)
        .setCorrection(TypicalLEDStrip);
    FastLED
        .addLeds<TOUCH_LED_TYPE, TOUCH_LED_PIN, TOUCH_LED_COLOR_ORDER>(touchStrip.leds,
                                                                       TOUCH_LED_COUNT)
        .setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(LED_BRIGHTNESS);

    addressableStrips[UPPER] = &upperStrip;
    addressableStrips[LOWER] = &lowerStrip;
    addressableStrips[MAIN]  = &mainStrip;
    addressableStrips[TOUCH] = &touchStrip;

    // Initialize non-addressable LEDs
    pinMode(MAD_HATTER_HAT_LED_PIN, OUTPUT);
    pinMode(PAP_FIG_HAT_LED_PIN, OUTPUT);
    pinMode(ANGEL_OF_GOD_HAT_LED_PIN, OUTPUT);
    pinMode(BOOK_EYE_LED_PIN, OUTPUT);

    nonAddressableLeds[MAD_HATTER_HAT]   = MAD_HATTER_HAT_LED_PIN;
    nonAddressableLeds[PAP_FIG_HAT]      = PAP_FIG_HAT_LED_PIN;
    nonAddressableLeds[ANGEL_OF_GOD_HAT] = ANGEL_OF_GOD_HAT_LED_PIN;
    nonAddressableLeds[BOOK_EYE]         = BOOK_EYE_LED_PIN;

    // Set up locked LED tracking arrays
    for (int i = 0; i < ADDRESSABLE_STRIP_COUNT; i++) {
        lockedLeds[static_cast<AddressableStrip>(i)] = std::vector<bool>(LedCounts[i], false);
        lockedColors[static_cast<AddressableStrip>(i)] =
            std::vector<CRGB>(LedCounts[i], CRGB::Black);
        unlockColors[static_cast<AddressableStrip>(i)] =
            std::vector<CRGB>(LedCounts[i], CRGB::Black);
    }

    // Set up the scene manager
    sceneManager.init();

    initialized = true;
}

void LedHandler::setAddressable(AddressableStrip strip, int index, CRGB color) {
    std::lock_guard<std::mutex> lock(lockMutex);
    if (lockedLeds[strip][index]) {
        std::lock_guard<std::mutex> ledLock(ledMutex);
        (*addressableStrips[strip])[index] = lockedColors[strip][index];
        unlockColors[strip][index]         = color;
    } else {
        std::lock_guard<std::mutex> ledLock(ledMutex);
        (*addressableStrips[strip])[index] = color;
    }
}

void LedHandler::lockLed(AddressableStrip strip, int index, CRGB color) {
    {
        std::lock_guard<std::mutex> lock(lockMutex);
        lockedLeds[strip][index]   = true;
        lockedColors[strip][index] = color;
    }
    {
        std::lock_guard<std::mutex> lock(ledMutex);
        (*addressableStrips[strip])[index] = color;
    }
    show();
}

void LedHandler::unlockLed(AddressableStrip strip, int index) {
    {
        std::lock_guard<std::mutex> lock(lockMutex);
        lockedLeds[strip][index]   = false;
        lockedColors[strip][index] = CRGB::Black;
    }
    {
        std::lock_guard<std::mutex> lock(ledMutex);
        (*addressableStrips[strip])[index] = unlockColors[strip][index];
    }
    show();
}

void LedHandler::show() {
    if (xSemaphoreTake(peripheralSync, portMAX_DELAY) == pdTRUE) {
        std::lock_guard<std::mutex> lock(ledMutex);
        FastLED.show();
        xSemaphoreGive(peripheralSync);
    }
}

void LedHandler::clear(bool unlock) {
    {
        std::lock_guard<std::mutex> lock(ledMutex);
        for (int i = 0; i < ADDRESSABLE_STRIP_COUNT; i++) {
            for (int j = 0; j < LedCounts[i]; j++) {
                (*addressableStrips[static_cast<AddressableStrip>(i)])[j] = CRGB::Black;
            }
        }
    }
    show();
    for (int i = 0; i < NON_ADDRESSABLE_LED_COUNT; i++) {
        setNonAddressable(i, false);
    }
}

void LedHandler::setNonAddressable(int index, bool state) {
    // std::lock_guard<std::mutex> lock(ledMutex);
    digitalWrite(nonAddressableLeds[static_cast<NonAddressableLed>(index)], state ? HIGH : LOW);
}