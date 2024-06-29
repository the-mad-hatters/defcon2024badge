#include "SceneBase.h"
#include "LedHandler.h"

void SceneBase::updateLedsWithPalette(AddressableStrip strip, const CRGBPalette16 &palette,
                                      uint8_t startIndex, uint8_t brightness) {
    for (int i = 0; i < LedCounts[strip]; i++) {
        uint8_t colorIndex = startIndex + (i * 255 / LedCounts[strip]);
        leds.setAddressable(strip, i, ColorFromPalette(palette, colorIndex, brightness));
    }
}

void SceneBase::updateNonAddressableLeds(bool state, NonAddressableLed led) {
    leds.setNonAddressable(led, state);
}