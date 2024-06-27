#ifndef BADGE_MANAGER_H
#define BADGE_MANAGER_H

#include <mutex>
#include <unordered_map>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "config.h"
#include "util.h"
#include "display/DisplayManager.h"
#include "led/LedHandler.h"
#include "touch/TouchHandler.h"
#include "BadgeMode.h"

// Badge main class
class Badge {
  public:
    static Badge &getInstance() {
        static Badge instance;
        return instance;
    }
    void init();
    static void modeInputTask(void *pvParameters);
    static void modeManagerTask(void *pvParameters);
    void setMode(ModeType mode);
    BadgeMode *getMode();
    bool hasMode(ModeType mode);
    void handleTouch(int pin);

  private:
    Badge()
        : currentMode(nullptr)
        , display(DisplayManager::getInstance())
        , touch(TouchHandler::getInstance())
        , leds(LedHandler::getInstance()) {};
    Badge(const Badge &)            = delete;
    Badge &operator=(const Badge &) = delete;

    DisplayManager &display;
    TouchHandler &touch;
    LedHandler &leds;

    BadgeMode *currentMode;
    std::mutex modeMutex;

    // Keep a map of all modes
    std::unordered_map<ModeType, std::unique_ptr<BadgeMode>> modes;
};

#endif // BADGE_MANAGER_H