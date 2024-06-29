#ifndef SCENE_BASE_H
#define SCENE_BASE_H

#include "LedBase.h"

class LedHandler;

class SceneBase {
  public:
    virtual void tick() = 0;

  protected:
    LedHandler &leds;

    SceneBase(LedHandler &ledHandler)
        : leds(ledHandler) //
    {
    }

    void updateLedsWithPalette(AddressableStrip strip, const CRGBPalette16 &palette,
                               uint8_t startIndex, uint8_t brightness);
    void updateNonAddressableLeds(bool state, NonAddressableLed led);
};

#endif // SCENE_BASE_H
