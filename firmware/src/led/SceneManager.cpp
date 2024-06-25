#include "SceneManager.h"

static const char *TAG = "SceneManager";

// Include all scenes here
#include "scenes/SceneGoingToHell.hpp"

void SceneManager::init() {
    if (initialized) {
        return;
    }

    LedHandler &leds = LedHandler::getInstance();

    // Create all scenes here
    scenes[SceneType::GOING_TO_HELL] = std::make_unique<GoingToHell>(leds);

    // Create the scene task
    xTaskCreate(sceneTask, "SceneTask", 2048, this, 5, NULL);

    initialized = true;
}

void SceneManager::setScene(SceneType scene) {
    ESP_LOGD(TAG, "Switching to scene %d", static_cast<int>(scene));
    std::lock_guard<std::mutex> lock(sceneMutex);
    currentScene = scenes[scene].get();
}

void SceneManager::sceneTask(void *pvParameters) {
    SceneManager *self = static_cast<SceneManager *>(pvParameters);

    while (true) {
        std::lock_guard<std::mutex> lock(self->sceneMutex);
        if (self->currentScene) {
            self->currentScene->tick();
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}
