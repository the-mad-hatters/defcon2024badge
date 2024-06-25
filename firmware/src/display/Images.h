#ifndef IMAGES_H
#define IMAGES_H

#include <stdint.h>
#include <U8g2lib.h>

#define EYE_WIDTH  85
#define EYE_HEIGHT 64
const uint8_t eye[] PROGMEM = {
    0x00, 0x00, 0x00, 0xe0, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
    0xff, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xfc, 0xff, 0xff, 0xff, 0xff, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
    0xff, 0xff, 0xff, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x80, 0xff, 0xff, 0xe0, 0xe7, 0xff, 0xff, 0x01,
    0x00, 0x00, 0x00, 0xe0, 0xff, 0x0f, 0xff, 0xff, 0xfe, 0xff, 0x07, 0x00, 0x00, 0x00, 0xf0, 0xff,
    0xe1, 0xff, 0xff, 0xf7, 0xff, 0x0f, 0x00, 0x00, 0x00, 0xf8, 0x7f, 0xf8, 0x8f, 0xe1, 0x9f, 0xff,
    0x3f, 0x00, 0x00, 0x00, 0xfc, 0x1f, 0xfe, 0xfe, 0x7f, 0x7f, 0xfe, 0x7f, 0x00, 0x00, 0x00, 0xff,
    0x07, 0xbf, 0xff, 0xff, 0xff, 0xf9, 0xff, 0x00, 0x00, 0x80, 0xff, 0xc3, 0xef, 0xff, 0xff, 0xff,
    0xe3, 0xff, 0x01, 0x00, 0x80, 0xff, 0xe0, 0xfb, 0x07, 0xc0, 0xff, 0x8f, 0xff, 0x03, 0x00, 0xc0,
    0x7f, 0xf0, 0xff, 0x01, 0x00, 0xff, 0x1f, 0xff, 0x0f, 0x00, 0xe0, 0x3f, 0xf8, 0xff, 0x00, 0x00,
    0xfc, 0x3f, 0xfc, 0x1f, 0x00, 0xf0, 0x1f, 0xbc, 0x7f, 0x00, 0x00, 0xf8, 0xff, 0xf8, 0x3f, 0x00,
    0xf0, 0x0f, 0xfe, 0x3f, 0x00, 0x00, 0xf0, 0xff, 0xe1, 0x3f, 0x00, 0xf8, 0x07, 0xff, 0x1f, 0x00,
    0x00, 0xe0, 0xff, 0xc3, 0x7f, 0x00, 0xfc, 0x83, 0xff, 0x0f, 0x00, 0x00, 0xc0, 0xff, 0x87, 0xff,
    0x00, 0xfc, 0xc1, 0xff, 0x0f, 0x00, 0x00, 0x80, 0xff, 0x0f, 0xff, 0x01, 0xfc, 0xe1, 0xff, 0x07,
    0x00, 0x00, 0x00, 0xff, 0x1f, 0xfe, 0x01, 0xfe, 0xf0, 0xff, 0x07, 0x00, 0x00, 0x00, 0xff, 0x3f,
    0xfc, 0x03, 0x7e, 0xf8, 0xff, 0x03, 0x80, 0x0f, 0x00, 0xfe, 0x7f, 0xf8, 0x07, 0x7e, 0xf8, 0xff,
    0x03, 0xe0, 0x3f, 0x00, 0xfe, 0xff, 0xf0, 0x07, 0x7f, 0xfc, 0xff, 0x03, 0xe0, 0x7f, 0x00, 0xfc,
    0xff, 0xf1, 0x07, 0x3f, 0xfe, 0xff, 0x03, 0xf0, 0x7f, 0x00, 0xfc, 0xff, 0xe3, 0x0f, 0x3f, 0xff,
    0xff, 0x01, 0xf0, 0xff, 0x00, 0xf8, 0xff, 0xc7, 0x0f, 0x3f, 0xff, 0xff, 0x01, 0xf8, 0xff, 0x00,
    0xf8, 0xff, 0xc7, 0x0f, 0xbf, 0xff, 0xff, 0x01, 0xf8, 0xff, 0x01, 0xf8, 0xff, 0x8f, 0x1f, 0xdf,
    0xff, 0xff, 0x01, 0xf8, 0xff, 0x01, 0xf8, 0xff, 0x9f, 0x1f, 0xdf, 0xff, 0xff, 0x01, 0xf8, 0xff,
    0x01, 0xf8, 0xff, 0xbf, 0x1f, 0xff, 0xff, 0xff, 0x01, 0xf0, 0xff, 0x01, 0xf8, 0xff, 0x7f, 0x1f,
    0xbf, 0xff, 0xff, 0x03, 0xf0, 0xff, 0x01, 0xf8, 0xff, 0x7f, 0x1f, 0xbf, 0xff, 0xff, 0x03, 0xf0,
    0xff, 0x00, 0xf8, 0xff, 0x7f, 0x1f, 0x3e, 0xff, 0xff, 0x03, 0xe0, 0xff, 0x00, 0xf8, 0xff, 0x3f,
    0x1f, 0x7e, 0xff, 0xff, 0x03, 0xc0, 0x7f, 0x00, 0xf8, 0xff, 0x3f, 0x1f, 0x7e, 0xfe, 0xff, 0x07,
    0x80, 0x3f, 0x00, 0xf8, 0xff, 0x1f, 0x1f, 0xfc, 0xfc, 0xff, 0x07, 0x00, 0x06, 0x00, 0xf8, 0xff,
    0x8f, 0x1f, 0xfc, 0xfc, 0xff, 0x0f, 0x00, 0x00, 0x00, 0xfc, 0xff, 0x8f, 0x1f, 0xf8, 0xf9, 0xff,
    0x0f, 0x00, 0x00, 0x00, 0xfc, 0xff, 0x87, 0x0f, 0xf0, 0xf3, 0xff, 0x1f, 0x00, 0x00, 0x00, 0xfe,
    0xff, 0xc3, 0x0f, 0xf0, 0xe3, 0xff, 0x3f, 0x00, 0x00, 0x00, 0xfe, 0xff, 0xc1, 0x07, 0xe0, 0xc7,
    0xff, 0x3f, 0x00, 0x00, 0x00, 0xff, 0xff, 0xe1, 0x07, 0xc0, 0x8f, 0xff, 0x7f, 0x00, 0x00, 0x00,
    0xff, 0xff, 0xf0, 0x03, 0x80, 0x1f, 0xff, 0xff, 0x00, 0x00, 0x80, 0xff, 0x7f, 0xf0, 0x03, 0x80,
    0x3f, 0xfe, 0xff, 0x01, 0x00, 0xc0, 0xff, 0x3f, 0xf8, 0x01, 0x00, 0x7f, 0xfc, 0xff, 0x07, 0x00,
    0xf0, 0xff, 0x1f, 0xfc, 0x00, 0x00, 0xfe, 0xf9, 0xff, 0x0f, 0x00, 0xf8, 0xff, 0x0f, 0x7e, 0x00,
    0x00, 0xfc, 0xf3, 0xff, 0x3f, 0x00, 0xfe, 0xdf, 0x07, 0x7f, 0x00, 0x00, 0xf8, 0xef, 0xff, 0xff,
    0xe3, 0xff, 0xff, 0xc3, 0x3f, 0x00, 0x00, 0xf0, 0x9f, 0xff, 0xff, 0xff, 0xff, 0xfb, 0xe0, 0x1f,
    0x00, 0x00, 0xe0, 0x7f, 0x7f, 0xff, 0xff, 0xff, 0x7e, 0xf8, 0x0f, 0x00, 0x00, 0x80, 0xff, 0xfd,
    0xfb, 0xff, 0xbf, 0x3f, 0xfe, 0x07, 0x00, 0x00, 0x00, 0xff, 0xf7, 0x9f, 0xff, 0xf3, 0x8f, 0xff,
    0x03, 0x00, 0x00, 0x00, 0xfe, 0xdf, 0xff, 0x81, 0xff, 0xe3, 0xff, 0x00, 0x00, 0x00, 0x00, 0xf8,
    0xff, 0xfe, 0xff, 0xff, 0xf8, 0x7f, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xff, 0xf7, 0xff, 0x9f, 0xff,
    0x1f, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xff, 0xff, 0x38, 0xf8, 0xff, 0x07, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0xff, 0xff, 0xff,
    0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0xff, 0xff, 0x0f, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x80, 0xff, 0xff, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0xff,
    0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00};

