#ifndef LED_SCENE_MANAGER_H
#define LED_SCENE_MANAGER_H

#include <memory>
#include <mutex>
#include "freertos/FreeRTOS.h"
#include <unordered_map>
#include "freertos/semphr.h"
#include "SceneBase.h"

// List of LED animation scenes
enum class SceneType {
    NONE,
    GOING_TO_HELL,
};

// Scene manager to track and switch between LED animation scenes
class SceneManager {
  public:
    static SceneManager &getInstance() {
        static SceneManager instance;
        return instance;
    }
    void init();
    void setScene(SceneType scene);
    static void sceneTask(void *pvParameters);

  private:
    SceneManager() : currentScene(nullptr) {};
    SceneManager(const SceneManager &)            = delete;
    SceneManager &operator=(const SceneManager &) = delete;
    bool initialized                              = false;

    std::unordered_map<SceneType, std::unique_ptr<SceneBase>> scenes;
    SceneBase *currentScene;
    std::mutex sceneMutex;
};

#endif // LED_SCENE_MANAGER_H