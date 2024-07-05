#ifndef SCENE_PALETTES_H
#define SCENE_PALETTES_H

#include "led/LedBase.h"

/*Try to minimize the number of LEDs on at one time in order 
to maximize battery life. Black = Off, Color = On*/

// Mad Hatter Palette
//-------------------------------------------------------------
const TProgmemPalette16 mad_hatter_p PROGMEM = {
    CRGB::Lime, CRGB::Black, CRGB::Black, CRGB::Teal,
    CRGB::Black, CRGB::Black, CRGB::DarkOrange, CRGB::Black,
    CRGB::Black, CRGB::MediumVioletRed, CRGB::Black, CRGB::Black,
    CRGB::Aqua, CRGB::Black, CRGB::Black, CRGB::Black
    
};

const TProgmemPalette16 mad_hatter_main_p PROGMEM = {
    CRGB::Teal, CRGB::Black, CRGB::Black, CRGB::DarkOrange,
    CRGB::Black, CRGB::Black, CRGB::Teal, CRGB::Black,
    CRGB::Black, CRGB::DarkOrange, CRGB::Black, CRGB::Black,
    CRGB::Teal, CRGB::Black, CRGB::Black, CRGB::Black
    
};

// Celestial Clouds Palette
//-------------------------------------------------------------
const TProgmemPalette16 celestial_clouds_p PROGMEM = {
    CRGB::Navy, CRGB::Black, CRGB::Black, CRGB::WhiteSmoke,
    CRGB::Black, CRGB::Black, CRGB::Navy, CRGB::Black,
    CRGB::Black, CRGB::WhiteSmoke, CRGB::Black, CRGB::Black,
    CRGB::Navy, CRGB::Black, CRGB::Black, CRGB::Black
    
};

const TProgmemPalette16 celestial_clouds_main_p PROGMEM = {
    CRGB::SkyBlue, CRGB::Black, CRGB::Black, CRGB::SkyBlue,
    CRGB::Black, CRGB::Black, CRGB::SkyBlue, CRGB::Black,
    CRGB::Black, CRGB::SkyBlue, CRGB::Black, CRGB::Black,
    CRGB::SkyBlue, CRGB::Black, CRGB::Black, CRGB::Black
    
};

// Gone to Hell Palette
//-------------------------------------------------------------
const TProgmemPalette16 gone_to_hell_p PROGMEM = {
    CRGB::DarkRed, CRGB::Black, CRGB::Red, CRGB::Maroon,
    CRGB::Black, CRGB::Black, CRGB::Orange, CRGB::Black,
    CRGB::Black, CRGB::Yellow, CRGB::Black, CRGB::Black,
    CRGB::Red, CRGB::Black, CRGB::Black, CRGB::Black
    
};

const TProgmemPalette16 gone_to_hell_main_p PROGMEM = {
    CRGB::DarkRed, CRGB::Black, CRGB::Black, CRGB::Yellow,
    CRGB::Black, CRGB::Black, CRGB::DarkRed, CRGB::Black,
    CRGB::Black, CRGB::Yellow, CRGB::Black, CRGB::Black,
    CRGB::DarkRed, CRGB::Black, CRGB::Black, CRGB::Black
    
};

// Secret Combinations Palette
//-------------------------------------------------------------
const TProgmemPalette16 secret_combinations_p PROGMEM = {
    CRGB::Gold, CRGB::Black, CRGB::Black, CRGB::Gold,
    CRGB::Black, CRGB::Black, CRGB::Gold, CRGB::Black,
    CRGB::Black, CRGB::Gold, CRGB::Black, CRGB::Black,
    CRGB::Gold, CRGB::Black, CRGB::Black, CRGB::Black
    
};

const TProgmemPalette16 secret_combinations_main_p PROGMEM = {
    CRGB::Gold, CRGB::Black, CRGB::Black, CRGB::Red,
    CRGB::Black, CRGB::Black, CRGB::Gold, CRGB::Black,
    CRGB::Black, CRGB::Red, CRGB::Black, CRGB::Black,
    CRGB::Gold, CRGB::Black, CRGB::Black, CRGB::Black
    
};

