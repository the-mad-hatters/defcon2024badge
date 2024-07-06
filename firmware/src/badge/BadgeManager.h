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
    void setMode(ModeType mode);
    BadgeMode *getMode();
    bool hasMode(ModeType mode);

    /**
     * @brief Flash an LED strip with a color given a count of flashes to do and a total duration
     *
     * @param strip The addressable strip to flash all LEDs on
     * @param color The color to flash the LEDs
     * @param count The number of times to flash the LEDs
     * @param duration The total duration to flash the LEDs for
     */
    void flashLedStrip(AddressableStrip strip, CRGB color, int count = 1, int duration = 900);

  private:
    Badge()
        : currentMode(nullptr)
        , display(DisplayManager::getInstance())
        , touch(TouchHandler::getInstance())
        , leds(LedHandler::getInstance()) {};
    Badge(const Badge &)            = delete;
    Badge &operator=(const Badge &) = delete;

    static void modeInputTask(void *pvParameters);
    static void modeManagerTask(void *pvParameters);

    DisplayManager &display;
    TouchHandler &touch;
    LedHandler &leds;

    BadgeMode *currentMode;
    std::mutex modeMutex;

    // Keep a map of all modes
    std::unordered_map<ModeType, std::unique_ptr<BadgeMode>> modes;
};

#endif // BADGE_MANAGER_H