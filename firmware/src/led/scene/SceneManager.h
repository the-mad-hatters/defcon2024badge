#ifndef LED_SCENE_MANAGER_H
#define LED_SCENE_MANAGER_H

#include <memory>
#include <mutex>
#include "freertos/FreeRTOS.h"
#include <unordered_map>
#include "freertos/semphr.h"
#include "Scene.h"

// List of LED animation scenes
enum class SceneType {
    NONE,
    MAD_HATTER,
    CELESTIAL_CLOUDS,
    GONE_TO_HELL,
    SECRET_COMBINATIONS,
    THE_SALAMANDER,
    TAPIR_JOYRIDE,
    SUPPRESSIVE_BADGE,
    SEER_STONES,
    THETAN_POSSESSION,
    DRUNK_AS_HELL,
    GONE_CLEAR,
    AMERICAN_JESUS,
    DC32_Y2K_AESTHETIC,
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

    std::unordered_map<SceneType, std::unique_ptr<Scene>> scenes;
    Scene *currentScene;
    std::mutex sceneMutex;
};

#endif // LED_SCENE_MANAGER_H
