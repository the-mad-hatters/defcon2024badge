#include "BadgeMode.h"
#include "badge/BadgeManager.h"

#define NSFW_ENTRY_FONT        u8g2_font_ncenB08_tr
#define NSFW_ENTRY_LARGER_FONT u8g2_font_ncenB10_tr

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
    if (showingNSFWPrompt) {
        return;
    }
    showingNSFWPrompt = true;

    // Make sure we stop whatever the current mode is doing so we can show the prompt
    leave();

    bool currentNSFWMode = Badge::getInstance().getNSFWMode();

    if (nsfwPromptType == NSFWPromptType::PASSWORD) {
        // Show a text entry prompt to get the password for NSFW mode
        display->setFont(NSFW_ENTRY_FONT, NSFW_ENTRY_LARGER_FONT);
        display->showTextEntry("Enter NSFW Password", "", 'a', [this, currentNSFWMode](std::string password) {
            unsigned long loadStart = millis();

            // Check if the password matches the encrypted password
            std::transform(password.begin(), password.end(), password.begin(), ::toupper);
            display->setFont(PROMPT_FONT);
            if (password == xorCrypt(hexToBin(NSFW_PASSWORD_ENC), NSFW_PASSWORD_KEY)) {
                if (!currentNSFWMode) {
                    display->showTextCentered("NSFW loading...");
                    Badge::getInstance().setNSFWMode(true);
                } else {
                    display->showTextCentered("Already in NSFW mode");
                }
            } else {
                display->showTextCentered("Incorrect password");
                Badge::getInstance().setNSFWMode(false);
            }

            // Wait briefly so the message can be read... total of 1s including load time
            vTaskDelay(std::max(0UL, (1000 - (millis() - loadStart))) / portTICK_PERIOD_MS);

            showingNSFWPrompt = false;
            touch->clearEvents();
            enter();
        });
    } else if (nsfwPromptType == NSFWPromptType::QUESTION) {
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

            // Let the current mode go back to running again
            showingNSFWPrompt = false;
            touch->clearEvents();
            enter();
        });
    }
}
