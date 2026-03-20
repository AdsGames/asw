/// @file scene.h
/// @author Allan Legemaate (alegemaate@gmail.com)
/// @brief Scene manager
/// @date 2025-03-25
///
/// @copyright Copyright (c) 2025
///

#ifndef ASW_SCENE_H
#define ASW_SCENE_H

#include <algorithm>
#include <chrono>
#include <iostream>
#include <memory>
#include <ranges>
#include <unordered_map>
#include <vector>

#include "./core.h"
#include "./display.h"
#include "./game.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

namespace asw::scene {

/// @brief Default time step for the game loop.
constexpr auto DEFAULT_TIMESTEP = std::chrono::milliseconds(8);

/// @brief Forward declaration of the SceneManager class.
template <typename T> class SceneManager;

/// @brief Base class for game scenes.
/// @tparam T The type of the scene ID.
/// @details This class defines the interface for game scenes in the scene
/// engine.
///
template <typename T> class Scene {
public:
    /// @brief Constructor for the Scene class.
    /// @param manager Reference to the SceneManager object.
    /// @details This constructor initializes the scene engine reference.
    ///
    explicit Scene(SceneManager<T>& manager)
        : manager(manager)
    {
    }

    /// @brief Destructor for the Scene class.
    virtual ~Scene() = default;

    /// @brief Initialize the game scene.
    ///
    /// @details This function is called when the scene is registered and
    /// initialized.
    ///
    virtual void init() {
        // Default implementation does nothing
    };

    /// @brief Update the game scene.
    ///
    /// @param dt The time in seconds since the last update.
    /// @details This function is called every frame to update the scene.
    ///
    virtual void update(float dt)
    {
        // Erase inactive objects
        std::erase_if(_objects, [](const auto& obj) { return !obj->alive; });

        // Update all objects in the scene
        for (auto const& obj : _objects) {
            if (obj->active && obj->alive) {
                obj->update(dt);
            }
        }

        // Create new objects
        for (auto const& obj : _obj_to_create) {
            _objects.push_back(obj);
        }

        // Clear the objects to create
        _obj_to_create.clear();
    };

    /// @brief Draw the game scene.
    ///
    /// @details This function is called every frame to draw the scene.
    ///
    virtual void draw()
    {
        // Sort objects by z-index
        std::ranges::sort(_objects, std::less {}, &game::GameObject::z_index);

        for (auto const& obj : _objects) {
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
    virtual void cleanup()
    {
        _objects.clear();
    };

    /// @brief Add a game object to the scene.
    ///
    /// @param gameObject The game object to add to the scene.
    ///
    void register_object(const std::shared_ptr<game::GameObject>& obj)
    {
        _objects.push_back(obj);
    }

    /// @brief Create a new game object in the scene.
    ///
    /// @param gameObject The game object to add to the scene.
    ///
    template <typename ObjectType, typename... Args>
    std::shared_ptr<ObjectType> create_object(Args&&... args)
    {
        static_assert(std::is_base_of_v<game::GameObject, ObjectType>,
            "ObjectType must be derived from Scene<T>");
        static_assert(std::is_constructible_v<ObjectType, Args...>,
            "ObjectType must be constructible with the given arguments");

        auto obj = std::make_shared<ObjectType>(std::forward<Args>(args)...);
        _obj_to_create.emplace_back(obj);
        return obj;
    }

    /// @brief Get game objects in the scene.
    ///
    /// @return A vector of shared pointers to game objects in the scene.
    ///
    const std::vector<std::shared_ptr<game::GameObject>>& get_objects() const
    {
        return _objects;
    }

    /// @brief Get game objects of a specific type in the scene.
    ///
    /// @tparam ObjectType The type of the game object to get.
    /// @return A vector of shared pointers to game objects of the specified
    /// type in the scene.
    ///
    template <typename ObjectType> std::vector<std::shared_ptr<ObjectType>> get_object_view()
    {
        static_assert(std::is_base_of_v<game::GameObject, ObjectType>,
            "ObjectType must be derived from Scene<T>");

        std::vector<std::shared_ptr<ObjectType>> result;
        for (const auto& obj : _objects) {
            if (auto casted_obj = std::dynamic_pointer_cast<ObjectType>(obj)) {
                result.push_back(casted_obj);
            }
        }
        return result;
    }

protected:
    /// @brief Reference to the scene manager.
    SceneManager<T>& manager;

private:
    /// @brief Collection of game objects in the scene.
    std::vector<std::shared_ptr<game::GameObject>> _objects;

    /// @brief Objects to be created in the next frame.
    std::vector<std::shared_ptr<game::GameObject>> _obj_to_create;
};

/// @brief SceneManager class for managing game scenes.
/// @tparam T The type of the scene ID.
/// @details This class manages the
/// registration, updating, and drawing of game scenes. It also handles the
/// transition between scenes.
///
template <typename T> class SceneManager {
public:
    /// @brief Constructor for the SceneManager class.
    ///
    SceneManager()
    {
#ifdef __EMSCRIPTEN__
        instance_ = this;
        em_time_ = std::chrono::high_resolution_clock::now();
#endif
    }

    /// @brief Register a scene to be managed by the scene engine.
    ///
    /// @param scene_id The unique identifier for the scene.
    /// @param scene Pointer to the Scene object to be registered.
    ///
    template <typename SceneType, typename... Args>
    void register_scene(const T scene_id, Args&&... args)
    {
        static_assert(
            std::is_base_of_v<Scene<T>, SceneType>, "SceneType must be derived from Scene<T>");
        static_assert(std::is_constructible_v<SceneType, Args...>,
            "SceneType must be constructible with the given arguments");

        auto scene = std::make_shared<SceneType>(std::forward<Args>(args)...);
        _scenes[scene_id] = scene;
    }

    /// @brief Set the next scene
    ///
    /// @param scene_id The unique identifier for the scene.
    ///
    void set_next_scene(const T scene_id)
    {
        _next_scene = scene_id;
        _has_next_scene = true;
    }

    /// @brief Main loop for the scene engine. If this is not enough, or you
    /// want to define your own loop, you can use the update and draw functions
    /// directly.
    ///
    /// This function is called to start the main loop of the scene engine.
    ///
    void start()
    {
#ifdef __EMSCRIPTEN__
        emscripten_set_main_loop(SceneManager::loop_emscripten, 0, 1);
#else

        using namespace std::chrono_literals;
        std::chrono::nanoseconds lag(0ns);
        auto time_start = std::chrono::high_resolution_clock::now();
        auto last_second = std::chrono::high_resolution_clock::now();
        int frames = 0;

        while (!asw::core::is_exiting()) {
            auto delta_time = std::chrono::high_resolution_clock::now() - time_start;
            time_start = std::chrono::high_resolution_clock::now();
            lag += std::chrono::duration_cast<std::chrono::nanoseconds>(delta_time);

            while (lag >= this->_timestep) {
                update(std::chrono::duration<float>(this->_timestep).count());
                lag -= this->_timestep;
            }

            // Draw
            draw();

            frames++;

            if (std::chrono::high_resolution_clock::now() - last_second >= 1s) {
                _fps = frames;
                frames = 0;
                last_second = last_second + 1s;
            }
        }

        // Cleanup
        cleanup();
#endif
    }

    /// @brief Destroy the scene manager and clean up resources.
    /// This function is called when the scene manager is destroyed to clean up any resources used
    /// by the scene engine.
    ///
    void cleanup()
    {
        if (_active_scene != nullptr) {
            _active_scene->cleanup();
        }

        _scenes.clear();
    }

    /// @brief Update the current scene.
    ///
    /// @param dt The time in seconds since the last update.
    ///
    void update(const float dt)
    {
        if (asw::core::is_exiting()) {
            return;
        }

        asw::core::update();
        change_scene();

        if (_active_scene != nullptr) {
            _active_scene->update(dt);
        }
    }

    /// @brief Draw the current scene.
    ///
    void draw()
    {
        if (asw::core::is_exiting()) {
            return;
        }

        if (_active_scene != nullptr) {
            asw::display::clear();
            _active_scene->draw();
            asw::display::present();
        }
    }

    /// @brief Set the fixed timestep for the game loop.
    ///
    /// @param ts Timestep duration (default: 8ms).
    ///
    void set_timestep(std::chrono::nanoseconds ts)
    {
        _timestep = ts;
    }

    /// @brief Get the current timestep.
    ///
    /// @return The current timestep duration.
    ///
    std::chrono::nanoseconds get_timestep() const
    {
        return _timestep;
    }

    /// @brief Get the current FPS. Only applies to the managed loop.
    ///
    /// @return The current FPS.
    ///
    int get_fps() const
    {
        return _fps;
    }

private:
    /// @brief Change the current scene to the next scene.
    ///
    void change_scene()
    {
        if (!_has_next_scene) {
            return;
        }

        if (_active_scene != nullptr) {
            _active_scene->cleanup();
        }

        if (auto it = _scenes.find(_next_scene); it != _scenes.end()) {
            _active_scene = it->second;
            _active_scene->init();
        }

        _has_next_scene = false;
    }

    /// @brief The current scene of the scene engine.
    std::shared_ptr<Scene<T>> _active_scene { nullptr };

    /// @brief The next scene of the scene engine.
    T _next_scene;

    /// @brief Flag to indicate if there is a next scene to change to.
    bool _has_next_scene { false };

    /// @brief Collection of all scenes registered in the scene engine.
    std::unordered_map<T, std::shared_ptr<Scene<T>>> _scenes;

    /// @brief Fixed timestep for the game loop.
    std::chrono::nanoseconds _timestep { DEFAULT_TIMESTEP };

    /// @brief FPS Counter for managed loop.
    int _fps { 0 };

#ifdef __EMSCRIPTEN__
    /// @brief Pointer to the current instance of the scene manager.
    static SceneManager<T>* instance_;

    /// @brief The time of the last frame.
    static std::chrono::high_resolution_clock::time_point em_time_;

    /// @brief Emscripten loop function.
    static void loop_emscripten()
    {
        if (instance_ != nullptr) {
            auto delta_time = std::chrono::high_resolution_clock::now() - SceneManager::em_time_;
            SceneManager::em_time_ = std::chrono::high_resolution_clock::now();

            instance_->update(std::chrono::duration<float>(delta_time).count());
            instance_->draw();
        }
    }
#endif
};

#ifdef __EMSCRIPTEN__
template <typename T> SceneManager<T>* SceneManager<T>::instance_ = nullptr;

// Start time
template <typename T> auto SceneManager<T>::em_time_ = std::chrono::high_resolution_clock::now();

#endif

} // namespace asw::scene

#endif // ASW_SCENE_H