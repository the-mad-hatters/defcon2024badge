#ifndef LED_HANDLER_H
#define LED_HANDLER_H

#include <map>
#include <mutex>
#include <vector>

#include "LedBase.h"
#include "scene/SceneManager.h"

class LedHandler {
  public:
    // Singleton instance
    static LedHandler &getInstance() {
        static LedHandler instance;
        return instance;
    };

    // Initialize the LED handler
    void init();

    /**
     * @brief Set the color of a single LED on an addressable strip
     *
     * @param strip The addressable strip to set the LED on
     * @param index The index of the LED to set
     * @param color The color to set the LED to
     */
    void setAddressable(AddressableStrip strip, int index, CRGB color);

    /**
     * @brief Lock an LED to prevent animations from overwriting it
     *
     * @param strip The addressable strip to lock an LED on
     * @param index The index of the LED to lock
     * @param color The color to set the LED to
     */
    void lockLed(AddressableStrip strip, int index, CRGB color);

    /**
     * @brief Unlock an LED to allow animations to overwrite it
     *
     * @param strip The addressable strip to unlock an LED on
     * @param index The index of the LED to unlock
     */
    void unlockLed(AddressableStrip strip, int index);

    /**
     * @brief Abstraction for FastLED.show() to handle locking
     */
    void show();

    /**
     * @brief Clear/reset all LEDs on all addressable strips as well as non-addressable LEDs
     *
     * @param unlock If true, unlock all locked LEDs as well. Default is false.
     */
    void clear(bool unlock = false);

    /**
     * @brief Set the state of a single LED on a non-addressable strip
     *
     * @param led The non-addressable LED to set
     * @param on True to turn the LED on, false to turn it off
     */
    void setNonAddressable(NonAddressableLed led, bool on);

    /**
     * @brief Lock a non-addressable LED to prevent other operations from overwriting it
     *
     * @param led The non-addressable LED to lock
     * @param on The state to set the LED to
     */
    void lockNonAddressable(NonAddressableLed led, bool on);

    /**
     * @brief Unlock a non-addressable LED to allow other operations to overwrite it
     *
     * @param led The non-addressable LED to unlock
     */
    void unlockNonAddressable(NonAddressableLed led);

    /**
     * @brief Set an animation scene to run. A scene may be set at any time and will be run
     *     until a new scene is set. Set to NONE to stop all animations.
     *
     * @param scene
     */
    void setScene(SceneType scene) {
        sceneManager.setScene(scene);
    };

  private:
    LedHandler() : sceneManager(SceneManager::getInstance()) {};
    LedHandler(const LedHandler &)            = delete;
    LedHandler &operator=(const LedHandler &) = delete;
    bool initialized                          = false;

    SceneManager &sceneManager;
    AddressableStrips addressableStrips;
    NonAddressableLeds nonAddressableLeds;
    std::mutex ledMutex;

    // Support for locking LEDs to prevent animations from overwriting them
    std::map<AddressableStrip, std::vector<bool>> lockedLeds;
    std::map<AddressableStrip, std::vector<CRGB>> lockedColors;
    std::map<AddressableStrip, std::vector<CRGB>> unlockColors; // Colors for restoring unlocked
                                                                // LEDs
    std::map<NonAddressableLed, bool> lockedNonAddressableLeds;
    std::map<NonAddressableLed, bool> lockedNonAddressableStates;
    std::map<NonAddressableLed, bool> unlockNonAddressableStates; // States for restoring unlocked
                                                                  // non-addressable LEDs
    std::mutex lockMutex;
};

#endif // LED_HANDLER_H
