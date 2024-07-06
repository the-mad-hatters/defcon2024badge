#ifndef MODE_MAGIC8BALL_HPP
#define MODE_MAGIC8BALL_HPP

#include "MessageMode.hpp"

static const char *TAG_MAGIC8BALLMODE = "Magic8BallMode";

class Magic8BallMode : public MessageMode {
  public:
    Magic8BallMode() : MessageMode(ModeType::MAGIC_8BALL, "/8ball.txt", "/8ball_nsfw.txt") {
    }

    void handleTouch(TouchEvent event) override {
        MessageMode::handleTouch(event);
    }

  protected:
    void start() override {
        showPrompt();
    }

    void stop() override {
    }

    void showPrompt() {
        display->setFont(PROMPT_FONT);
        const char *options[] = {"Get Answer"};
        display->showPrompt("Magic 8 Ball", options, 1, 0, [this](int index) {
            if (index == 0) {
                std::string answer                = getRandomMessage();
                DisplayManager::TextBounds bounds = display->getTextBounds(MESSAGE_FONT, answer.c_str());

                if (bounds.width > display->getDisplayWidth()) {
                    ScrollEvent event;
                    display->setFont(MESSAGE_FONT);
                    display->setScrollSpeed(MESSAGE_SPEED);
                    display->setScrollIterations(1);
                    display->scrollText(answer.c_str());

                    // Wait for scroll to complete before showing prompt again
                    while (true) {
                        if (xQueueReceive(textScrollEvents, &event, portMAX_DELAY) == pdTRUE) {
                            if (event.type == ScrollEventType::SCROLL_END) {
                                touch->clearEvents();
                                showPrompt();
                                break;
                            }
                        }
                    }
                } else {
                    display->setFont(MESSAGE_FONT);
                    display->showTextCentered(answer.c_str());
                    vTaskDelay(3000 / portTICK_PERIOD_MS);
                    touch->clearEvents();
                    showPrompt();
                }
            }
        });
    }
};

#endif // MODE_MAGIC8BALL_HPP
