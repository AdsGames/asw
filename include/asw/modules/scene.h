#ifndef ASW_SCENE_H
#define ASW_SCENE_H

#include <chrono>
#include <map>
#include <memory>
#include <string>

namespace asw::scene {
  using namespace std::chrono_literals;

  /// @brief The time step for the game loop.
  constexpr auto timestep = 8ms;

  /// @brief Forward declaration of the SceneManager class.
  template <typename T>
  class SceneManager;

  /**
   * @brief Base class for game scenes.
   * @tparam T The type of the scene ID.
   * @details This class defines the interface for game scenes in the scene
   * engine.
   */
  template <typename T>
  class Scene {
   public:
    /**
     * @brief Constructor for the Scene class.
     * @param sceneManager Reference to the SceneManager object.
     * @details This constructor initializes the scene engine reference.
     *
     */
    explicit Scene(SceneManager<T>& sceneManager)
        : sceneManager(sceneManager) {}

    /**
     * @brief Destructor for the Scene class.
     *
     */
    virtual ~Scene() = default;

    /**
     * @brief Initialize the game scene.
     *
     * @details This function is called when the scene is registered and
     * initialized.
     */
    virtual void init() = 0;

    /**
     * @brief Update the game scene.
     *
     * @param deltaTime The time since the last update.
     * @details This function is called every frame to update the scene.
     */
    virtual void update(float deltaTime) = 0;

    /**
     * @brief Draw the game scene.
     *
     * @details This function is called every frame to draw the scene.
     */
    virtual void draw() = 0;

    /**
     * @brief Handle input for the game scene.
     *
     * @details This function is called every frame to handle input for the
     * scene.
     */
    virtual void cleanup() = 0;

   protected:
    SceneManager<T>& sceneManager;
  };

  /**
   * @brief SceneManager class for managing game scenes.
   * @tparam T The type of the scene ID.
   * @details This class manages the
   * registration, updating, and drawing of game scenes. It also handles the
   * transition between scenes.
   */
  template <typename T>
  class SceneManager {
   public:
    /**
     * @brief Constructor for the SceneManager class.
     */
    SceneManager() = default;

    /**
     * @brief Register a scene to be managed by the scene engine.
     *
     * @param sceneId The unique identifier for the scene.
     * @param scene Pointer to the Scene object to be registered.
     */
    void registerScene(const T sceneId, Scene<T>* scene);

    /**
     * @brief Set the next scene
     *
     * @param sceneId The unique identifier for the scene.
     */
    void setNextScene(const T sceneId);

    /**
     * @brief Main loop for the scene engine. If this is not enough, or you want
     * to define your own loop, you can use the update and draw functions
     * directly.
     *
     * This function is called to start the main loop of the scene engine.
     */
    void start();

    /**
     * @brief Update the current scene.
     *
     */
    void update(const float deltaTime);

    /**
     * @brief Draw the current scene.
     *
     */
    void draw();

   private:
    /**
     * @brief Emscripten loop function.
     *
     */
    void loopEmscripten();

    /**
     * @brief Change the current scene to the next scene.
     *
     */
    void changeScene();

    /// @brief The current scene of the scene engine.
    Scene<T>* activeScene{nullptr};

    /// @brief The next scene of the scene engine.
    T nextScene;

    /// @brief Flag to indicate if there is a next scene to change to.
    bool hasNextScene{false};

    /// @brief Collection of all scenes registered in the scene engine.
    std::unordered_map<T, Scene<T>*> scenes;
  };

}  // namespace asw::scene

#endif  // ASW_SCENE_H