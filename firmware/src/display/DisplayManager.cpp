#include <atomic>
#include "DisplayManager.h"
#include "esp_log.h"
#include "esp_task_wdt.h"

#include "sync.h"

QueueHandle_t textScrollEvents = xQueueCreate(5, sizeof(ScrollEvent));
std::atomic<TaskHandle_t> scrollHaltNotify(nullptr);

static const char *TAG = "DisplayManager";

void DisplayManager::init() {
    if (initialized) {
        return;
    }
    u8g2.setI2CAddress(OLED_ADDR);
    u8g2.begin();
    {
        std::lock_guard<std::mutex> lock(displayMutex);
        displayWidth  = u8g2.getDisplayWidth();
        displayHeight = u8g2.getDisplayHeight();
    }
    ESP_LOGD(TAG, "Display initialized");
    initialized = true;
}

void DisplayManager::getDisplaySize(int &width, int &height) {
    std::lock_guard<std::mutex> lock(displayMutex);
    if (displayWidth == 0 || displayHeight == 0) {
        displayWidth  = u8g2.getDisplayWidth();
        displayHeight = u8g2.getDisplayHeight();
    }
    width  = displayWidth;
    height = displayHeight;
}

DisplayManager::TextBounds DisplayManager::getTextBounds(const uint8_t *font, const char *text) {
    TextBounds bounds;

    std::lock_guard<std::mutex> lock(displayMutex);
    u8g2.setFont(font);
    bounds.height = u8g2.getMaxCharHeight();
    if (u8g2_IsAllValidUTF8(u8g2.getU8g2(), text)) {
        bounds.width = u8g2.getUTF8Width(text);
    } else {
        bounds.width = u8g2.getStrWidth(text);
    }

    return bounds;
}

void DisplayManager::clear() {
    ESP_LOGD(TAG, "Clearing display");
    stopAnimations();
    std::lock_guard<std::mutex> lock(displayMutex);
    currentComponent = ComponentType::NONE;
    if (xSemaphoreTake(peripheralSync, portMAX_DELAY) == pdTRUE) {
        u8g2.firstPage();
        do {
            u8g2.clearBuffer();
        } while (u8g2.nextPage());
        xSemaphoreGive(peripheralSync);
    }
}

void DisplayManager::drawImage(ImageID imageId, u8g2_uint_t x, u8g2_uint_t y) {
    const ImageData &image = IMAGES[static_cast<unsigned int>(imageId)];

    // Clear the display and draw the image
    stopAnimations();
    if (xSemaphoreTake(peripheralSync, portMAX_DELAY) == pdTRUE) {
        std::lock_guard<std::mutex> lock(displayMutex);
        u8g2.firstPage();
        do {
            u8g2.drawXBMP(x, y, image.width, image.height, image.data);
        } while (u8g2.nextPage());
        xSemaphoreGive(peripheralSync);
    }
}

void DisplayManager::showTextAt(const char *text, u8g2_uint_t x, u8g2_uint_t y) {
    stopAnimations();
    ESP_LOGD(TAG, "Showing text \"%s\" at (%d, %d)", text, x, y);
    if (xSemaphoreTake(peripheralSync, portMAX_DELAY) == pdTRUE) {
        std::lock_guard<std::mutex> lock(displayMutex);
        u8g2.firstPage();
        do {
            u8g2.setFont(font);
            u8g2.setFontPosTop();
            u8g2.drawStr(x, y, text);
        } while (u8g2.nextPage());
        xSemaphoreGive(peripheralSync);
    }
}

