#ifndef MODE_MAD_HATTER_METER_HPP
#define MODE_MAD_HATTER_METER_HPP

#include <vector>
#include <string>
#include <deque>
#include <mutex>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "MessageMode.hpp"

static const char *TAG_MADHATTERMETER = "MadHatterMeterMode";

// Customizable handshakes for calibration and audit
std::set<int> eMeterPins = {HANDSHAKE_2, HANDSHAKE_4};

class MadHatterMeterMode : public MessageMode {
  public:
    MadHatterMeterMode()
        : MessageMode(ModeType::MAD_HATTER_METER, "/emeter.txt", "/emeter_nsfw.txt")
        , inputStates(touch->getInputStates())
        , state(State::WAITING) {
    }

    void start() override {
        ESP_LOGD(TAG_MADHATTERMETER, "Entering Mad Hatter Meter mode");
        resetState();
        if (!taskHandle) {
            ESP_LOGD(TAG_MADHATTERMETER, "Starting Mad Hatter Meter state manager task");
            xTaskCreate(stateManagerTask, "MadHatterMeter:stateManagerTask", 4096, this, 5, NULL);
        }
    }

    void stop() override {
        ESP_LOGD(TAG_MADHATTERMETER, "Exiting Mad Hatter Meter mode");
        if (taskHandle) {
            vTaskDelete(taskHandle);
            taskHandle = NULL;
        }
        leds->unlockLed(AddressableStrip::TOUCH, 3);
    }

    void handleTouch(TouchEvent event) override {
        MessageMode::handleTouch(event);
        if (event.type != TOUCH_DOWN) {
            return;
        }

        // Check for exit
        if (event.pin == HANDSHAKE_1) {
            resetState();
            return;
        }

        // Check for calibration activation
        if (std::all_of(eMeterPins.begin(), eMeterPins.end(),
                        [this](int pin) { return inputStates[handshakePins.right(pin)] == TOUCH_DOWN; })) {
            if (getState() == State::WAITING) {
                ESP_LOGD(TAG_MADHATTERMETER, "Calibration activated");
                setState(State::CALIBRATING);
            }
        }
    }

  private:
    enum class State {
        WAITING,
        CALIBRATING,
        AUDITING,
    };
    QueueHandle_t stateQueue = xQueueCreate(1, sizeof(State));

    static const int touchEventBufferSizeMax = 100;

    std::mutex touchMutex;
    TouchEventType (&inputStates)[HANDSHAKE_COUNT];
    std::mutex stateMutex;
    State state;
    int initialAverage;
    double initialStandardDeviation;

    static void stateManagerTask(void *pvParameters) {
        MadHatterMeterMode *self = static_cast<MadHatterMeterMode *>(pvParameters);
        State state;

        while (true) {
            if (xQueueReceive(self->stateQueue, &state, portMAX_DELAY) == pdTRUE) {
                {
                    std::lock_guard<std::mutex> lock(self->stateMutex);
                    self->state = state;
                    ESP_LOGD(TAG_MADHATTERMETER, "State changed to %d", static_cast<int>(state));
                }
                self->handleState();
            }
        }
    }

    void handleState() {
        // Update state to match the currently set state
        switch (getState()) {
            case State::WAITING: {
                // Get configured handshakes for calibration and audit
                std::vector<int> handshakeIds;
                for (int pin : eMeterPins) {
                    handshakeIds.push_back(handshakePins.right(pin) + 1);
                }
                std::string handshakeDesc = "Enter handshakes " + join(handshakeIds, " and ") + " to  begin...";

                // Scroll instructions so that user knows what to do
                display->setFont(MESSAGE_FONT);
                display->setScrollSpeed(MESSAGE_SPEED);
                display->setScrollIterations(SCROLL_FOREVER);
                display->scrollText(handshakeDesc.c_str());
                break;
            }
            case State::CALIBRATING:
                display->setFont(MESSAGE_FONT);
                display->showTextCentered("Calibrating...");
                if (initializeEMeter()) {
                    setState(State::AUDITING);
                }
                break;
            case State::AUDITING: {
                display->setFont(MESSAGE_FONT);
                display->setScrollSpeed(MESSAGE_SPEED);
                display->setScrollIterations(SCROLL_FOREVER);
                const char *startMessage = "Started... don't remove hands...";
                display->scrollText(startMessage);
                performEMeterAudit();
                break;
            }
            default: break;
        }
    }

    void setState(State newState) {
        xQueueSend(stateQueue, &newState, portMAX_DELAY);
    }

    State getState() {
        std::lock_guard<std::mutex> lock(stateMutex);
        return state;
    }

    void resetState() {
        initialAverage           = 0;
        initialStandardDeviation = 0;
        setState(State::WAITING);
    }

