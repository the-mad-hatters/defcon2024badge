#include "SceneManager.h"
#include "ScenePalettes.h"

static const char *TAG = "SceneManager";

void SceneManager::init() {
    if (initialized) {
        return;
    }

    //
    // Create all scenes here
    //

    // clang-format off
    scenes[SceneType::MAD_HATTER] = std::make_unique<Scene>(
        SceneConfigMapAddressable{
            {AddressableStrip::UPPER, SceneConfigAddressable(mad_hatter_p, 0, UPPER_LED_BASE_BRIGHTNESS, 10, ScenePattern::PALETTE_FILL)},
            {AddressableStrip::LOWER, SceneConfigAddressable(mad_hatter_p, 0, LOWER_LED_BASE_BRIGHTNESS, 10, ScenePattern::PALETTE_FILL)},
            {AddressableStrip::MAIN,  SceneConfigAddressable(drunk_as_hell_p, 0, MAIN_LED_BASE_BRIGHTNESS, 10, ScenePattern::PALETTE_FILL)},
            {AddressableStrip::TOUCH, SceneConfigAddressable(CRGB::Red, 0, TOUCH_LED_BASE_BRIGHTNESS, 100, ScenePattern::COLOR_CYCLE)},
        },
        SceneConfigNonAddr{250, ScenePatternNonAddr::RANDOM}
    );
    scenes[SceneType::CELESTIAL_CLOUDS] = std::make_unique<Scene>(
        SceneConfigMapAddressable{
            {AddressableStrip::UPPER, SceneConfigAddressable(celestial_clouds_p, 0, UPPER_LED_BASE_BRIGHTNESS, 10, ScenePattern::PALETTE_FILL)},
            {AddressableStrip::LOWER, SceneConfigAddressable(celestial_clouds_p, 0, LOWER_LED_BASE_BRIGHTNESS, 10, ScenePattern::PALETTE_FILL)},
            {AddressableStrip::MAIN,  SceneConfigAddressable(celestial_clouds_main_p, 0, MAIN_LED_BASE_BRIGHTNESS, 10, ScenePattern::PALETTE_FILL)},
            {AddressableStrip::TOUCH, SceneConfigAddressable(CRGB::Navy, 0, TOUCH_LED_BASE_BRIGHTNESS, 100, ScenePattern::COLOR_CYCLE)},
        },
        SceneConfigNonAddr{250, ScenePatternNonAddr::RANDOM}
    );
    scenes[SceneType::GONE_TO_HELL] = std::make_unique<Scene>(
        SceneConfigMapAddressable{
            {AddressableStrip::UPPER, SceneConfigAddressable(gone_to_hell_p, 0, UPPER_LED_BASE_BRIGHTNESS, 10, ScenePattern::PALETTE_FILL)},
            {AddressableStrip::LOWER, SceneConfigAddressable(gone_to_hell_p, 0, LOWER_LED_BASE_BRIGHTNESS, 10, ScenePattern::PALETTE_FILL)},
            {AddressableStrip::MAIN,  SceneConfigAddressable(gone_to_hell_main_p, 0, MAIN_LED_BASE_BRIGHTNESS, 10, ScenePattern::PALETTE_FILL)},
            {AddressableStrip::TOUCH, SceneConfigAddressable(CRGB::Red, 0, TOUCH_LED_BASE_BRIGHTNESS, 150, ScenePattern::COLOR_CYCLE)},
        },
        SceneConfigNonAddr{250, ScenePatternNonAddr::RANDOM}
    );
    scenes[SceneType::SECRET_COMBINATIONS] = std::make_unique<Scene>(
        SceneConfigMapAddressable{
            {AddressableStrip::UPPER, SceneConfigAddressable(secret_combinations_p, 0, UPPER_LED_BASE_BRIGHTNESS, 10, ScenePattern::PALETTE_FILL)},
            {AddressableStrip::LOWER, SceneConfigAddressable(secret_combinations_p, 0, LOWER_LED_BASE_BRIGHTNESS, 10, ScenePattern::PALETTE_FILL)},
            {AddressableStrip::MAIN,  SceneConfigAddressable(secret_combinations_p, 0, MAIN_LED_BASE_BRIGHTNESS, 10, ScenePattern::PALETTE_FILL)},
            {AddressableStrip::TOUCH, SceneConfigAddressable(CRGB::Red, 0, TOUCH_LED_BASE_BRIGHTNESS, 100, ScenePattern::COLOR_CYCLE)},
        },
        SceneConfigNonAddr{250, ScenePatternNonAddr::RANDOM}
    );
    scenes[SceneType::THE_SALAMANDER] = std::make_unique<Scene>(
        SceneConfigMapAddressable{
            {AddressableStrip::UPPER, SceneConfigAddressable(the_salamander_p, 0, UPPER_LED_BASE_BRIGHTNESS, 10, ScenePattern::PALETTE_FILL)},
            {AddressableStrip::LOWER, SceneConfigAddressable(the_salamander_p, 0, LOWER_LED_BASE_BRIGHTNESS, 10, ScenePattern::PALETTE_FILL)},
            {AddressableStrip::MAIN,  SceneConfigAddressable(the_salamander_p, 0, MAIN_LED_BASE_BRIGHTNESS, 10, ScenePattern::PALETTE_FILL)},
            {AddressableStrip::TOUCH, SceneConfigAddressable(CRGB::Red, 0, TOUCH_LED_BASE_BRIGHTNESS, 100, ScenePattern::COLOR_CYCLE)},
        },
        SceneConfigNonAddr{250, ScenePatternNonAddr::RANDOM}
    );
    scenes[SceneType::SEER_STONES] = std::make_unique<Scene>(
        SceneConfigMapAddressable{
            {AddressableStrip::UPPER, SceneConfigAddressable(seer_stones_p, 0, UPPER_LED_BASE_BRIGHTNESS, 10, ScenePattern::PALETTE_FILL)},
            {AddressableStrip::LOWER, SceneConfigAddressable(seer_stones_p, 0, LOWER_LED_BASE_BRIGHTNESS, 10, ScenePattern::PALETTE_FILL)},
            {AddressableStrip::MAIN,  SceneConfigAddressable(seer_stones_main_p, 0, MAIN_LED_BASE_BRIGHTNESS, 10, ScenePattern::PALETTE_FILL)},
            {AddressableStrip::TOUCH, SceneConfigAddressable(CRGB::Red, 0, TOUCH_LED_BASE_BRIGHTNESS, 100, ScenePattern::COLOR_CYCLE)},
        },
        SceneConfigNonAddr{250, ScenePatternNonAddr::RANDOM}
    );
    scenes[SceneType::DRUNK_AS_HELL] = std::make_unique<Scene>(
        SceneConfigMapAddressable{
            {AddressableStrip::UPPER, SceneConfigAddressable(drunk_as_hell_p, 0, UPPER_LED_BASE_BRIGHTNESS, 10, ScenePattern::PALETTE_FILL)},
            {AddressableStrip::LOWER, SceneConfigAddressable(drunk_as_hell_p, 0, LOWER_LED_BASE_BRIGHTNESS, 10, ScenePattern::PALETTE_FILL)},
            {AddressableStrip::MAIN,  SceneConfigAddressable(drunk_as_hell_main_p, 0, MAIN_LED_BASE_BRIGHTNESS, 10, ScenePattern::PALETTE_FILL)},
            {AddressableStrip::TOUCH, SceneConfigAddressable(CRGB::Red, 0, TOUCH_LED_BASE_BRIGHTNESS, 100, ScenePattern::COLOR_CYCLE)},
        },
        SceneConfigNonAddr{250, ScenePatternNonAddr::RANDOM}
    );

    scenes[SceneType::AMERICAN_JESUS] = std::make_unique<Scene>(
        SceneConfigMapAddressable{
            {AddressableStrip::UPPER, SceneConfigAddressable(red_white_blue_p, 0, UPPER_LED_BASE_BRIGHTNESS, 10, ScenePattern::PALETTE_FILL)},
            {AddressableStrip::LOWER, SceneConfigAddressable(red_white_blue_p, 0, LOWER_LED_BASE_BRIGHTNESS, 10, ScenePattern::PALETTE_FILL)},
            {AddressableStrip::MAIN,  SceneConfigAddressable(red_white_blue_main_p, 0, MAIN_LED_BASE_BRIGHTNESS, 10, ScenePattern::PALETTE_FILL)},
            {AddressableStrip::TOUCH, SceneConfigAddressable(CRGB::Red, 0, TOUCH_LED_BASE_BRIGHTNESS, 100, ScenePattern::COLOR_CYCLE)},
        },
        SceneConfigNonAddr{250, ScenePatternNonAddr::RANDOM}

    );
    
    scenes[SceneType::DC32_Y2K_AESTHETIC] = std::make_unique<Scene>(
        SceneConfigMapAddressable{
            {AddressableStrip::UPPER, SceneConfigAddressable(dc32_y2k_aesthetic_p, 0, UPPER_LED_BASE_BRIGHTNESS, 20, ScenePattern::PALETTE_FILL)},
            {AddressableStrip::LOWER, SceneConfigAddressable(dc32_y2k_aesthetic_p, 0, LOWER_LED_BASE_BRIGHTNESS, 20, ScenePattern::PALETTE_FILL)},
            {AddressableStrip::MAIN,  SceneConfigAddressable(dc32_y2k_aesthetic_p, 0, MAIN_LED_BASE_BRIGHTNESS, 30, ScenePattern::PALETTE_FILL)},
            {AddressableStrip::TOUCH, SceneConfigAddressable(CRGB::Red, 0, TOUCH_LED_BASE_BRIGHTNESS, 100, ScenePattern::COLOR_CYCLE)},
        },
        SceneConfigNonAddr{250, ScenePatternNonAddr::RANDOM}
    );
    // clang-format on

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
