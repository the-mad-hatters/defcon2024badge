#ifndef SCENE_PALETTES_H
#define SCENE_PALETTES_H

#include "led/LedBase.h"

// Mad Hatter Palette
const TProgmemPalette16 mad_hatter_p PROGMEM = {
    CRGB::DarkOrange, CRGB::Black, CRGB::Red,   CRGB::Black, CRGB::GreenYellow, CRGB::Black,
    CRGB::Black,      CRGB::Black, CRGB::Green, CRGB::Black, CRGB::Red,         CRGB::Black,
    CRGB::Blue,       CRGB::Black, CRGB::Black, CRGB::Black

};

const TProgmemPalette16 mad_hatter_main_p PROGMEM = {
    CRGB::DarkOrange, CRGB::Black, CRGB::Red,   CRGB::Black, CRGB::GreenYellow, CRGB::Black,
    CRGB::Black,      CRGB::Black, CRGB::Green, CRGB::Black, CRGB::Red,         CRGB::Black,
    CRGB::Blue,       CRGB::Black, CRGB::Black, CRGB::Black

};

// Celestial Clouds Palette
const TProgmemPalette16 celestial_clouds_p PROGMEM = {
    CRGB::Navy, CRGB::Black, CRGB::Navy, CRGB::Black, CRGB::Snow, CRGB::Black,
    CRGB::Snow, CRGB::Black, CRGB::Navy, CRGB::Black, CRGB::Navy, CRGB::Black,
    CRGB::Snow, CRGB::Black, CRGB::Snow, CRGB::Black};

const TProgmemPalette16 celestial_clouds_main_p PROGMEM = {
    CRGB::Navy,  CRGB::Black, CRGB::Red,   CRGB::Black, CRGB::Navy, CRGB::Black,
    CRGB::Black, CRGB::Black, CRGB::Navy,  CRGB::Black, CRGB::Red,  CRGB::Black,
    CRGB::Navy,  CRGB::Black, CRGB::Black, CRGB::Black

};

// Gone to Hell Palette
const TProgmemPalette16 gone_to_hell_p PROGMEM = {
    CRGB::DarkRed, CRGB::Black, CRGB::Red,    CRGB::Black, CRGB::Maroon, CRGB::Black,
    CRGB::Black,   CRGB::Black, CRGB::Orange, CRGB::Black, CRGB::Red,    CRGB::Black,
    CRGB::Maroon,  CRGB::Black, CRGB::Black,  CRGB::Black

};

const TProgmemPalette16 gone_to_hell_main_p PROGMEM = {
    CRGB::Maroon,     CRGB::Black, CRGB::Maroon, CRGB::Black, CRGB::DarkOrange, CRGB::Black,
    CRGB::Maroon,     CRGB::Black, CRGB::Maroon, CRGB::Black, CRGB::Maroon,     CRGB::Black,
    CRGB::DarkOrange, CRGB::Black, CRGB::Maroon, CRGB::Black

};

// Secret Combinations Palette
const TProgmemPalette16 secret_combinations_p PROGMEM = {
    CRGB::Gold,        CRGB::Black, CRGB::Red,   CRGB::Black, CRGB::GreenYellow, CRGB::Black,
    CRGB::Black,       CRGB::Black, CRGB::Gold,  CRGB::Black, CRGB::Red,         CRGB::Black,
    CRGB::GreenYellow, CRGB::Black, CRGB::Black, CRGB::Black

};

// The Salamander Palette
const TProgmemPalette16 the_salamander_p PROGMEM = {
    CRGB::Chartreuse, CRGB::Black, CRGB::Red,         CRGB::Black, CRGB::DarkGreen, CRGB::Black,
    CRGB::OliveDrab,  CRGB::Black, CRGB::ForestGreen, CRGB::Black, CRGB::Red,       CRGB::Black,
    CRGB::LawnGreen,  CRGB::Black, CRGB::OliveDrab,   CRGB::Black

};

// Seer Stones Palettes
const TProgmemPalette16 seer_stones_p PROGMEM = {
    CRGB::Black,  CRGB::Black, CRGB::Red,    CRGB::Black, CRGB::Black, CRGB::Black,
    CRGB::Indigo, CRGB::Black, CRGB::Black,  CRGB::Black, CRGB::Red,   CRGB::Black,
    CRGB::Black,  CRGB::Black, CRGB::Yellow, CRGB::Black

};

const TProgmemPalette16 seer_stones_main_p PROGMEM = {
    CRGB::Indigo, CRGB::Black, CRGB::Yellow, CRGB::Black, CRGB::Red,    CRGB::Black,
    CRGB::Black,  CRGB::Black, CRGB::Indigo, CRGB::Black, CRGB::Yellow, CRGB::Black,
    CRGB::Red,    CRGB::Black, CRGB::Black,  CRGB::Black

};

// Drunk as Hell Palettes
const TProgmemPalette16 drunk_as_hell_p PROGMEM = {
    CRGB::Blue,      CRGB::Black, CRGB::DarkMagenta, CRGB::Black, CRGB::Gold,   CRGB::Black,
    CRGB::LawnGreen, CRGB::Black, CRGB::OrangeRed,   CRGB::Black, CRGB::Yellow, CRGB::Black,
    CRGB::Lime,      CRGB::Black, CRGB::LimeGreen,   CRGB::Black

};

const TProgmemPalette16 drunk_as_hell_main_p PROGMEM = {
    CRGB::Black, CRGB::Black,  CRGB::Black, CRGB::Maroon, CRGB::Black, CRGB::Black,
    CRGB::Black, CRGB::Maroon, CRGB::Black, CRGB::Black,  CRGB::Black, CRGB::Maroon,
    CRGB::Black, CRGB::Black,  CRGB::Black, CRGB::Maroon

};

// DC32 Y2K Aesthetic Theme
const TProgmemPalette16 dc32_y2k_aesthetic_p PROGMEM = {
    CRGB::DarkOrange, CRGB::Black, CRGB::Red,           CRGB::Black, CRGB::Teal,     CRGB::Black,
    CRGB::DarkOrange, CRGB::Black, CRGB::DarkSlateBlue, CRGB::Black, CRGB::DeepPink, CRGB::Black,
    CRGB::DeepPink,   CRGB::Black, CRGB::Red,           CRGB::Black};

#endif // SCENE_PALETTES_H