#define EYE_V2_WIDTH  127
#define EYE_V2_HEIGHT 64
const uint8_t eye_v2[] PROGMEM = {
    0xf3, 0x03, 0xfe, 0x01, 0x81, 0x7f, 0x42, 0x6f, 0x7b, 0x22, 0xfe, 0x41, 0xc0, 0x3f, 0xc0, 0x67,
    0xf1, 0x07, 0xfe, 0x00, 0x02, 0x7f, 0x04, 0x67, 0xf2, 0x22, 0x7f, 0x20, 0xc0, 0x3f, 0xe0, 0x67,
    0xf9, 0x0f, 0xfe, 0x00, 0x06, 0xfc, 0x04, 0x67, 0xf2, 0x10, 0x1f, 0x20, 0x80, 0x1f, 0xf0, 0x47,
    0xf8, 0x1f, 0x7e, 0xc0, 0x44, 0xe0, 0x88, 0x67, 0xf6, 0x10, 0x07, 0x32, 0x01, 0x1f, 0xf8, 0x4f,
    0xfc, 0x3f, 0x3c, 0xe0, 0x8c, 0x83, 0x98, 0x33, 0xe6, 0x89, 0xc0, 0x98, 0x03, 0x1f, 0xfc, 0x0f,
    0xfc, 0x01, 0x3c, 0xf0, 0x18, 0x0f, 0xc0, 0x33, 0xe6, 0x01, 0x70, 0x98, 0x03, 0x1e, 0xfe, 0x1f,
    0x3e, 0x00, 0x1e, 0xf0, 0x19, 0x7e, 0xc0, 0x33, 0xee, 0x03, 0x3e, 0x8c, 0x07, 0x3c, 0x00, 0x1c,
    0x3e, 0xe0, 0x0f, 0xf8, 0x39, 0xf8, 0xe3, 0x39, 0xce, 0xe3, 0x1f, 0xcc, 0x07, 0x7c, 0x00, 0x3e,
    0x7f, 0xe0, 0x0f, 0xf8, 0x33, 0xf0, 0xff, 0x39, 0xce, 0xff, 0x07, 0xce, 0x0f, 0xf8, 0x01, 0x3f,
    0xff, 0xe0, 0x07, 0xf8, 0x73, 0xc4, 0xff, 0x39, 0xce, 0xff, 0x23, 0xe7, 0x0f, 0xf8, 0x81, 0x3f,
    0xff, 0xe0, 0x07, 0xfc, 0xf3, 0x8c, 0xff, 0x3c, 0x9e, 0xff, 0x10, 0xe7, 0x0f, 0xf0, 0xc1, 0x7f,
    0xff, 0xe1, 0x03, 0xfc, 0xe3, 0x18, 0xfe, 0x3c, 0x9e, 0x7f, 0x9c, 0xe7, 0x1f, 0xf0, 0xc1, 0x7f,
    0xff, 0xc3, 0x0b, 0xf8, 0xe0, 0x79, 0xfc, 0x3c, 0x9e, 0x1f, 0x8f, 0xc3, 0x1f, 0xe8, 0xe1, 0x7f,
    0x07, 0xc0, 0x33, 0x10, 0xe0, 0xf1, 0x71, 0x3e, 0x3e, 0x87, 0xc7, 0x03, 0x0e, 0xe6, 0xe1, 0x7f,
    0x07, 0xc0, 0xc9, 0x00, 0xcc, 0x03, 0x47, 0x3e, 0x3e, 0x61, 0xc0, 0x03, 0x00, 0xd1, 0x01, 0x70,
    0x0f, 0xfe, 0x1d, 0x00, 0xcf, 0x03, 0x18, 0x3e, 0x3e, 0x08, 0xe0, 0x39, 0x80, 0xdc, 0x07, 0x78,
    0x1f, 0xfe, 0x7d, 0x00, 0x9f, 0x07, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xf9, 0x00, 0xde, 0x1f, 0x7c,
    0x1f, 0xfe, 0xf0, 0x00, 0x9e, 0x07, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x7c, 0x80, 0xbf, 0x1f, 0x7e,
    0x3f, 0xfc, 0xe0, 0x63, 0x98, 0x0f, 0x00, 0xf0, 0x07, 0x00, 0xf0, 0x1c, 0xc2, 0x87, 0x0f, 0x7f,
    0x7f, 0xfc, 0xc0, 0x87, 0x20, 0x0f, 0x00, 0x06, 0x30, 0x00, 0xf8, 0x84, 0xe1, 0x83, 0x8f, 0x7f,
    0xff, 0x7c, 0xc0, 0x1f, 0x03, 0x1f, 0x80, 0xf1, 0xc7, 0x00, 0x78, 0x60, 0xf8, 0x03, 0x8f, 0x7f,
    0x01, 0x7c, 0xe0, 0x3f, 0x0c, 0x1e, 0x40, 0xfc, 0x1f, 0x01, 0x7c, 0x30, 0xfc, 0x03, 0x07, 0x78,
    0x03, 0x78, 0xff, 0x3f, 0x3c, 0x3e, 0x30, 0xfe, 0x3f, 0x06, 0x3c, 0x0c, 0xfe, 0x47, 0x07, 0x40,
    0x07, 0x78, 0xff, 0x1f, 0x0e, 0x3e, 0x08, 0xff, 0x7f, 0x08, 0x3e, 0x38, 0xfc, 0x7f, 0x07, 0x78,
    0x0f, 0x3c, 0xff, 0x07, 0x00, 0x7c, 0x84, 0xff, 0xff, 0x10, 0x1e, 0x40, 0xf8, 0x7f, 0x0f, 0x7e,
    0x07, 0xbe, 0xff, 0x01, 0xf0, 0x7d, 0x82, 0xff, 0xff, 0x20, 0x9f, 0x07, 0xc0, 0xff, 0x1e, 0x7c,
    0x41, 0xbe, 0x7f, 0x00, 0x7f, 0x38, 0xc1, 0x3f, 0xfe, 0x41, 0x1e, 0x7f, 0x00, 0xff, 0x1e, 0x70,
    0x38, 0xbe, 0x0f, 0xf0, 0x07, 0x98, 0xc0, 0x1f, 0xfc, 0x81, 0x0c, 0xf8, 0x03, 0xfc, 0x1e, 0x42,
    0x1e, 0x9e, 0x03, 0xfe, 0x80, 0x43, 0xc0, 0x0f, 0xf8, 0x01, 0xe1, 0x80, 0x3f, 0xe0, 0x3e, 0x0e,
    0x01, 0x1f, 0xe0, 0x0f, 0xf8, 0x27, 0xc0, 0x0f, 0xf8, 0x01, 0xe0, 0x0f, 0xf8, 0x81, 0x3e, 0x38,
    0x00, 0x0f, 0xfc, 0x80, 0xff, 0x07, 0xc0, 0x0f, 0xf8, 0x01, 0xf0, 0xff, 0x80, 0x0f, 0x3c, 0x40,
    0x07, 0xc0, 0x7f, 0xf0, 0xff, 0x0f, 0xc0, 0x0f, 0xf8, 0x01, 0xf0, 0xff, 0x0f, 0xfe, 0x20, 0x00,
    0x7f, 0xf0, 0x3f, 0xff, 0xff, 0x0f, 0xc0, 0x1f, 0xfc, 0x01, 0xf8, 0xff, 0x7f, 0xfc, 0x07, 0x7f,
    0x7f, 0xc3, 0x3f, 0xff, 0xff, 0x07, 0xc0, 0x7f, 0xff, 0x01, 0xf0, 0xff, 0x7f, 0xfc, 0x03, 0x7f,
    0x3f, 0x0e, 0x3f, 0xf0, 0xff, 0xa7, 0x80, 0xff, 0xff, 0x81, 0xf0, 0xff, 0x0f, 0x7e, 0x38, 0x7f,
    0x1e, 0x1e, 0xf8, 0x00, 0xfe, 0x33, 0x81, 0xff, 0xff, 0x40, 0xe6, 0x7f, 0x80, 0x1f, 0x3e, 0x3e,
    0x18, 0x9e, 0xe0, 0x0f, 0xc0, 0x73, 0x02, 0xff, 0x7f, 0x20, 0xe7, 0x01, 0xf0, 0x03, 0x1e, 0x0c,
    0x01, 0xbe, 0x03, 0xff, 0x03, 0xf0, 0x04, 0xfe, 0x7f, 0x90, 0x0f, 0xc0, 0xff, 0xe0, 0x1e, 0x46,
    0x63, 0xbe, 0x0f, 0xfc, 0x7f, 0xc0, 0x19, 0xfc, 0x1f, 0xcc, 0x01, 0xfe, 0x1f, 0x78, 0x9f, 0x71,
    0x87, 0xbe, 0x7f, 0xf0, 0xff, 0x01, 0x20, 0xf8, 0x0f, 0x02, 0x80, 0xff, 0x03, 0x7e, 0x5f, 0x7c,
    0x03, 0xbc, 0xff, 0x81, 0xff, 0x7c, 0xc0, 0x80, 0x81, 0x01, 0x9f, 0xff, 0xc0, 0x7f, 0x0f, 0x78,
    0x03, 0xb8, 0xff, 0x07, 0xfc, 0x7c, 0x00, 0x03, 0x60, 0x00, 0x9f, 0x1f, 0xf0, 0x77, 0x07, 0x78,
    0x01, 0x38, 0xff, 0x1f, 0x70, 0x3e, 0x00, 0xfc, 0x0f, 0x00, 0x3f, 0x03, 0xfc, 0x03, 0x07, 0x70,
    0x01, 0x7c, 0xfb, 0x3f, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x01, 0x3e, 0x00, 0xff, 0x01, 0x07, 0x70,
    0x79, 0x7c, 0xf0, 0x3f, 0x1c, 0x1f, 0x80, 0x01, 0xe0, 0x00, 0x7e, 0x0c, 0xff, 0x81, 0xcf, 0x61,
    0x7f, 0x7c, 0xe0, 0x3f, 0x1e, 0x1f, 0x00, 0xfe, 0x1f, 0x00, 0x7c, 0x18, 0xfe, 0x81, 0x8f, 0x7f,
    0x3f, 0x7e, 0xe0, 0x0f, 0x07, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x78, 0x60, 0xfc, 0x83, 0x9f, 0x7f,
    0x1f, 0xfe, 0xf0, 0x87, 0x81, 0x0f, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x84, 0xf8, 0xdf, 0x1f, 0x7f,
    0x1f, 0xff, 0xf0, 0x03, 0x98, 0x07, 0x80, 0x3d, 0xde, 0x00, 0xf0, 0x0c, 0xf0, 0xdf, 0x3f, 0x7e,
    0x0f, 0xe0, 0xfc, 0x00, 0x9e, 0x07, 0x70, 0x3c, 0x1e, 0x0e, 0xf0, 0x3d, 0xc0, 0xdf, 0x01, 0x7e,
    0x0f, 0xc0, 0x7d, 0x00, 0xcf, 0xc3, 0x0f, 0x39, 0xce, 0xf8, 0xe1, 0xf9, 0x80, 0xef, 0x00, 0x7c,
    0x07, 0xe0, 0x19, 0x80, 0xcf, 0xf3, 0xc3, 0x39, 0xce, 0xe3, 0xe7, 0x79, 0x00, 0xee, 0x01, 0x78,
    0x07, 0xe0, 0x89, 0x00, 0xe0, 0xf1, 0xf1, 0x39, 0xce, 0x8f, 0xcf, 0x03, 0x80, 0xe4, 0x01, 0x78,
    0xff, 0xe3, 0x23, 0x00, 0xe0, 0x79, 0xf8, 0x33, 0xee, 0x1f, 0xcf, 0x03, 0x00, 0xf2, 0xf1, 0x7f,
    0xff, 0xf3, 0x13, 0xf8, 0xe3, 0x3c, 0xfe, 0x33, 0xe6, 0x7f, 0x9c, 0xc7, 0x0f, 0xf0, 0xe3, 0x7f,
    0xff, 0xf1, 0x07, 0xf8, 0x73, 0x0c, 0xff, 0x33, 0xe6, 0xff, 0x38, 0xe7, 0x0f, 0xf8, 0xe3, 0x7f,
    0xff, 0xf1, 0x07, 0xf8, 0x73, 0xc6, 0xff, 0x37, 0xe6, 0xff, 0x31, 0xe7, 0x0f, 0xf8, 0xc7, 0x3f,
    0xfe, 0x80, 0x0f, 0xf8, 0x3b, 0xe3, 0xff, 0x67, 0xf2, 0xff, 0x67, 0xce, 0x0f, 0xfc, 0xc1, 0x3f,
    0xfe, 0x00, 0x0f, 0xf8, 0x39, 0xf9, 0xff, 0x67, 0xf2, 0xff, 0x0f, 0xcc, 0x0f, 0x3c, 0x80, 0x3f,
    0x7e, 0x00, 0x1e, 0xf8, 0x19, 0xfc, 0xc3, 0x67, 0xf2, 0xe3, 0x1f, 0xdc, 0x07, 0x3e, 0x80, 0x1f,
    0x7c, 0x3c, 0x3e, 0xf0, 0x0c, 0x3e, 0xc0, 0x6f, 0xf2, 0x01, 0x3c, 0x98, 0x07, 0x3f, 0x06, 0x1f,
    0xfc, 0x3f, 0x3e, 0xf0, 0x8c, 0x07, 0x90, 0x6f, 0xfb, 0x01, 0xe0, 0x90, 0x03, 0x3f, 0xfe, 0x0f,
    0xf8, 0x1f, 0x7f, 0x60, 0xc6, 0x40, 0x88, 0x4f, 0xf9, 0x0c, 0x80, 0x31, 0x83, 0x3f, 0xfc, 0x4f,
    0xf9, 0x0f, 0xff, 0x40, 0x26, 0x70, 0x8c, 0x4f, 0xf9, 0x18, 0x0e, 0x20, 0x80, 0x3f, 0xfc, 0x47};

