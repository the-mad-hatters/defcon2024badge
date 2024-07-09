#include "BadgeMode.h"
#include "badge/BadgeManager.h"

void BadgeMode::handleTouch(TouchEvent event) {
    if (std::none_of(nonNSFWPins.begin(), nonNSFWPins.end(),
                     [this](int pin) { return inputStates[handshakePins.right(pin)] == TOUCH_DOWN; }) &&
        std::all_of(NSFWPins.begin(), NSFWPins.end(), [this](int pin) {
            int index = handshakePins.right[pin];
            return inputStates[index] == TOUCH_DOWN && latestEvents[index].duration > NSFW_ACTIVATION_TIME;
        })) {
        showNSFWPrompt();
    } else {
        receiveTouch(event);
    }
}

void BadgeMode::showNSFWPrompt() {
    // Make sure we stop whatever the current mode is doing so we can show the prompt
    leave();

    bool currentNSFWMode  = Badge::getInstance().getNSFWMode();
    const char *options[] = {"Activate", "Deactivate"};
    display->setFont(PROMPT_FONT);
    display->showPrompt("NSFW mode", options, 2, currentNSFWMode ? 1 : 0, [this, currentNSFWMode](int index) {
        // Set NSFW mode based on the selected index
        bool nsfwMode = index == 0;

        if (currentNSFWMode != nsfwMode) {
            // Update the messages and display them accordingly
            const char *message = nsfwMode ? "NSFW loading..." : "Back to safety";
            display->setFont(PROMPT_FONT);
            display->showTextCentered(message);
            unsigned long loadStart = millis();
            Badge::getInstance().setNSFWMode(nsfwMode);

            // Wait briefly so the message can be read... total of 1s including load time
            vTaskDelay(std::max(0UL, (1000 - (millis() - loadStart))) / portTICK_PERIOD_MS);
        }

        // Let the current mode do it's thing again
        enter();
    });
}
