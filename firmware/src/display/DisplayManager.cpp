#include "DisplayManager.h"
#include "esp_log.h"
#include "esp_task_wdt.h"

#include "sync.h"

QueueHandle_t textScrollEvents = xQueueCreate(5, sizeof(ScrollEvent));

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

void DisplayManager::showTextCenter(const uint8_t *font, const char *text) {
    ESP_LOGD(TAG, "Showing centered text: %s", text);
    int x, y;
    {
        std::lock_guard<std::mutex> lock(displayMutex);
        u8g2.setFont(font);
        int textWidth     = u8g2.getStrWidth(text);
        int textHeight    = u8g2.getMaxCharHeight();
        int displayWidth  = u8g2.getDisplayWidth();
        int displayHeight = u8g2.getDisplayHeight();
        x                 = std::max(0, (displayWidth - textWidth) / 2);
        y                 = std::max(0, (displayHeight - textHeight) / 2);
    }
    showTextAt(font, text, x, y);
}

void DisplayManager::showList(const uint8_t *font, const char *items[], int count, int selected) {
    const int arrowXOffset = 0;
    const int textXOffset  = selected == -1 ? 0 : 10;

    // Render the list
    stopAnimations();
    if (xSemaphoreTake(peripheralSync, portMAX_DELAY) == pdTRUE) {
        std::lock_guard<std::mutex> lock(displayMutex);
        u8g2.firstPage();
        do {
            u8g2.setFont(font);
            for (int i = 0; i < count; i++) {
                u8g2.drawStr(textXOffset, 10 * (i + 1), items[i]);
                if (i == selected) {
                    u8g2.drawGlyph(arrowXOffset, 10 * (i + 1), '>');
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
    self->scrollTaskHandle = NULL;
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

    scrolling = false;
}

void DisplayManager::stopAnimations() {
    ESP_LOGD(TAG, "Stopping animations");
    // Stop any scrolling text
    if (scrollTaskHandle) {
        ESP_LOGD(TAG, "Cleaning up scroll task");
        vTaskDelete(scrollTaskHandle);
        scrollTaskHandle  = NULL;
        ScrollEvent event = {
            .type = ScrollEventType::SCROLL_CANCEL,
        };
        xQueueSend(textScrollEvents, &event, 0);
    }
    scrolling = false;
}
