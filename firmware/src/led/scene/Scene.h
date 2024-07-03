#ifndef SCENE_BASE_H
#define SCENE_BASE_H

#include "SceneConfig.h"

// Forward declaration to avoid circular dependency
class LedHandler;

class Scene {
  public:
    Scene(const SceneConfigMapAddressable &addressableConfig,
          const SceneConfigNonAddr &nonAddrConfig);

    void tick();

  private:
    LedHandler &leds;
    SceneConfigMapAddressable addressableConfig;
    SceneConfigNonAddr nonAddrConfig;
    std::unordered_map<AddressableStrip, unsigned long> lastUpdateTime;
    unsigned long lastUpdateTimeNonAddr;

    //
    // Pattern update methods
    //

    void updateLedsWithPalette(AddressableStrip strip, const CRGBPalette16 &palette,
                               uint8_t startIndex, uint8_t brightness);
    void updateLedsCycleColorNext(AddressableStrip strip, const CRGB &color, uint8_t startIndex,
                                  uint8_t brightness);
    void updateNonAddressableLedRandom();
};

#endif // SCENE_BASE_H
