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
    ESP_LOGD(TAG, "Display initialized");
    initialized = true;
}

void DisplayManager::clear() {
    ESP_LOGD(TAG, "Clearing display");
    stopAnimations();
    if (xSemaphoreTake(peripheralSync, portMAX_DELAY) == pdTRUE) {
        std::lock_guard<std::mutex> lock(displayMutex);
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
            u8g2.setFont(u8g2_font_ncenB14_tr);
            u8g2.drawXBMP(x, y, image.width, image.height, image.data);
        } while (u8g2.nextPage());
        xSemaphoreGive(peripheralSync);
    }
}

void DisplayManager::showTextAt(const uint8_t *font, const char *text, u8g2_uint_t x,
                                u8g2_uint_t y) {
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

void DisplayManager::showTextCentered(const uint8_t *font, const char *text) {
    stopAnimations();

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

    int textHeight    = 0;
    int displayWidth  = 0;
    int displayHeight = 0;
    int y             = 0;
    {
        std::lock_guard<std::mutex> lock(displayMutex);
        u8g2.setFont(font);
        int lineHeight = u8g2.getMaxCharHeight();
        textHeight     = lineHeight * lines.size();
        displayWidth   = u8g2.getDisplayWidth();
        displayHeight  = u8g2.getDisplayHeight();
        y              = std::max(0, (displayHeight - textHeight) / 2);
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

void DisplayManager::showList(const uint8_t *font, const char *items[], int count, int selected) {
    const char *arrowChar  = "➜"; // 0x279C in the unifont font
    const int arrowPadding = 4;

    int displayWidth = 0, displayHeight = 0, textWidth = 0, textHeight = 0, textXOffset = 0,
        arrowWidth = 0, arrowHeight = 0, arrowXOffset = 0, arrowYOffset = 0, maxItemsPerPage = 0,
        scrollOffset = 0;
    {
        std::lock_guard<std::mutex> lock(displayMutex);
        // Calculate the text width and height and get display dimensions
        u8g2.setFont(font);
        textWidth     = 0;
        textHeight    = u8g2.getMaxCharHeight();
        displayWidth  = u8g2.getDisplayWidth();
        displayHeight = u8g2.getDisplayHeight();
        for (int i = 0; i < count; i++) {
            textWidth = std::max(textWidth, (int)u8g2.getStrWidth(items[i]));
        }
        maxItemsPerPage = displayHeight / textHeight;

        // Calculate the arrow position and text offset
        if (selected >= 0) {
            u8g2.setFont(u8g2_font_unifont_t_78_79);
            arrowWidth   = u8g2.getUTF8Width(arrowChar);
            arrowHeight  = u8g2.getMaxCharHeight();
            arrowYOffset = (textHeight - arrowHeight) / 2 - 1; // Offset to vertically center the
                                                               // arrow in the text
            textXOffset = arrowWidth + arrowPadding;

            // Adjust the scroll offset to ensure the selected item is fully visible
            if (selected >= maxItemsPerPage - 1) {
                scrollOffset = selected - (maxItemsPerPage - 2);
            }

            // Ensure the selected item is not partially cut off at the bottom
            if (selected < scrollOffset) {
                scrollOffset = selected;
            } else if (selected >= scrollOffset + maxItemsPerPage - 1) {
                scrollOffset = selected - (maxItemsPerPage - 2);
            }
        }
    }

    // Render the list
    stopAnimations();
    if (xSemaphoreTake(peripheralSync, portMAX_DELAY) == pdTRUE) {
        std::lock_guard<std::mutex> lock(displayMutex);
        u8g2.firstPage();
        do {
            for (int i = scrollOffset; i < std::min(count, scrollOffset + maxItemsPerPage); i++) {
                int y = (i - scrollOffset + 1) * textHeight;
                u8g2.setFont(font);
                u8g2.drawStr(textXOffset, y, items[i]);
                if (i == selected) {
                    u8g2.setFont(u8g2_font_unifont_t_78_79);
                    u8g2.drawUTF8(arrowXOffset, y + arrowYOffset, arrowChar);
                }
            }
        } while (u8g2.nextPage());
        xSemaphoreGive(peripheralSync);
    }
}

void DisplayManager::scrollText(const uint8_t *font, const char *text, int speed, int iterations,
                                int offset) {
    stopAnimations();

    // Clear the event queue
    xQueueReset(textScrollEvents);

    // Set the scroll parameters
    scrollFont       = font;
    scrollTextBuffer = text;
    scrollSpeed      = speed;
    scrollIterations = iterations;
    scrollOffset     = offset;
    scrolling        = true;

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
    self->scrolling        = false;
    self->scrollTaskHandle = NULL;
    if (TaskHandle_t haltingTask = scrollHaltNotify.load()) {
        scrollHaltNotify.store(nullptr);
        xTaskNotifyGive(haltingTask);
    }
    vTaskDelete(NULL);
}

void DisplayManager::doScrollText() {
    int textWidth = 0, textHeight = 0, displayWidth = 0, displayHeight = 0, scrollX = 0,
        scrollY = 0;
    {
        std::lock_guard<std::mutex> lock(displayMutex);
        u8g2.setFont(scrollFont);
        textWidth     = u8g2.getStrWidth(scrollTextBuffer);
        textHeight    = u8g2.getMaxCharHeight();
        displayWidth  = u8g2.getDisplayWidth();
        displayHeight = u8g2.getDisplayHeight();
        scrollX       = displayWidth;
        scrollY       = scrollOffset >= 0 ? scrollOffset : 0;
        if (scrollOffset == SCROLL_OFFSET_RANDOM) {
            scrollY = random(displayHeight - textHeight);
        } else if (scrollOffset == SCROLL_OFFSET_MIDDLE) {
            scrollY = (displayHeight - textHeight) / 2;
        }
    }

    for (int i = 0; i < scrollIterations || scrollIterations == SCROLL_FOREVER; i++) {
        // Send a scroll start event
        ScrollEvent startEvent = {
            .type = ScrollEventType::SCROLL_START,
        };
        xQueueSend(textScrollEvents, &startEvent, 0);

        // Scroll the text across the display
        for (int offset = 0; offset < textWidth + displayWidth; offset++) {
            if (!scrolling) {
                return;
            }

            if (xSemaphoreTake(peripheralSync, portMAX_DELAY) == pdTRUE) {
                {
                    std::lock_guard<std::mutex> lock(displayMutex);
                    u8g2.firstPage();
                    do {
                        u8g2.clearBuffer();
                        u8g2.setFontPosTop();
                        u8g2.drawStr(scrollX - offset, scrollY, scrollTextBuffer);
                    } while (u8g2.nextPage());
                }
                xSemaphoreGive(peripheralSync);
            }

            // Check if we need to stop scrolling and exit
            if (scrollHaltNotify.load()) {
                return;
            }

            // Delay to control the scroll speed
            vTaskDelay(scrollSpeed / portTICK_PERIOD_MS);
        }

        // Send a scroll end event
        ScrollEvent endEvent = {
            .type = ScrollEventType::SCROLL_END,
        };
        xQueueSend(textScrollEvents, &endEvent, 0);

        // If offset is random, generate a new random offset
        if (scrollOffset == SCROLL_OFFSET_RANDOM) {
            scrollY = random(displayHeight - textHeight);
        }
    }
}

void DisplayManager::stopAnimations() {
    ESP_LOGD(TAG, "Stopping animations");
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
    scrolling = false;
}
