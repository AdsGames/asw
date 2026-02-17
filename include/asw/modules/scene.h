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
#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

#include "./core.h"
#include "./display.h"
#include "./game.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

namespace asw::scene {
  using namespace std::chrono_literals;

  /// @brief Default time step for the game loop.
  constexpr auto DEFAULT_TIMESTEP = 8ms;

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

    /// @brief Destructor for the Scene class.
    virtual ~Scene() = default;

    /// @brief Initialize the game scene.
    ///
    /// @details This function is called when the scene is registered and
    /// initialized.
    ///
    virtual void init() {};

    /// @brief Update the game scene.
    ///
    /// @param deltaTime The time in seconds since the last update.
    /// @details This function is called every frame to update the scene.
    ///
    virtual void update(float deltaTime) {
      // Erase inactive objects
      objects.erase(
          std::remove_if(objects.begin(), objects.end(),
                         [](const std::shared_ptr<game::GameObject>& obj) {
                           return !obj->alive;
                         }),
          objects.end());

      // Update all objects in the scene
      for (auto& obj : objects) {
        if (obj->active && obj->alive) {
          obj->update(deltaTime);
        }
      }

      // Create new objects
      for (auto& obj : objectsToCreate) {
        objects.push_back(obj);
      }

      // Clear the objects to create
      objectsToCreate.clear();
    };

    /// @brief Draw the game scene.
    ///
    /// @details This function is called every frame to draw the scene.
    ///
    virtual void draw() {
      // Sort objects by z-index
      std::sort(objects.begin(), objects.end(),
                [](const std::shared_ptr<game::GameObject>& a,
                   const std::shared_ptr<game::GameObject>& b) {
                  return a->zIndex < b->zIndex;
                });

      for (auto& obj : objects) {
        if (obj->active) {
          obj->draw();
        }
      }
    };

    /// @brief Handle input for the game scene.
    ///
    /// @details This function is called every frame to handle input for the
    /// scene.
    ///
    virtual void cleanup() { objects.clear(); };

    /// @brief Add a game object to the scene.
    ///
    /// @param gameObject The game object to add to the scene.
    ///
    void registerObject(const std::shared_ptr<game::GameObject> obj) {
      objects.push_back(obj);
    }

    /// @brief Create a new game object in the scene.
    ///
    /// @param gameObject The game object to add to the scene.
    ///
    template <typename ObjectType, typename... Args>
    std::shared_ptr<ObjectType> createObject(Args&&... args) {
      static_assert(std::is_base_of_v<game::GameObject, ObjectType>,
                    "ObjectType must be derived from Scene<T>");
      static_assert(
          std::is_constructible_v<ObjectType, Args...>,
          "ObjectType must be constructible with the given arguments");

      auto obj = std::make_shared<ObjectType>(std::forward<Args>(args)...);
      objectsToCreate.emplace_back(obj);
      return obj;
    }

    /// @brief Get game objects in the scene.
    ///
    /// @return A vector of shared pointers to game objects in the scene.
    ///
    std::vector<std::shared_ptr<game::GameObject>> getObjects() {
      return objects;
    }

    /// @brief Get game objects of a specific type in the scene.
    ///
    /// @tparam ObjectType The type of the game object to get.
    /// @return A vector of shared pointers to game objects of the specified
    /// type in the scene.
    ///
    template <typename ObjectType>
    std::vector<std::shared_ptr<ObjectType>> getObjectView() {
      static_assert(std::is_base_of_v<game::GameObject, ObjectType>,
                    "ObjectType must be derived from Scene<T>");

      std::vector<std::shared_ptr<ObjectType>> result;
      for (const auto& obj : objects) {
        if (auto castedObj = std::dynamic_pointer_cast<ObjectType>(obj)) {
          result.push_back(castedObj);
        }
      }
      return result;
    }

   protected:
    /// @brief Reference to the scene manager.
    SceneManager<T>& sceneManager;

   private:
    /// @brief Collection of game objects in the scene.
    std::vector<std::shared_ptr<game::GameObject>> objects;

    /// @brief Objects to be created in the next frame.
    std::vector<std::shared_ptr<game::GameObject>> objectsToCreate;
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
      em_time = std::chrono::high_resolution_clock::now();
#endif
    }

    /// @brief Register a scene to be managed by the scene engine.
    ///
    /// @param sceneId The unique identifier for the scene.
    /// @param scene Pointer to the Scene object to be registered.
    ///
    template <typename SceneType, typename... Args>
    void registerScene(const T sceneId, Args&&... args) {
      static_assert(std::is_base_of_v<Scene<T>, SceneType>,
                    "SceneType must be derived from Scene<T>");
      static_assert(std::is_constructible_v<SceneType, Args...>,
                    "SceneType must be constructible with the given arguments");

      auto scene = std::make_shared<SceneType>(std::forward<Args>(args)...);
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

        while (lag >= this->timestep) {
          update(this->timestep / 1s);
          lag -= this->timestep;
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

      if (activeScene != nullptr) {
        activeScene->update(deltaTime);
      }
    }

    /// @brief Draw the current scene.
    ///
    void draw() {
      if (activeScene != nullptr) {
        asw::display::clear();
        activeScene->draw();
        asw::display::present();
      }
    }

    /// @brief Set the fixed timestep for the game loop.
    ///
    /// @param ts Timestep duration (default: 8ms).
    ///
    void setTimestep(std::chrono::nanoseconds ts) { timestep = ts; }

    /// @brief Get the current timestep.
    ///
    /// @return The current timestep duration.
    ///
    std::chrono::nanoseconds getTimestep() const { return timestep; }

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

      if (auto it = scenes.find(nextScene); it != scenes.end()) {
        activeScene = it->second;
        activeScene->init();
      }

      hasNextScene = false;
    }

    /// @brief The current scene of the scene engine.
    std::shared_ptr<Scene<T>> activeScene;

    /// @brief The next scene of the scene engine.
    T nextScene;

    /// @brief Flag to indicate if there is a next scene to change to.
    bool hasNextScene{false};

    /// @brief Collection of all scenes registered in the scene engine.
    std::unordered_map<T, std::shared_ptr<Scene<T>>> scenes;

    /// @brief Fixed timestep for the game loop.
    std::chrono::nanoseconds timestep{DEFAULT_TIMESTEP};

    /// @brief FPS Counter for managed loop.
    int fps{0};

#ifdef __EMSCRIPTEN__
    /// @brief Pointer to the current instance of the scene manager.
    static SceneManager<T>* instance;

    /// @brief The time of the last frame.
    static std::chrono::high_resolution_clock::time_point em_time;

    /// @brief Emscripten loop function.
    static void loopEmscripten() {
      if (instance != nullptr) {
        auto delta_time =
            std::chrono::high_resolution_clock::now() - SceneManager::em_time;
        SceneManager::em_time = std::chrono::high_resolution_clock::now();

        instance->update(delta_time / 1ms);
        instance->draw();
      }
    }
#endif
  };

#ifdef __EMSCRIPTEN__
  template <typename T>
  SceneManager<T>* SceneManager<T>::instance = nullptr;

  // Start time
  template <typename T>
  auto SceneManager<T>::em_time = std::chrono::high_resolution_clock::now();

#endif

}  // namespace asw::scene

#endif  // ASW_SCENE_H