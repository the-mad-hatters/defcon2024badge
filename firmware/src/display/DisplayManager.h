#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <mutex>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include <U8g2lib.h>
#include "config.h"
#include "touch/TouchHandler.h"
#include "Images.h"

#define SCROLL_ALIGN_MIDDLE -2
#define SCROLL_ALIGN_RANDOM -1
#define SCROLL_FOREVER      -1

// Callbacks for user input
using ListCallback      = std::function<void(int selected)>;
using PromptCallback    = std::function<void(int selected)>;
using TextEntryCallback = std::function<void(const std::string &text)>;

// Define some useful character sets
#define CHAR_SET_UPPER        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define CHAR_SET_LOWER        "abcdefghijklmnopqrstuvwxyz"
#define CHAR_SET_ALPHA        CHAR_SET_UPPER CHAR_SET_LOWER
#define CHAR_SET_NUMERIC      "0123456789"
#define CHAR_SET_SPACE        " "
#define CHAR_SET_ALPHANUMERIC CHAR_SET_ALPHA CHAR_SET_SPACE CHAR_SET_NUMERIC
#define CHAR_SET_SYMBOLS      "!@#$%^&*()-_=+[]{};:'\",.<>/?\\|`~"
#define CHAR_SET_ALL          CHAR_SET_ALPHANUMERIC CHAR_SET_SYMBOLS

// Define default fonts for rendering text
#define FONT_DEFAULT         u8g2_font_ncenB08_tr
#define FONT_DEFAULT_LARGER  u8g2_font_ncenB10_tr
#define FONT_DEFAULT_SYMBOLS u8g2_font_unifont_t_78_79