// The Salamander Palette
//-------------------------------------------------------------
const TProgmemPalette16 the_salamander_p PROGMEM = {
    CRGB::Chartreuse, CRGB::Black, CRGB::Black, CRGB::DarkGreen,
    CRGB::Black, CRGB::Black, CRGB::OliveDrab, CRGB::Black,
    CRGB::Black, CRGB::ForestGreen, CRGB::Black, CRGB::Black,
    CRGB::LawnGreen, CRGB::Black, CRGB::Black, CRGB::Black
    
};

const TProgmemPalette16 the_salamander_main_p PROGMEM = {
    CRGB::DarkGreen, CRGB::Black, CRGB::Black, CRGB::OliveDrab,
    CRGB::Black, CRGB::Black, CRGB::DarkGreen, CRGB::Black,
    CRGB::Black, CRGB::OliveDrab, CRGB::Black, CRGB::Black,
    CRGB::DarkGreen, CRGB::Black, CRGB::Black, CRGB::Black
    
};

// Tapir Joyride
//-------------------------------------------------------------
const TProgmemPalette16 tapir_joyride_p PROGMEM = {
    CRGB::DarkMagenta, CRGB::Black, CRGB::Black, CRGB::DarkOrange,
    CRGB::Black, CRGB::Black, CRGB::DarkMagenta, CRGB::Black,
    CRGB::Black, CRGB::DarkOrange, CRGB::Black, CRGB::Black,
    CRGB::DarkMagenta, CRGB::Black, CRGB::Black, CRGB::Black
    
};

const TProgmemPalette16 tapir_joyride_main_p PROGMEM = {
    CRGB::DarkOliveGreen, CRGB::Black, CRGB::Black, CRGB::DarkOliveGreen,
    CRGB::Black, CRGB::Black, CRGB::DarkOliveGreen, CRGB::Black,
    CRGB::Black, CRGB::DarkOliveGreen, CRGB::Black, CRGB::Black,
    CRGB::DarkOliveGreen, CRGB::Black, CRGB::Black, CRGB::Black
    
};

// Suppressive Badge
//-------------------------------------------------------------
const TProgmemPalette16 suppressive_badge_p PROGMEM = {
    CRGB::Gold, CRGB::Black, CRGB::Black, CRGB::Cyan,
    CRGB::Black, CRGB::Black, CRGB::Gold, CRGB::Black,
    CRGB::Black, CRGB::Cyan, CRGB::Black, CRGB::Black,
    CRGB::Gold, CRGB::Black, CRGB::Black, CRGB::Black
    
};

const TProgmemPalette16 suppressive_badge_main_p PROGMEM = {
    CRGB::Indigo, CRGB::Black, CRGB::Black, CRGB::Indigo,
    CRGB::Black, CRGB::Black, CRGB::Indigo, CRGB::Black,
    CRGB::Black, CRGB::Indigo, CRGB::Black, CRGB::Black,
    CRGB::Indigo, CRGB::Black, CRGB::Black, CRGB::Black
    
};

// Seer Stones Palettes
//-------------------------------------------------------------
const TProgmemPalette16 seer_stones_p PROGMEM = {
    CRGB::Red, CRGB::Black, CRGB::Black, CRGB::Indigo,
    CRGB::Black, CRGB::Black, CRGB::Yellow, CRGB::Black,
    CRGB::Black, CRGB::Red, CRGB::Black, CRGB::Black,
    CRGB::Indigo, CRGB::Black, CRGB::Black, CRGB::Black
    
};

const TProgmemPalette16 seer_stones_main_p PROGMEM = {
    CRGB::Indigo, CRGB::Black, CRGB::Black, CRGB::Red,
    CRGB::Black, CRGB::Black, CRGB::Indigo, CRGB::Black,
    CRGB::Black, CRGB::Red, CRGB::Black, CRGB::Black,
    CRGB::Indigo, CRGB::Black, CRGB::Black, CRGB::Black
    
};

// Thetan Possession Palette
//-------------------------------------------------------------
const TProgmemPalette16 thetan_possession_p PROGMEM = {
    CRGB::Red, CRGB::Black, CRGB::Black, CRGB::Maroon,
    CRGB::Black, CRGB::Black, CRGB::Red, CRGB::Black,
    CRGB::Black, CRGB::Maroon, CRGB::Black, CRGB::Black,
    CRGB::Red, CRGB::Black, CRGB::Black, CRGB::Black
    
};