void DisplayManager::showTextCentered(const char *text) {
    stopAnimations();

    // Split the text into lines so it's easier to calculate things and iterate over them
    std::vector<std::string> lines;
    std::string str(text);
    size_t pos = 0;
    std::string token;
    while ((pos = str.find('\n')) != std::string::npos) {
        token = str.substr(0, pos);
        lines.push_back(token);
        str.erase(0, pos + 1);
    }
    if (!str.empty()) {
        lines.push_back(str);
    }

    // Calculate the total text height and y offset
    int textBlockHeight = 0;
    int y               = 0;
    {
        std::lock_guard<std::mutex> lock(displayMutex);
        u8g2.setFont(font);
        int lineHeight  = u8g2.getMaxCharHeight();
        textBlockHeight = lineHeight * lines.size();
        y               = std::max(0, (displayHeight - textBlockHeight) / 2);
    }

    if (xSemaphoreTake(peripheralSync, portMAX_DELAY) == pdTRUE) {
        std::lock_guard<std::mutex> lock(displayMutex);
        u8g2.firstPage();
        do {
            u8g2.clearBuffer();
            u8g2.setFontPosTop();
            for (const auto &line : lines) {
                int textWidth = u8g2.getStrWidth(line.c_str());
                int x         = std::max(0, (displayWidth - textWidth) / 2);
                u8g2.drawStr(x, y, line.c_str());
                y += u8g2.getMaxCharHeight();
            }
        } while (u8g2.nextPage());
        xSemaphoreGive(peripheralSync);
    }
}

void DisplayManager::showList(const char *items[], int itemCount, int initialSelection,
                              ListCallback callback) {
    listState.items         = std::vector<std::string>(items, items + itemCount);
    listState.selectedIndex = initialSelection;
    listState.callback      = callback;
    renderList();
}

void DisplayManager::showPrompt(const char *prompt, const char *options[], int optionCount,
                                int initialSelection, PromptCallback callback) {
    promptState.prompt         = prompt;
    promptState.options        = std::vector<std::string>(options, options + optionCount);
    promptState.selectedOption = initialSelection;
    promptState.callback       = callback;
    renderPrompt();
}

void DisplayManager::showTextEntry(const char *prompt, const char *initialText,
                                   const char initialCharSelected, TextEntryCallback callback) {
    textEntryState.prompt      = prompt;
    textEntryState.enteredText = initialText;
    textEntryState.callback    = callback;
    // Find the initial character index
    textEntryState.selectedCharIndex = textEntryState.availableChars.find(initialCharSelected);
    if (textEntryState.selectedCharIndex == std::string::npos) {
        textEntryState.selectedCharIndex = 0;
    }
    renderTextEntry();
}

void DisplayManager::renderList() {
    currentComponent = ComponentType::LIST;

    // Calculate the list bounds and scroll offset
    ListBounds bounds = getListBounds(listState.items, listState.selectedIndex);
    int itemsPerPage  = displayHeight / bounds.items.itemHeight;
    int scrollOffset  = getListScrollOffset(listState.selectedIndex, itemsPerPage);

    // Render the list
    stopAnimations();
    if (xSemaphoreTake(peripheralSync, portMAX_DELAY) == pdTRUE) {
        std::lock_guard<std::mutex> lock(displayMutex);
        u8g2.firstPage();
        do {
            for (int i = scrollOffset;
                 i < std::min(listState.items.size(),
                              static_cast<std::size_t>(scrollOffset + itemsPerPage));
                 i++) {
                int y = (i - scrollOffset + 1) * bounds.items.itemHeight;
                u8g2.setFont(font);
                u8g2.drawStr(bounds.items.offset.x, y, listState.items[i].c_str());
                if (i == listState.selectedIndex) {
                    u8g2.setFont(symbolFont);
                    u8g2.drawUTF8(bounds.arrow.offset.x, y + bounds.arrow.offset.y, arrowChar);
                }
            }
        } while (u8g2.nextPage());
        xSemaphoreGive(peripheralSync);
    }
}