    bool initializeEMeter() {
        const int numInitialReadings = 100;
        std::vector<int> initialReadings;
        {
            // Wait for numInitialReadings to be collected
            for (int i = 0; i < numInitialReadings; ++i) {
                vTaskDelay(100 / portTICK_PERIOD_MS);
                std::lock_guard<std::mutex> lock(touchMutex);
                if (std::all_of(eMeterPins.begin(), eMeterPins.end(),
                                [this](int pin) { return inputStates[handshakePins.right(pin)] == TOUCH_DOWN; })) {
                    int touchReading = 0;
                    for (auto pin : eMeterPins) {
                        touchReading += latestEvents[handshakePins.right(pin)].value;
                    }
                    initialReadings.push_back(touchReading);
                } else {
                    display->showTextCentered("Calibration\nfailed");
                    vTaskDelay(1000 / portTICK_PERIOD_MS);
                    resetState();
                    return false;
                }
            }
        }

        initialAverage           = calculateAverage(initialReadings);
        initialStandardDeviation = calculateStandardDeviation(initialReadings, initialAverage);

        ESP_LOGI(TAG_MADHATTERMETER, "Initial Average: %d", initialAverage);
        ESP_LOGI(TAG_MADHATTERMETER, "Initial Standard Deviation: %d", initialStandardDeviation);

        display->showTextCentered("Calibrated");
        leds->lockLed(AddressableStrip::TOUCH, 3, CRGB::Green);
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        return true;
    }

    void performEMeterAudit() {
        unsigned long setStartTime = 0;
        bool setState              = false;

        while (std::all_of(eMeterPins.begin(), eMeterPins.end(),
                           [this](int pin) { return inputStates[handshakePins.right(pin)] == TOUCH_DOWN; })) {
            ESP_LOGD(TAG_MADHATTERMETER, "Performing audit");
            int touchReading = 0;
            {
                std::lock_guard<std::mutex> lock(touchMutex);
                for (auto pin : eMeterPins) {
                    touchReading += latestEvents[handshakePins.right(pin)].value;
                }
            }

            double lowerBound = initialAverage - 3 * initialStandardDeviation;
            double upperBound = initialAverage + 3 * initialStandardDeviation;

            ESP_LOGD(TAG_MADHATTERMETER, "Touch reading: %d", touchReading);
            ESP_LOGD(TAG_MADHATTERMETER, "Lower bound: %d", lowerBound);
            ESP_LOGD(TAG_MADHATTERMETER, "Upper bound: %d", upperBound);

            if (touchReading >= lowerBound && touchReading <= upperBound) {
                leds->lockLed(AddressableStrip::TOUCH, 3, CRGB::Green);
                display->showTextCentered("SET");

                if (!setState) {
                    setState     = true;
                    setStartTime = millis();
                }

                if (setState && millis() - setStartTime >= 20000) {
                    showAuditMessage();
                    setState = false;
                }
            } else if (touchReading < lowerBound) {
                leds->lockLed(AddressableStrip::TOUCH, 3, CRGB::Red);
                display->showTextCentered("FALL");
                setState = false;
            } else {
                leds->lockLed(AddressableStrip::TOUCH, 3, CRGB::Red);
                display->showTextCentered("RISE");
                setState = false;
            }

            vTaskDelay(100 / portTICK_PERIOD_MS);
        }

        leds->unlockLed(AddressableStrip::TOUCH, 3);
        display->setFont(MESSAGE_FONT);
        display->setScrollSpeed(MESSAGE_SPEED);
        display->setScrollIterations(1);
        display->scrollText("Hands removed... restarting...");
        ScrollEvent event;
        while (true) {
            if (xQueueReceive(textScrollEvents, &event, portMAX_DELAY) == pdTRUE) {
                if (event.type == ScrollEventType::SCROLL_END) {
                    break;
                }
            }
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        resetState();
    }

    int calculateAverage(const std::vector<int> &readings) {
        int sum = 0;
        for (int value : readings) {
            sum += value;
        }
        return readings.size() > 0 ? sum / readings.size() : 0;
    }

    double calculateStandardDeviation(const std::vector<int> &readings, int average) {
        double sum = 0;
        for (int value : readings) {
            sum += pow(value - average, 2);
        }
        double variance = readings.size() > 0 ? sum / readings.size() : 0;
        return sqrt(variance);
    }

    void showAuditMessage() {
        if (messages.empty()) {
            ESP_LOGE(TAG_MADHATTERMETER, "No messages loaded");
            display->showTextCentered("No messages available");
            return;
        }
        int index = random(0, messages.size());
        display->setFont(MESSAGE_FONT);
        display->setScrollSpeed(MESSAGE_SPEED);
        display->setScrollIterations(1);
        display->scrollText(messages[index].c_str());
        ScrollEvent event;
        while (true) {
            if (xQueueReceive(textScrollEvents, &event, portMAX_DELAY) == pdTRUE) {
                if (event.type == ScrollEventType::SCROLL_END) {
                    break;
                }
            }
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
};

#endif // MODE_MAD_HATTER_METER_HPP
