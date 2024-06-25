#define FASTLED_RMT_MAX_CHANNELS 1

#include <Arduino.h>
#include <Wire.h>
#include <SPIFFS.h>
#include "sync.h"
#include "badge/BadgeManager.h"

SemaphoreHandle_t peripheralSync = xSemaphoreCreateBinary();

static const char *TAG = "main";

Badge &badge = Badge::getInstance();

void setup() {
    Serial.begin(115200);
    while (!Serial) {
        delay(10);
    }
    Serial.println("Starting up...");

    // Log the current log level
    static const char *ESP_LOG_LEVEL_NAMES[] = {"None", "Error", "Warn",
                                                "Info", "Debug", "Verbose"};
    Serial.printf("Current log level: %d: %s\n", CORE_DEBUG_LEVEL,
                  ESP_LOG_LEVEL_NAMES[CORE_DEBUG_LEVEL]);

    // Give the bus semaphore so tasks can begin running things that use bus peripherals
    xSemaphoreGive(peripheralSync);

    // In debug mode, scan for I2C devices on the OLED pins
    if (CORE_DEBUG_LEVEL >= 4) {
        ESP_LOGD(TAG, "Scanning for I2C display devices on pins %d and %d...", OLED_SDA, OLED_SCL);

        // Scan for I2C devices
        Wire.begin(OLED_SDA, OLED_SCL);
        for (byte address = 1; address < 127; address++) {
            Wire.beginTransmission(address);
            if (Wire.endTransmission() == 0) {
                ESP_LOGD(TAG, "-> Device found at 7-bit address 0x%02X", address);
            }
        }
        Wire.end();
    }

    // Initialize SPIFFS for file system access
    if (!SPIFFS.begin(true)) {
        ESP_LOGE(TAG, "An Error has occurred while mounting SPIFFS");
        return;
    }

    // Log all files in the root directory for debugging purposes
    if (CORE_DEBUG_LEVEL >= 4) {
        ESP_LOGD(TAG, "Files in root directory:");
        File root = SPIFFS.open("/");
        File file = root.openNextFile();
        while (file) {
            Serial.print("FILE: ");
            Serial.println(file.name());
            file = root.openNextFile();
        }
    }

    // Initialize the badge
    badge.init();
}

void loop() {
    // Empty loop - we should never get here because everything is being done in other tasks
}
