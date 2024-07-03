#ifndef SCENE_CONFIG_H
#define SCENE_CONFIG_H

#include <unordered_map>
#include "led/LedBase.h"

// Enum for addressable LED patterns
enum class ScenePattern {
    PALETTE_FILL,
    COLOR_CYCLE,
};

// Enum for non-addressable LED patterns
enum class ScenePatternNonAddr {
    RANDOM,
};

// Struct for pattern color options
struct ColorConfig {
    CRGBPalette16 palette;
    CRGB color;
};

// Scene configuration struct for addressable LED strips
struct SceneConfigAddressable {
    ColorConfig colorConfig;
    uint8_t startIndex;
    uint8_t brightness;
    unsigned long interval;
    ScenePattern pattern;

    SceneConfigAddressable(CRGBPalette16 palette, uint8_t startIndex, uint8_t brightness,
                           unsigned long interval, ScenePattern pattern)
        : colorConfig{.palette = palette}
        , startIndex(startIndex)
        , brightness(brightness)
        , interval(interval)
        , pattern(pattern) {
        if (pattern != ScenePattern::PALETTE_FILL) {
            Serial.println("Invalid pattern for palette color config");
        }
    }

    SceneConfigAddressable(CRGB color, uint8_t startIndex, uint8_t brightness,
                           unsigned long interval, ScenePattern pattern)
        : colorConfig{.color = color}
        , startIndex(startIndex)
        , brightness(brightness)
        , interval(interval)
        , pattern(pattern) {
        if (pattern != ScenePattern::COLOR_CYCLE) {
            Serial.println("Invalid pattern for single color config");
        }
    }
};

// Scene configuration struct for non-addressable LEDs
struct SceneConfigNonAddr {
    unsigned long interval;
    ScenePatternNonAddr pattern;
};

// Type alias for addressable LED strip configurations
using SceneConfigMapAddressable = std::unordered_map<AddressableStrip, SceneConfigAddressable>;

#endif // SCENE_CONFIG_H