void DisplayManager::renderPrompt() {
    currentComponent = ComponentType::PROMPT;

    const int promptPadding = 8;
    TextBounds promptBounds = getTextBounds(font, promptState.prompt.c_str());

    // Calculate the list bounds and scroll offset
    ListBounds listBounds = getListBounds(promptState.options, promptState.selectedOption);
    int itemsPerPage =
        (displayHeight - promptBounds.height - promptPadding) / listBounds.items.itemHeight;
    int scrollOffset = getListScrollOffset(promptState.selectedOption, itemsPerPage);

    // Render the prompt
    stopAnimations();
    if (xSemaphoreTake(peripheralSync, portMAX_DELAY) == pdTRUE) {
        std::lock_guard<std::mutex> lock(displayMutex);
        u8g2.firstPage();
        do {
            // Draw prompt centered at the top
            u8g2.setFont(font);
            int promptWidth = u8g2.getStrWidth(promptState.prompt.c_str());
            int promptX     = (displayWidth - promptWidth) / 2;
            u8g2.drawStr(promptX, promptPadding, promptState.prompt.c_str());

            // Draw options centered below the prompt
            int yOffset = listBounds.items.itemHeight;
            for (int i = scrollOffset;
                 i < std::min(promptState.options.size(),
                              static_cast<std::size_t>(scrollOffset + itemsPerPage));
                 i++) {
                int y = yOffset + (promptPadding * 2) +
                        (i - scrollOffset) * listBounds.items.itemHeight;
                int optionWidth = u8g2.getStrWidth(promptState.options[i].c_str());
                int optionX     = std::max(0, (displayWidth - listBounds.items.width)) / 2;
                u8g2.setFont(font);
                u8g2.drawStr(optionX, y, promptState.options[i].c_str());
                if (i == promptState.selectedOption) {
                    u8g2.setFont(u8g2_font_unifont_t_78_79);
                    u8g2.drawUTF8(optionX - (listBounds.arrow.width + arrowPadding -
                                             listBounds.arrow.offset.x),
                                  y + listBounds.arrow.offset.y, arrowChar);
                }
            }
        } while (u8g2.nextPage());
        xSemaphoreGive(peripheralSync);
    }
}

void DisplayManager::renderTextEntry() {
    currentComponent = ComponentType::TEXT_ENTRY;

    const int promptPadding   = 8;
    const int cursorPadding   = 4;
    const int selectorPadding = 2;

    TextBounds promptBounds = getTextBounds(font, textEntryState.prompt.c_str());
    TextBounds entryBounds  = getTextBounds(font, textEntryState.enteredText.c_str());
    TextBounds selectedChar;
    {
        std::lock_guard<std::mutex> lock(displayMutex);
        u8g2.setFont(largerFont);
        selectedChar = {
            .width  = u8g2.getMaxCharWidth(),
            .height = u8g2.getMaxCharHeight(),
        };
    }

    // Calculate the visible part of the character list
    int visibleChars = displayWidth / (selectedChar.width + selectorPadding);
    int visibleStart = std::max(0, textEntryState.selectedCharIndex - visibleChars / 2);
    int visibleEnd   = std::min(textEntryState.availableChars.length(),
                                static_cast<std::size_t>(visibleStart + visibleChars));

    // Ensure the selected character is visible
    if (textEntryState.selectedCharIndex < visibleStart) {
        visibleStart = textEntryState.selectedCharIndex;
    } else if (textEntryState.selectedCharIndex >= visibleEnd) {
        visibleStart = textEntryState.selectedCharIndex - visibleChars + 1;
    }

    // Render the text entry UI
    stopAnimations();
    if (xSemaphoreTake(peripheralSync, portMAX_DELAY) == pdTRUE) {
        std::lock_guard<std::mutex> lock(displayMutex);
        u8g2.firstPage();
        do {
            // Draw prompt centered at the top
            u8g2.setFont(font);
            int promptX = (displayWidth - promptBounds.width) / 2;
            u8g2.drawStr(promptX, promptPadding, textEntryState.prompt.c_str());

            // Draw entered text with cursor
            int enteredX = cursorPadding;
            int enteredY = promptPadding + entryBounds.height + cursorPadding;
            u8g2.setFont(font);
            u8g2.drawStr(enteredX, enteredY, textEntryState.enteredText.c_str());
            u8g2.drawStr(enteredX + entryBounds.width, enteredY, "_");

            // Draw a line to separate the entered text from the character selector
            int lineY = enteredY + entryBounds.height + cursorPadding;
            u8g2.drawLine(0, lineY, displayWidth, lineY);

            // Draw available characters with selected character highlighted
            int selectionY = displayHeight - selectedChar.height - cursorPadding;
            int selectionX =
                (displayWidth - (selectedChar.width + selectorPadding) * visibleChars) / 2;

            for (int i = visibleStart; i < visibleEnd; ++i) {
                int charX =
                    selectionX + (i - visibleStart) * (selectedChar.width + selectorPadding);
                char charToDraw[2] = {textEntryState.availableChars[i], '\0'};

                if (i == textEntryState.selectedCharIndex) {
                    u8g2.setFont(largerFont);
                    u8g2.setDrawColor(1);
                    int boxWidth  = selectedChar.width + selectorPadding;
                    int boxHeight = selectedChar.height + 2;
                    int boxX      = charX - (boxWidth - selectedChar.width) - 1;
                    int boxY      = selectionY - (boxHeight - selectedChar.height);
                    u8g2.drawRBox(boxX, boxY, boxWidth, boxHeight, 3);
                    u8g2.setDrawColor(0);
                    u8g2.drawStr(charX, selectionY, charToDraw);
                    u8g2.setFont(font);
                    u8g2.setDrawColor(1);
                } else {
                    u8g2.drawStr(charX, selectionY + (selectedChar.height - entryBounds.height) / 2,
                                 charToDraw);
                }
            }
        } while (u8g2.nextPage());
        xSemaphoreGive(peripheralSync);
    }
}

