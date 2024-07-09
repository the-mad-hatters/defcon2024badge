#ifndef MODE_ABOUT_HPP
#define MODE_ABOUT_HPP

#include "badge/BadgeMode.h"
#include "badge/BadgeManager.h"

static const char *TAG_ABOUTMODE = "AboutMode";

class AboutMode : public BadgeMode {
  public:
    AboutMode() : BadgeMode(ModeType::ABOUT) {
    }

    void enter() override {
        ESP_LOGD(TAG_ABOUTMODE, "Entering About mode");
        display->clear();
        display->setFont(u8g2_font_ncenB08_tr);
        display->showTextCentered("Mad Hatter Badge\nv1.0.0\n\nhttps://madhatters.lol");
    }

    void leave() override {
        ESP_LOGD(TAG_ABOUTMODE, "Exiting About mode");
        display->clear();
    }

  protected:
    void receiveTouch(TouchEvent event) override {
        if (event.pin == HANDSHAKE_1 && event.type == TOUCH_DOWN) {
            Badge::getInstance().setMode(ModeType::HOME);
        }
    }
};

#endif // MODE_ABOUT_HPP