// Define default arrow character for lists
#define LIST_DEFAULT_ARROW_CHAR "➜" // 0x279C in the unifont font

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
    struct TextBounds {
        int width;
        int height;
    };

    static DisplayManager &getInstance() {
        static DisplayManager instance;
        return instance;
    }
    void init();

    // ----------------------------------------------------------------------------------------- //
    // Setters for various display/component properties                                          //
    // ----------------------------------------------------------------------------------------- //

    void setFont(const uint8_t *font) {
        setFont(font, NULL);
    }
    void setFont(const uint8_t *font, const uint8_t *largerFont) {
        std::lock_guard<std::mutex> lock(displayMutex);
        this->font       = font == NULL ? FONT_DEFAULT : font;
        this->largerFont = largerFont == NULL ? FONT_DEFAULT_LARGER : largerFont;
    }
    void setSymbolFont(const uint8_t *font) {
        std::lock_guard<std::mutex> lock(displayMutex);
        this->symbolFont = font == NULL ? FONT_DEFAULT_SYMBOLS : font;
    }
    void setArrowChar(const char *arrowChar) {
        std::lock_guard<std::mutex> lock(displayMutex);
        this->arrowChar = arrowChar == NULL ? LIST_DEFAULT_ARROW_CHAR : arrowChar;
    }
    void setArrowPadding(int padding) {
        std::lock_guard<std::mutex> lock(displayMutex);
        arrowPadding = padding;
    }
    void setAvailableChars(std::string availableChars = CHAR_SET_ALL) {
        std::lock_guard<std::mutex> lock(displayMutex);
        textEntryState.availableChars = availableChars;
    }
    void setScrollSpeed(int speed) {
        std::lock_guard<std::mutex> lock(displayMutex);
        scrollState.speed = speed;
    }
    void setScrollIterations(int iterations) {
        std::lock_guard<std::mutex> lock(displayMutex);
        scrollState.iterations = iterations;
    }
    void setScrollAlignment(int alignment) {
        std::lock_guard<std::mutex> lock(displayMutex);
        scrollState.alignment = alignment;
    }

    // ----------------------------------------------------------------------------------------- //
    // Getters for calculated/display properties                                                 //
    // ----------------------------------------------------------------------------------------- //

    void getDisplaySize(int &width, int &height);
    TextBounds getTextBounds(const uint8_t *font, const char *text);

    // ----------------------------------------------------------------------------------------- //
    // Main rendering functions                                                                  //
    // ----------------------------------------------------------------------------------------- //

    void clear();
    void drawImage(ImageID imageId, u8g2_uint_t x, u8g2_uint_t y);
    void showTextAt(const char *text, u8g2_uint_t x, u8g2_uint_t y);
    void showTextCentered(const char *text);
    void showList(const char *items[], int itemCount, int initialSelection, ListCallback callback);
    void showPrompt(const char *prompt, const char *options[], int optionCount,
                    int initialSelection, PromptCallback callback);
    void showTextEntry(const char *prompt, const char *initialText, const char initialCharSelected,
                       TextEntryCallback callback);

    // ----------------------------------------------------------------------------------------- //
    // More complex state-based component renderers                                              //
    // ----------------------------------------------------------------------------------------- //

    void renderList();
    void renderPrompt();
    void renderTextEntry();

    // Handle touch events (mostly used for user input on prompts and text entry components)
    bool handleTouch(TouchEvent event);

    // ----------------------------------------------------------------------------------------- //
    // Animated components                                                                       //
    // ----------------------------------------------------------------------------------------- //

    void scrollText(const char *text);

  private:
    // Types of components that can be rendered - used for updating state in handleTouch()
    enum class ComponentType {
        NONE,
        LIST,
        PROMPT,
        TEXT_ENTRY,
    };

    // ----------------------------------------------------------------------------------------- //
    // Bounds + state for rendering list components                                              //
    // ----------------------------------------------------------------------------------------- //

    // Bounds for the list items
    struct ListItemBounds {
        int width;
        int height;
        int itemHeight;
        struct {
            int x;
            int y;
        } offset;
    };
    // Bounds for the list arrows
    struct ListArrowBounds {
        int width;
        int height;
        int padding;
        struct {
            int x;
            int y;
        } offset;
    };
    // Combined bounds and info for rendering the list
    struct ListBounds {
        ListItemBounds items;
        ListArrowBounds arrow;
    };

    // List state
    struct ListState {
        std::vector<std::string> items;
        int selectedIndex;
        ListCallback callback;
    };

    // ----------------------------------------------------------------------------------------- //
    // State for rendering prompts                                                               //
    // ----------------------------------------------------------------------------------------- //

    // Prompt state
    struct PromptState {
        std::string prompt;
        std::vector<std::string> options;
        int selectedOption;
        PromptCallback callback;
    };

    // ----------------------------------------------------------------------------------------- //
    // State for rendering text entry components                                                 //
    // ----------------------------------------------------------------------------------------- //

    // Text entry last action type
    enum class TextEntryAction {
        NONE,
        SELECT,
        BACKSPACE,
        LEFT,
        RIGHT,
    };

    // Text entry state
    struct TextEntryState {
        std::string prompt;
        std::string enteredText;
        std::string availableChars;
        int selectedCharIndex;
        TextEntryAction action;
        TextEntryCallback callback;
    };

    // ----------------------------------------------------------------------------------------- //
    // State for animated components                                                             //
    // ----------------------------------------------------------------------------------------- //

    // State for scrolling text
    struct ScrollTextState {
        const char *text;
        int speed;
        int iterations;
        int alignment;
        bool scrolling;
    };

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
        , displayWidth(0)  // Set during init() or via getDisplaySize()
        , displayHeight(0) // Set during init() or via getDisplaySize()
        , font(FONT_DEFAULT)
        , largerFont(FONT_DEFAULT_LARGER)
        , symbolFont(FONT_DEFAULT_SYMBOLS)
        , arrowChar(LIST_DEFAULT_ARROW_CHAR)
        , arrowPadding(4)
        , inputState{TOUCH_UP, TOUCH_UP, TOUCH_UP, TOUCH_UP}
        , currentComponent(ComponentType::NONE)
        , listState{std::vector<std::string>(), 0, nullptr}
        , promptState{"", std::vector<std::string>(), 0, nullptr}
        , textEntryState{"", "", CHAR_SET_ALL, 0, TextEntryAction::NONE, nullptr}
        , scrollState{nullptr, 20, SCROLL_FOREVER, SCROLL_ALIGN_MIDDLE}
        , scrollTaskHandle(NULL) {};
    DisplayManager(const DisplayManager &)            = delete;
    DisplayManager &operator=(const DisplayManager &) = delete;
    bool initialized                                  = false;

    inline void setComponent(ComponentType component);

    // Helper for calculating list bounds and state
    ListBounds getListBounds(std::vector<std::string> items, int selectedIndex);
    int getListScrollOffset(int selectedIndex, int maxItemsPerPage);

    static void scrollTask(void *pvParameters);
    void doScrollText();
    void stopAnimations();

    U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2;
    std::mutex displayMutex;
    u8g2_uint_t displayWidth;
    u8g2_uint_t displayHeight;
    const uint8_t *font;
    const uint8_t *largerFont;
    const uint8_t *symbolFont;
    const char *arrowChar;
    int arrowPadding;

    // Components and state
    TouchEventType inputState[HANDSHAKE_COUNT]   = {TOUCH_UP, TOUCH_UP, TOUCH_UP, TOUCH_UP};
    unsigned long holdStartTime[HANDSHAKE_COUNT] = {0, 0, 0, 0};
    ComponentType currentComponent;
    ListState listState;
    PromptState promptState;
    TextEntryState textEntryState;

    // Scroll text animation state
    ScrollTextState scrollState;
    TaskHandle_t scrollTaskHandle;
};

#endif // DISPLAY_MANAGER_H
