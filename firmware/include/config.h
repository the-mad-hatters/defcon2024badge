#ifndef CONFIG_H
#define CONFIG_H

// Display configuration
#define OLED_SDA    35
#define OLED_SCL    36
#define OLED_RST    -1
#define OLED_ADDR   0x78 // 0x3C (7-bit) or 0x78 (8-bit) for SSD1306
#define OLED_WIDTH  128
#define OLED_HEIGHT 64

/**
 * LED array configuration
 */

// Upper "Mad Hatter" addressable LED strip
#define UPPER_LED_PIN             34
#define UPPER_LED_COUNT           9
#define UPPER_LED_TYPE            WS2812
#define UPPER_LED_COLOR_ORDER     GRB
#define UPPER_LED_BASE_BRIGHTNESS 255
#define UPPER_LED_SPEED           8

// Lower "Auto Revelator" addressable LED strip
#define LOWER_LED_PIN             5
#define LOWER_LED_COUNT           13
#define LOWER_LED_TYPE            WS2812
#define LOWER_LED_COLOR_ORDER     GRB
#define LOWER_LED_BASE_BRIGHTNESS 255
#define LOWER_LED_SPEED           8

// Main board LED strip
#define MAIN_LED_PIN             38
#define MAIN_LED_COUNT           12
#define MAIN_LED_TYPE            WS2812
#define MAIN_LED_COLOR_ORDER     GRB
#define MAIN_LED_BASE_BRIGHTNESS 100
#define MAIN_LED_SPEED           8

// Touch button addressable LED strip
#define TOUCH_LED_PIN             17
#define TOUCH_LED_COUNT           4
#define TOUCH_LED_TYPE            WS2812
#define TOUCH_LED_COLOR_ORDER     GRB
#define TOUCH_LED_BASE_BRIGHTNESS 255
#define TOUCH_LED_SPEED           8

// Non-addressable LED pins
#define MAD_HATTER_HAT_LED_PIN   12
#define PAP_FIG_HAT_LED_PIN      11
#define ANGEL_OF_GOD_HAT_LED_PIN 10
#define BOOK_EYE_LED_PIN         9

/**
 * Touch input (handshakes)
 */

// Touch input pins
#define HANDSHAKE_1 1
#define HANDSHAKE_2 2
#define HANDSHAKE_3 3
#define HANDSHAKE_4 4

#define HANDSHAKE_COUNT    4
#define HANDSHAKE_DEBOUNCE 100

// Touch input thresholds
#define TOUCH_THRESHOLD 35000

#endif // CONFIG_H