bool DisplayManager::handleTouch(TouchEvent event) {
    // Track all input states so we can implement multi-touch actions
    if (inputState[event.pin] != event.type) {
        inputState[event.pin] = event.type;
    }

    // Track the start time of a touch event for repeat actions
    bool repeatAction = event.type == TOUCH_DOWN && millis() - holdStartTime[event.pin] >= 500;
    if ((event.changed || repeatAction) && event.type == TOUCH_DOWN) {
        holdStartTime[event.pin] = millis();
    } else if (event.type == TOUCH_UP) {
        holdStartTime[event.pin] = 0;
    }

    // Components that want to handle any touch event
    ComponentType handleAnyType[] = {
        ComponentType::TEXT_ENTRY, // Text entry will want to handle TOUCH_UP and TOUCH_DOWN
    };

    // Unless it's a TOUCH_DOWN event or the component is in the handleAnyType list, we don't want
    // to handle the event
    if (std::find(std::begin(handleAnyType), std::end(handleAnyType), currentComponent) ==
            std::end(handleAnyType) &&
        (event.type != TOUCH_DOWN || (!event.changed && !repeatAction))) {
        return false;
    }

    // By default don't let mode touch handlers run while we're showing/handling a component
    bool suppressModeHandler = currentComponent != ComponentType::NONE;

    // Handle touch events based on the current component being displayed
    switch (currentComponent) {
        case ComponentType::LIST:
            if (event.pin == HANDSHAKE_2) {
                ESP_LOGD(TAG, "Scrolling list up");
                listState.selectedIndex =
                    (listState.selectedIndex - 1 + listState.items.size()) % listState.items.size();
                renderList();
            } else if (event.pin == HANDSHAKE_3) {
                ESP_LOGD(TAG, "Scrolling list down");
                listState.selectedIndex = (listState.selectedIndex + 1) % listState.items.size();
                renderList();
            } else if (event.pin == HANDSHAKE_4) {
                currentComponent = ComponentType::NONE;
                ESP_LOGD(TAG, "Selecting list item");
                if (listState.callback) {
                    listState.callback(listState.selectedIndex);
                }
            }
            break;
        case ComponentType::PROMPT:
            if (event.pin == HANDSHAKE_2) {
                ESP_LOGD(TAG, "Scrolling prompt up");
                promptState.selectedOption =
                    (promptState.selectedOption - 1 + promptState.options.size()) %
                    promptState.options.size();
                renderPrompt();
            } else if (event.pin == HANDSHAKE_3) {
                ESP_LOGD(TAG, "Scrolling prompt down");
                promptState.selectedOption =
                    (promptState.selectedOption + 1) % promptState.options.size();
                renderPrompt();
            } else if (event.pin == HANDSHAKE_4) {
                currentComponent = ComponentType::NONE;
                ESP_LOGD(TAG, "Selecting prompt option");
                if (promptState.callback) {
                    promptState.callback(promptState.selectedOption);
                }
            }
            break;
        case ComponentType::TEXT_ENTRY:
            if (event.changed || repeatAction) {
                //
                // Special case for handling "backspace" (handshakes 2 and 3 pressed together)
                //

                if (inputState[HANDSHAKE_2] == TOUCH_DOWN &&
                    inputState[HANDSHAKE_3] == TOUCH_DOWN) {
                    if (textEntryState.enteredText.length() > 0) {
                        textEntryState.enteredText.pop_back();
                        renderTextEntry();
                    }
                    textEntryState.action = TextEntryAction::BACKSPACE;
                    break;
                }
                if (inputState[HANDSHAKE_2] == TOUCH_UP && inputState[HANDSHAKE_3] == TOUCH_UP) {
                    if (textEntryState.action == TextEntryAction::BACKSPACE) {
                        textEntryState.action = TextEntryAction::NONE;
                        break;
                    }
                }

                //
                // Other normal touch events
                //

                // Reset the action if the user lifts their finger
                if (event.type == TOUCH_UP && textEntryState.action != TextEntryAction::BACKSPACE) {
                    textEntryState.action = TextEntryAction::NONE;
                }
                // Handle the touch events
                if (event.type == TOUCH_DOWN &&
                    textEntryState.action != TextEntryAction::BACKSPACE) {
                    if (event.pin == HANDSHAKE_1) {
                        currentComponent      = ComponentType::NONE;
                        textEntryState.action = TextEntryAction::NONE;
                        ESP_LOGD(TAG, "Exiting text entry");
                        if (textEntryState.callback) {
                            textEntryState.callback(textEntryState.enteredText);
                        }
                    }
                    if (event.pin == HANDSHAKE_2) {
                        ESP_LOGD(TAG, "Scrolling text entry left");
                        textEntryState.selectedCharIndex =
                            (textEntryState.selectedCharIndex - 1 +
                             textEntryState.availableChars.length()) %
                            textEntryState.availableChars.length();
                        renderTextEntry();
                        textEntryState.action = TextEntryAction::LEFT;
                    }
                    if (event.pin == HANDSHAKE_3) {
                        ESP_LOGD(TAG, "Scrolling text entry right");
                        textEntryState.selectedCharIndex = (textEntryState.selectedCharIndex + 1) %
                                                           textEntryState.availableChars.length();
                        renderTextEntry();
                        textEntryState.action = TextEntryAction::RIGHT;
                    }
                    if (event.pin == HANDSHAKE_4) {
                        ESP_LOGD(TAG, "Selecting text entry character");
                        textEntryState.enteredText +=
                            textEntryState.availableChars[textEntryState.selectedCharIndex];
                        renderTextEntry();
                        textEntryState.action = TextEntryAction::SELECT;
                    }
                }
            }
            break;
    }

    return suppressModeHandler;
}

