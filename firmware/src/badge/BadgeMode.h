#ifndef BADGE_MODE_H
#define BADGE_MODE_H

#include <unordered_map>
#include "display/DisplayManager.h"
#include "led/LedHandler.h"
#include "touch/TouchHandler.h"

class BadgeMode {
  public:
    virtual ~BadgeMode() {};
    virtual void enter()                       = 0;
    virtual void exit()                        = 0;
    virtual void handleTouch(TouchEvent event) = 0;

  protected:
    DisplayManager *display;
    LedHandler *leds;
    TouchHandler *touch;
};

// List of modes
enum class ModeType {
    HOME,
    TRUTH,
    REVELATION,
    MAD_HATTER_METER,
    MAD_HATTER_8BALL,
};

// Menu items
extern const std::unordered_map<ModeType, const char *> ModeTitle;

#endif // BADGE_MODE_H