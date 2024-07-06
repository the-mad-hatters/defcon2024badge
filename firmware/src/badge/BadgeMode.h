#ifndef BADGE_MODE_H
#define BADGE_MODE_H

#include <unordered_map>
#include "display/DisplayManager.h"
#include "led/LedHandler.h"
#include "touch/TouchHandler.h"

// List of modes
enum class ModeType {
    HOME,
    TRUTH,
    REVELATION,
    MAGIC_8BALL,
    MAD_HATTER_METER,
    LED_MODES,
    DISPLAY_HANDLE,
    ABOUT,
};

class BadgeMode {
  public:
    BadgeMode(ModeType type)
        : display(&DisplayManager::getInstance())
        , leds(&LedHandler::getInstance())
        , touch(&TouchHandler::getInstance())
        , type(type) {};
    BadgeMode(DisplayManager *display, LedHandler *leds, TouchHandler *touch, ModeType type)
        : display(display), leds(leds), touch(touch), type(type) {};
    virtual ~BadgeMode() {};
    ModeType getType() {
        return type;
    }
    virtual void enter()                       = 0;
    virtual void exit()                        = 0;
    virtual void handleTouch(TouchEvent event) = 0;

  protected:
    DisplayManager *display;
    LedHandler *leds;
    TouchHandler *touch;
    ModeType type;
};

// Menu items
extern const std::unordered_map<ModeType, const char *> ModeTitle;

#endif // BADGE_MODE_H