void DisplayManager::scrollText(const char *text) {
    stopAnimations();

    // Clear the event queue
    xQueueReset(textScrollEvents);

    // Set the scroll parameters
    scrollState.text      = text;
    scrollState.scrolling = true;

    // Create the scroll task
    if (!scrollTaskHandle) {
        ESP_LOGD(TAG, "Creating scroll task");
        xTaskCreate(scrollTask, "ScrollTask", 2048, this, 5, &scrollTaskHandle);
    }
}

void DisplayManager::scrollTask(void *pvParameters) {
    DisplayManager *self = static_cast<DisplayManager *>(pvParameters);
    ESP_LOGD(TAG, "Scroll task started. Task: %p, Priority: %d", self->scrollTaskHandle,
             uxTaskPriorityGet(NULL));
    self->doScrollText();
    ESP_LOGD(TAG, "Scroll task complete... cleaning up");
    self->scrollState.scrolling = false;
    self->scrollTaskHandle      = NULL;
    if (TaskHandle_t haltingTask = scrollHaltNotify.load()) {
        scrollHaltNotify.store(nullptr);
        xTaskNotifyGive(haltingTask);
    }
    vTaskDelete(NULL);
}

void DisplayManager::doScrollText() {
    TextBounds bounds = getTextBounds(font, scrollState.text);
    int scrollX = 0, scrollY = 0;
    {
        std::lock_guard<std::mutex> lock(displayMutex);
        scrollX = displayWidth;
        scrollY = scrollState.alignment > 0 ? scrollState.alignment : 0;

        if (scrollState.alignment == SCROLL_ALIGN_RANDOM) {
            scrollY = random(displayHeight - bounds.height);
        } else if (scrollState.alignment == SCROLL_ALIGN_MIDDLE) {
            scrollY = (displayHeight - bounds.height) / 2;
        }
    }

    for (int i = 0; i < scrollState.iterations || scrollState.iterations == SCROLL_FOREVER; i++) {
        // Send a scroll start event
        ScrollEvent startEvent = {
            .type = ScrollEventType::SCROLL_START,
        };
        xQueueSend(textScrollEvents, &startEvent, 0);

        // Scroll the text across the display
        for (int offset = 0; offset < bounds.width + displayWidth; offset++) {
            if (!scrollState.scrolling) {
                return;
            }

            if (xSemaphoreTake(peripheralSync, portMAX_DELAY) == pdTRUE) {
                {
                    std::lock_guard<std::mutex> lock(displayMutex);
                    u8g2.firstPage();
                    do {
                        u8g2.clearBuffer();
                        u8g2.setFontPosTop();
                        u8g2.drawStr(scrollX - offset, scrollY, scrollState.text);
                    } while (u8g2.nextPage());
                }
                xSemaphoreGive(peripheralSync);
            }

            // Check if we need to stop scrolling and exit
            if (scrollHaltNotify.load()) {
                return;
            }

            // Delay to control the scroll speed
            vTaskDelay(scrollState.speed / portTICK_PERIOD_MS);
        }

        // Send a scroll end event
        ScrollEvent endEvent = {
            .type = ScrollEventType::SCROLL_END,
        };
        xQueueSend(textScrollEvents, &endEvent, 0);

        // If offset is random, generate a new random offset
        if (scrollState.alignment == SCROLL_ALIGN_RANDOM) {
            scrollY = random(displayHeight - bounds.height);
        }
    }
}