const TProgmemPalette16 thetan_possession_main_p PROGMEM = {
    CRGB::Brown, CRGB::Black, CRGB::Black, CRGB::Brown,
    CRGB::Black, CRGB::Black, CRGB::Brown, CRGB::Black,
    CRGB::Black, CRGB::Brown, CRGB::Black, CRGB::Black,
    CRGB::Brown, CRGB::Black, CRGB::Black, CRGB::Black
    
};

// Drunk as Hell Palettes
//-------------------------------------------------------------
const TProgmemPalette16 drunk_as_hell_p PROGMEM = {
    CRGB::Blue, CRGB::Black, CRGB::Black, CRGB::DarkMagenta,
    CRGB::Black, CRGB::Black, CRGB::Gold, CRGB::Black,
    CRGB::Black, CRGB::LawnGreen, CRGB::Black, CRGB::Black,
    CRGB::Lime, CRGB::Black, CRGB::Black, CRGB::Black
    
};

const TProgmemPalette16 drunk_as_hell_main_p PROGMEM = {
    CRGB::LimeGreen, CRGB::Black, CRGB::Black, CRGB::Maroon,
    CRGB::Black, CRGB::Black, CRGB::LawnGreen, CRGB::Black,
    CRGB::Black, CRGB::Blue, CRGB::Black, CRGB::Black,
    CRGB::DarkMagenta, CRGB::Black, CRGB::Black, CRGB::Black
    
};


//Gone Clear Palette
//-------------------------------------------------------------
const TProgmemPalette16 gone_clear_p PROGMEM = {
    CRGB::WhiteSmoke, CRGB::Black, CRGB::Black, CRGB::Wheat,
    CRGB::Black, CRGB::Black, CRGB::WhiteSmoke, CRGB::Black,
    CRGB::Black, CRGB::Wheat, CRGB::Black, CRGB::Black,
    CRGB::WhiteSmoke, CRGB::Black, CRGB::Black, CRGB::Black
    
};

const TProgmemPalette16 gone_clear_main_p PROGMEM = {
    CRGB::WhiteSmoke, CRGB::Black, CRGB::Black, CRGB::Turquoise,
    CRGB::Black, CRGB::Black, CRGB::WhiteSmoke, CRGB::Black,
    CRGB::Black, CRGB::Turquoise, CRGB::Black, CRGB::Black,
    CRGB::WhiteSmoke, CRGB::Black, CRGB::Black, CRGB::Black
    
};

//American Jesus
//-------------------------------------------------------------
const TProgmemPalette16 red_white_blue_p PROGMEM = {
    CRGB::Red, CRGB::Black, CRGB::Black, CRGB::White,
    CRGB::Black, CRGB::Black, CRGB::Blue, CRGB::Black,
    CRGB::Black, CRGB::Red, CRGB::Black, CRGB::Black,
    CRGB::White, CRGB::Black, CRGB::Black, CRGB::Black
};

const TProgmemPalette16 red_white_blue_main_p PROGMEM = {
    CRGB::Blue, CRGB::Black, CRGB::Black, CRGB::Red,
    CRGB::Black, CRGB::Black, CRGB::White, CRGB::Black,
    CRGB::Black, CRGB::Blue, CRGB::Black, CRGB::Black,
    CRGB::Red, CRGB::Black, CRGB::Black, CRGB::Black
};

// DC32 Y2K Aesthetic Theme
//-------------------------------------------------------------
const TProgmemPalette16 dc32_y2k_aesthetic_p PROGMEM = {
    CRGB::DarkOrange, CRGB::Black, CRGB::Black, CRGB::Red,
    CRGB::Black, CRGB::Black, CRGB::Teal, CRGB::Black,
    CRGB::Black, CRGB::DarkSlateBlue, CRGB::Black, CRGB::Black,
    CRGB::DeepPink, CRGB::Black, CRGB::Black, CRGB::Black
};

const TProgmemPalette16 dc32_y2k_aesthetic_main_p PROGMEM = {
    CRGB::Teal, CRGB::Black, CRGB::Black, CRGB::DarkOrange,
    CRGB::Black, CRGB::Black, CRGB::DarkSlateBlue, CRGB::Black,
    CRGB::Black, CRGB::Red, CRGB::Black, CRGB::Black,
    CRGB::DeepPink, CRGB::Black, CRGB::Black, CRGB::Black
};
#endif // SCENE_PALETTES_H
