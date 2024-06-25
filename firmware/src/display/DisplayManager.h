#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <mutex>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include <U8g2lib.h>
#include "config.h"
#include "Images.h"

#define SCROLL_OFFSET_MIDDLE -2
#define SCROLL_OFFSET_RANDOM -1
#define SCROLL_FOREVER       -1

// Events for text scrolling
enum class ScrollEventType {
    SCROLL_START,
    SCROLL_END,
    SCROLL_CANCEL,
};
struct ScrollEvent {
    ScrollEventType type;
};
extern QueueHandle_t textScrollEvents;

class DisplayManager {
  public:
    static DisplayManager &getInstance() {
        static DisplayManager instance;
        return instance;
    }
    void init();
    void clear();
    void drawImage(ImageID imageId, u8g2_uint_t x, u8g2_uint_t y);
    void showTextAt(const uint8_t *font, const char *text, u8g2_uint_t x, u8g2_uint_t y);
    void showTextCentered(const uint8_t *font, const char *text);
    void showList(const uint8_t *font, const char *items[], int count, int selected);
    void scrollText(const uint8_t *font, const char *text, int speed = 20,
                    int iterations = SCROLL_FOREVER, int offset = SCROLL_OFFSET_MIDDLE);

  private:
    DisplayManager()
        : u8g2(U8G2_R0, // Rotation
#if defined(OLED_RST) && (OLED_RST >= 0)
               OLED_RST, // Reset pin
#else
               U8X8_PIN_NONE, // Reset pin
#endif
               OLED_SCL, // Clock pin
               OLED_SDA  // Data pin
               )
        , scrolling(false)
        , scrollIterations(-1) {};
    DisplayManager(const DisplayManager &)            = delete;
    DisplayManager &operator=(const DisplayManager &) = delete;
    bool initialized                                  = false;

    static void scrollTask(void *pvParameters);
    void doScrollText();
    void stopAnimations();

    U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2;
    const uint8_t *scrollFont;
    const char *scrollTextBuffer;
    int scrollSpeed;
    int scrollIterations;
    int scrollOffset;
    bool scrolling;
    TaskHandle_t scrollTaskHandle;
    std::mutex displayMutex;
};

#endif // DISPLAY_MANAGER_H
