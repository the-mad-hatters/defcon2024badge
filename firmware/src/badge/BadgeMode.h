#ifndef BADGE_MODE_H
#define BADGE_MODE_H

#include <unordered_map>
#include "display/DisplayManager.h"
#include "led/LedHandler.h"
#include "touch/TouchHandler.h"

#define PROMPT_FONT          u8g2_font_ncenB08_tr
#define NSFW_ACTIVATION_TIME 1000

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
    friend class Badge;

    BadgeMode(DisplayManager *display, LedHandler *leds, TouchHandler *touch, ModeType type,
              const std::set<int> &nsfwPins = {HANDSHAKE_2, HANDSHAKE_3})
        : display(display)
        , leds(leds)
        , touch(touch)
        , type(type)
        , allPins(handshakePins.right.keys())
        , NSFWPins(nsfwPins)
        , inputStates(touch->getInputStates())
        , latestEvents(touch->getLatestEvents()) {
        // Calculate non-NSFW pins
        std::set_difference(allPins.begin(), allPins.end(), NSFWPins.begin(), NSFWPins.end(),
                            std::inserter(nonNSFWPins, nonNSFWPins.begin()));
    };
    BadgeMode(ModeType type, const std::set<int> &nsfwPins = {HANDSHAKE_2, HANDSHAKE_3})
        : BadgeMode(&DisplayManager::getInstance(), &LedHandler::getInstance(), &TouchHandler::getInstance(), type,
                    nsfwPins) {};
    virtual ~BadgeMode() {};
    ModeType getType() {
        return type;
    }
    virtual const char *getBaseType() const {
        return "BadgeMode";
    }

  protected:
    DisplayManager *display;
    LedHandler *leds;
    TouchHandler *touch;
    ModeType type;

    // Touch event handling
    TouchEvent (&latestEvents)[HANDSHAKE_COUNT];
    TouchEventType (&inputStates)[HANDSHAKE_COUNT];
    void handleTouch(TouchEvent event);

    virtual void enter() = 0;
    virtual void leave() = 0;

    // Touch event pass-through when NSFW mode is not activated
    virtual void receiveTouch(TouchEvent event) {};

  private:
    // NSFW mode activation
    std::set<int> allPins;
    std::set<int> NSFWPins;
    std::set<int> nonNSFWPins;

    void showNSFWPrompt();
};

// Menu items
extern const std::unordered_map<ModeType, const char *> ModeTitle;

#endif // BADGE_MODE_H