void DisplayManager::stopAnimations() {
    // ESP_LOGD(TAG, "Stopping animations");
    // Stop any scrolling text
    if (scrollTaskHandle) {
        ESP_LOGD(TAG, "Cleaning up scroll task");

        // Notify the task to stop and wait for it to exit
        scrollHaltNotify.store(xTaskGetCurrentTaskHandle());
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        // Send a scroll cancel event
        ScrollEvent event = {
            .type = ScrollEventType::SCROLL_CANCEL,
        };
        xQueueSend(textScrollEvents, &event, 0);
    }
    scrollState.scrolling = false;
}

DisplayManager::ListBounds DisplayManager::getListBounds(std::vector<std::string> items,
                                                         int selectedIndex) {
    ListBounds bounds;

    // Get dimensions for the list items
    {
        std::lock_guard<std::mutex> lock(displayMutex);
        u8g2.setFont(font);
        bounds.items.width = 0;
        for (const auto &item : items) {
            bounds.items.width = std::max(bounds.items.width, (int)u8g2.getStrWidth(item.c_str()));
        }
        bounds.items.itemHeight = u8g2.getMaxCharHeight();
        bounds.items.height     = items.size() * bounds.items.itemHeight;
    }

    // Get dimensions for the list arrows
    TextBounds arrowBounds = getTextBounds(symbolFont, arrowChar);
    std::lock_guard<std::mutex> lock(displayMutex);
    u8g2.setFont(symbolFont);
    bounds.arrow.width   = arrowBounds.width;
    bounds.arrow.height  = arrowBounds.height;
    bounds.arrow.padding = 4;
    u8g2.setFont(font);

    // Calculate the item and arrow offsets
    if (selectedIndex >= 0) {
        bounds.items.offset.x = bounds.arrow.width + bounds.arrow.padding;
        bounds.items.offset.y = 0;
        bounds.arrow.offset.x = 0;
        bounds.arrow.offset.y = (bounds.items.itemHeight - bounds.arrow.height) / 2 - 1;
    }

    return bounds;
}

int DisplayManager::getListScrollOffset(int selectedIndex, int maxItemsPerPage) {
    int scrollOffset = 0;

    // Adjust the scroll offset to ensure the selected item is fully visible
    if (selectedIndex >= maxItemsPerPage - 1) {
        scrollOffset = selectedIndex - (maxItemsPerPage - 2);
    }

    // Make sure the selected item is not partially cut off at the bottom
    if (selectedIndex < scrollOffset) {
        scrollOffset = selectedIndex;
    } else if (selectedIndex >= scrollOffset + maxItemsPerPage - 1) {
        scrollOffset = selectedIndex - (maxItemsPerPage - 2);
    }

    return scrollOffset;
}