#define ROCK_WIDTH  134
#define ROCK_HEIGHT 64
const uint8_t rock[] PROGMEM = {
    0x00, 0xf0, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xfe, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xff, 0xff, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc,
    0x3f, 0x00, 0x00, 0xc0, 0xff, 0x87, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
    0xff, 0xff, 0x01, 0x00, 0xe0, 0x7f, 0xc0, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xe0, 0xff, 0xff, 0x03, 0x00, 0xf0, 0x01, 0xc0, 0xe1, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xf8, 0xff, 0xfc, 0x07, 0x00, 0xf0, 0x01, 0x00, 0xf0, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xfe, 0xc7, 0xf0, 0x0f, 0x00, 0xf8, 0x0f, 0x00, 0xf8, 0x0f, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xff, 0x87, 0xc1, 0x1f, 0x00, 0x7c, 0xf0, 0x00, 0xe7, 0x1f, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x7f, 0x0c, 0x83, 0x3f, 0x00, 0x3c, 0x80, 0xff, 0xe1, 0x7b, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x80, 0x7f, 0x18, 0x02, 0x7e, 0x00, 0xfe, 0x01, 0x00, 0xf0, 0xf0, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xff, 0x30, 0x04, 0xf0, 0x00, 0xfe, 0x3f, 0x00, 0x7c, 0xf8,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0x61, 0x0c, 0xe0, 0x01, 0x1e, 0xe0, 0x8f, 0x3f,
    0xcc, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xfd, 0x43, 0x08, 0xc0, 0x03, 0x1f, 0x00, 0x3c,
    0x18, 0xe6, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xe0, 0xc7, 0x10, 0xf0, 0x03, 0xff, 0x3f,
    0x00, 0x0c, 0xe3, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0xc0, 0x8f, 0x31, 0xf8, 0x07, 0xff,
    0x7f, 0x00, 0x86, 0x31, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x0f, 0x1f, 0x63, 0xe0, 0x07,
    0xff, 0x3f, 0x80, 0xc3, 0x18, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0x7f, 0x3c, 0x86, 0x01,
    0x0f, 0xff, 0x03, 0xc0, 0x61, 0x1c, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x80, 0x01, 0x04,
    0x03, 0x0e, 0x1f, 0x70, 0xf8, 0x30, 0x0c, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x03,
    0x18, 0x1c, 0x0e, 0x0f, 0xc7, 0x3f, 0x18, 0x0e, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x00,
    0x06, 0x30, 0xf0, 0x1f, 0x8e, 0x01, 0x00, 0x0c, 0x07, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe,
    0x03, 0x0c, 0x60, 0x80, 0x1f, 0xce, 0x00, 0x00, 0x07, 0x87, 0x1f, 0x00, 0xc0, 0x1f, 0x00, 0x00,
    0x0f, 0x0e, 0x18, 0x80, 0xc3, 0x1f, 0x3e, 0x00, 0xf8, 0x80, 0xc3, 0x1f, 0x00, 0xfc, 0xff, 0x00,
    0x00, 0x0f, 0x18, 0x70, 0x00, 0xfe, 0x1f, 0x3e, 0xc0, 0x0f, 0xc0, 0xc1, 0x1f, 0x00, 0xff, 0xff,
    0x01, 0x00, 0x1f, 0x60, 0xc0, 0x07, 0xfc, 0x1f, 0x1c, 0x7c, 0x00, 0xe0, 0xe1, 0x19, 0x80, 0xff,
    0xf1, 0x03, 0x00, 0x7f, 0x80, 0x81, 0xff, 0xff, 0x3f, 0x9c, 0x03, 0x00, 0xf0, 0xe0, 0x19, 0xc0,
    0x3f, 0xe3, 0x07, 0x00, 0xff, 0x00, 0x06, 0x3e, 0x00, 0x3c, 0xfc, 0x00, 0x00, 0x78, 0xf0, 0x18,
    0xe0, 0x63, 0x86, 0x0f, 0x80, 0xff, 0x03, 0x1c, 0x00, 0x00, 0x38, 0x78, 0x00, 0x00, 0x3e, 0x78,
    0x18, 0xf0, 0x43, 0x04, 0x1e, 0x80, 0xf3, 0x0f, 0x70, 0x00, 0x00, 0x3e, 0x78, 0xe0, 0x81, 0x1f,
    0x78, 0x18, 0xf8, 0x87, 0x08, 0x38, 0x80, 0x03, 0x0e, 0x80, 0xe3, 0xff, 0x3f, 0xf0, 0xff, 0xe0,
    0x0f, 0x3c, 0x18, 0xf8, 0x1f, 0x19, 0x70, 0x80, 0x03, 0x00, 0x00, 0xfe, 0x01, 0x38, 0xe0, 0x3f,
    0xf0, 0x03, 0x1e, 0x1c, 0x1c, 0x3e, 0x13, 0x78, 0x80, 0x03, 0x00, 0x00, 0x00, 0x00, 0x3c, 0xe0,
    0x0f, 0xfc, 0x01, 0x1e, 0x0c, 0x1e, 0x78, 0x22, 0xfc, 0x80, 0x7f, 0x00, 0x00, 0x00, 0xc0, 0x3f,
    0xc0, 0x07, 0x7e, 0x00, 0x0f, 0x0c, 0xfe, 0xf3, 0x44, 0xf0, 0x80, 0xff, 0x7f, 0x00, 0x00, 0xfe,
    0x3d, 0x80, 0x83, 0x1f, 0x80, 0x07, 0x0c, 0x0e, 0x0c, 0x8c, 0xc1, 0x81, 0xff, 0xff, 0x1f, 0xf0,
    0x3f, 0x1c, 0x00, 0xc3, 0x0f, 0x80, 0x03, 0x0e, 0x07, 0x18, 0x18, 0x86, 0x81, 0xff, 0xff, 0xff,
    0xff, 0x07, 0x1c, 0x00, 0xff, 0x03, 0xc0, 0x83, 0x07, 0x1f, 0x30, 0x20, 0xf8, 0x81, 0x07, 0xf8,
    0xff, 0xff, 0x00, 0x1c, 0x00, 0xfe, 0x01, 0xe0, 0xe1, 0x07, 0x77, 0x60, 0xc0, 0xe0, 0x81, 0x03,
    0x00, 0xfe, 0x0f, 0x00, 0x0e, 0x00, 0xfc, 0x00, 0xf8, 0xf0, 0x83, 0x83, 0xc1, 0x00, 0xff, 0x01,
    0x03, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0xf8, 0x00, 0x3c, 0xf8, 0x83, 0x07, 0x86, 0x07, 0xfe,
    0x03, 0x07, 0x00, 0x00, 0x00, 0xf0, 0x07, 0x00, 0xe0, 0x00, 0x1f, 0xfc, 0x81, 0x0f, 0x08, 0xfe,
    0xc3, 0x03, 0x07, 0x00, 0x00, 0x00, 0xff, 0x07, 0x00, 0xc0, 0xc3, 0x07, 0xfe, 0x81, 0x3f, 0x30,
    0x00, 0x00, 0x03, 0x0f, 0x00, 0x00, 0xf0, 0xff, 0x03, 0x00, 0x80, 0xff, 0x81, 0xff, 0x80, 0xff,
    0xc0, 0x00, 0x00, 0x03, 0x7e, 0x00, 0x00, 0xff, 0xff, 0x03, 0x00, 0x00, 0xfe, 0xe0, 0x7f, 0x80,
    0xc0, 0x00, 0xc3, 0xff, 0x03, 0xfe, 0x07, 0xf0, 0xff, 0xe3, 0x01, 0x00, 0x00, 0xfc, 0xf8, 0x3f,
    0x80, 0x00, 0x00, 0x38, 0x80, 0x03, 0xfe, 0xff, 0xff, 0x3f, 0xe0, 0x00, 0x00, 0x00, 0xf0, 0xff,
    0x1f, 0x80, 0x01, 0x00, 0x00, 0xc0, 0x03, 0x3c, 0xff, 0xff, 0x01, 0x78, 0x00, 0x00, 0x00, 0x80,
    0xff, 0x0f, 0xc0, 0xff, 0x01, 0x00, 0xfc, 0x03, 0x1c, 0xf0, 0x1f, 0x00, 0x3c, 0x00, 0x00, 0x00,
    0x00, 0xfc, 0x03, 0x80, 0xff, 0x3f, 0xe0, 0x8f, 0x03, 0x38, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x80, 0xff, 0xff, 0xff, 0x81, 0x01, 0xf8, 0x00, 0x00, 0x00, 0x0f, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x01, 0xfe, 0x3f, 0x80, 0x01, 0xf0, 0x0f, 0x00, 0xc0, 0x03,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x01, 0x00, 0x00, 0xc0, 0x01, 0xe0, 0xff, 0xff, 0xf1,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x01, 0x00, 0x00, 0xf8, 0x00, 0xc0, 0xff, 0xff,
    0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x03, 0x00, 0x80, 0xff, 0x00, 0x80, 0xff,
    0xff, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0xf8, 0x7f, 0x00, 0x00,
    0xff, 0xff, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x80, 0xff, 0x3f, 0x00,
    0x00, 0xfe, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x3f, 0x38,
    0x00, 0x00, 0xf8, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe7, 0xff, 0x03,
    0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00,
    0x00, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e,
    0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xfc, 0x00, 0xc0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xf8, 0xff, 0xf3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xf0, 0xff, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xe0, 0xff, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xc0, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// Image ID enum
enum class ImageID {
    EYE,
    EYE_V2,
    ROCK,
};

// Image data struct to associate image ID with image data, width, and height
struct ImageData {
    ImageID id;
    const uint8_t *data;
    uint8_t width;
    uint8_t height;
};

// Array of images
const ImageData IMAGES[] = {
    {ImageID::EYE, eye, EYE_WIDTH, EYE_HEIGHT},
    {ImageID::EYE_V2, eye_v2, EYE_V2_WIDTH, EYE_V2_HEIGHT},
    {ImageID::ROCK, rock, ROCK_WIDTH, ROCK_HEIGHT},
};

#endif // IMAGES_H