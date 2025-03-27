/// @file scene.h
/// @author Allan Legemaate (alegemaate@gmail.com)
/// @brief Scene manager
/// @date 2025-03-25
///
/// @copyright Copyright (c) 2025
///

#ifndef ASW_SCENE_H
#define ASW_SCENE_H

#include <chrono>
#include <unordered_map>

#include "./core.h"
#include "./display.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

namespace asw::scene {
  using namespace std::chrono_literals;

  /// @brief The time step for the game loop.
  constexpr auto timestep = 8ms;

  /// @brief Forward declaration of the SceneManager class.
  template <typename T>
  class SceneManager;

  /// @brief Base class for game scenes.
  /// @tparam T The type of the scene ID.
  /// @details This class defines the interface for game scenes in the scene
  /// engine.
  ///
  template <typename T>
  class Scene {
   public:
    /// @brief Constructor for the Scene class.
    /// @param sceneManager Reference to the SceneManager object.
    /// @details This constructor initializes the scene engine reference.
    ///
    explicit Scene(SceneManager<T>& sceneManager)
        : sceneManager(sceneManager) {}

    /// @brief Initialize the game scene.
    ///
    /// @details This function is called when the scene is registered and
    /// initialized.
    ///
    virtual void init() = 0;

    /// @brief Update the game scene.
    ///
    /// @param deltaTime The time since the last update.
    /// @details This function is called every frame to update the scene.
    ///
    virtual void update(float deltaTime) = 0;

    /// @brief Draw the game scene.
    ///
    /// @details This function is called every frame to draw the scene.
    ///
    virtual void draw() = 0;

    /// @brief Handle input for the game scene.
    ///
    /// @details This function is called every frame to handle input for the
    /// scene.
    ///
    virtual void cleanup() = 0;

   protected:
    SceneManager<T>& sceneManager;
  };

  /// @brief SceneManager class for managing game scenes.
  /// @tparam T The type of the scene ID.
  /// @details This class manages the
  /// registration, updating, and drawing of game scenes. It also handles the
  /// transition between scenes.
  ///
  template <typename T>
  class SceneManager {
   public:
    /// @brief Constructor for the SceneManager class.
    ///
    SceneManager() {
#ifdef __EMSCRIPTEN__
      instance = this;
#endif
    }

    /// @brief Register a scene to be managed by the scene engine.
    ///
    /// @param sceneId The unique identifier for the scene.
    /// @param scene Pointer to the Scene object to be registered.
    ///
    void registerScene(const T sceneId, Scene<T>* scene) {
      scenes[sceneId] = scene;
    }

    /// @brief Set the next scene
    ///
    /// @param sceneId The unique identifier for the scene.
    ///
    void setNextScene(const T sceneId) {
      nextScene = sceneId;
      hasNextScene = true;
    }

    /// @brief Main loop for the scene engine. If this is not enough, or you
    /// want to define your own loop, you can use the update and draw functions
    /// directly.
    ///
    /// This function is called to start the main loop of the scene engine.
    ///
    void start() {
#ifdef __EMSCRIPTEN__
      emscripten_set_main_loop(SceneManager::loopEmscripten, 0, 1);
#else

      std::chrono::nanoseconds lag(0ns);
      auto time_start = std::chrono::high_resolution_clock::now();
      auto last_second = std::chrono::high_resolution_clock::now();
      int frames = 0;

      while (!asw::core::exit) {
        auto delta_time =
            std::chrono::high_resolution_clock::now() - time_start;
        time_start = std::chrono::high_resolution_clock::now();
        lag += std::chrono::duration_cast<std::chrono::nanoseconds>(delta_time);

        while (lag >= timestep) {
          update(timestep / 1ms);
          lag -= timestep;
        }

        // Draw
        draw();

        frames++;

        if (std::chrono::high_resolution_clock::now() - last_second >= 1s) {
          fps = frames;
          frames = 0;
          last_second = last_second + 1s;
        }
      }
#endif
    }

    /// @brief Update the current scene.
    ///
    void update(const float deltaTime) {
      asw::core::update();

      changeScene();

      if (activeScene == nullptr) {
        return;
      }

      activeScene->update(deltaTime);
    }

    /// @brief Draw the current scene.
    ///
    void draw() {
      if (activeScene == nullptr) {
        return;
      }

      asw::display::clear();
      activeScene->draw();
      asw::display::present();
    }

    /// @brief Get the current FPS. Only applies to the managed loop.
    ///
    /// @return The current FPS.
    ///
    int getFPS() const { return fps; }

   private:
    /// @brief Change the current scene to the next scene.
    ///
    void changeScene() {
      if (!hasNextScene) {
        return;
      }

      if (activeScene != nullptr) {
        activeScene->cleanup();
      }

      if (scenes.find(nextScene) == scenes.end()) {
        hasNextScene = false;
        return;
      }

      activeScene = scenes[nextScene];
      activeScene->init();
      hasNextScene = false;
    }

    /// @brief The current scene of the scene engine.
    Scene<T>* activeScene{nullptr};

    /// @brief The next scene of the scene engine.
    T nextScene;

    /// @brief Flag to indicate if there is a next scene to change to.
    bool hasNextScene{false};

    /// @brief Collection of all scenes registered in the scene engine.
    std::unordered_map<T, Scene<T>*> scenes;

    /// @breif FPS Counter for managed loop;
    int fps{0};

#ifdef __EMSCRIPTEN__
    /// @brief Pointer to the current instance of the scene manager.
    static SceneManager<T>* instance;

    /// @brief Emscripten loop function.
    static void loopEmscripten() {
      if (instance != nullptr) {
        instance->update(timestep / 1ms);
        instance->draw();
      }
    }
#endif
  };

#ifdef __EMSCRIPTEN__
  template <typename T>
  SceneManager<T>* SceneManager<T>::instance = nullptr;
#endif

}  // namespace asw::scene

#endif  // ASW_SCENE_H