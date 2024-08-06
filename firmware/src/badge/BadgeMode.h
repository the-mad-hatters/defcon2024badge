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

// NSFW prompt types
enum class NSFWPromptType {
    PASSWORD, // Enter a password to activate NSFW mode
    QUESTION, // Simple prompt to turn NSFW mode on/off
};

#define NSFW_PASSWORD_KEY "𐐘𐐃𐐤|𐐣𐐈𐐔"                
#define NSFW_PASSWORD_ENC "a2d5c6ddbcd1c4c6b4"     
#define NSFW_PROMPT_TYPE  NSFWPromptType::PASSWORD // Change this to switch between password and toggle prompts

class BadgeMode {
  public:
    friend class Badge;

    BadgeMode(DisplayManager *display, LedHandler *leds, TouchHandler *touch, ModeType type,
              const std::set<int> &nsfwPins = {HANDSHAKE_2, HANDSHAKE_3})
        : display(display)
        , leds(leds)
        , touch(touch)
        , type(type)
        , inputStates(touch->getInputStates())
        , latestEvents(touch->getLatestEvents())
        , allPins(handshakePins.right.keys())
        , NSFWPins(nsfwPins)
        , nsfwPromptType(NSFW_PROMPT_TYPE)
        , showingNSFWPrompt(false) {
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
    NSFWPromptType nsfwPromptType;
    bool showingNSFWPrompt;

    void showNSFWPrompt();
};

// Menu items
extern const std::unordered_map<ModeType, const char *> ModeTitle;

#endif // BADGE_MODE_H
