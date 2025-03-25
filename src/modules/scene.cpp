/**
 * @file scene.cpp
 * @author
 * @brief Scene implementation for the ASW library
 * @date 2025-03-25
 */

#include "./asw/modules/scene.h"
#include "./asw/modules/core.h"
#include "./asw/modules/display.h"

template <typename T>
void asw::scene::SceneManager<T>::registerScene(const T sceneId,
                                                asw::scene::Scene<T>* scene) {
  scenes[sceneId] = scene;
}

template <typename T>
void asw::scene::SceneManager<T>::setNextScene(const T sceneId) {
  nextScene = sceneId;
  hasNextScene = true;
}

template <typename T>
void asw::scene::SceneManager<T>::update(const float deltaTime) {
  changeScene();

  if (activeScene == nullptr) {
    return;
  }

  activeScene->update(deltaTime);
}

template <typename T>
void asw::scene::SceneManager<T>::draw() {
  if (activeScene == nullptr) {
    return;
  }

  activeScene->draw();
}

template <typename T>
void asw::scene::SceneManager<T>::changeScene() {
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

template <typename T>
void asw::scene::SceneManager<T>::start() {
#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop(loopEmscripten, 0, 1);
#else
  std::chrono::nanoseconds lag(0ns);
  auto time_start = std::chrono::high_resolution_clock::now();
  auto last_second = std::chrono::high_resolution_clock::now();
  int frames = 0;

  while (!asw::core::exit) {
    auto delta_time = std::chrono::high_resolution_clock::now() - time_start;
    time_start = std::chrono::high_resolution_clock::now();
    lag += std::chrono::duration_cast<std::chrono::nanoseconds>(delta_time);

    while (lag >= timestep) {
      asw::core::update();
      update(timestep / 1ms);
      lag -= timestep;
    }

    // Clear screen
    asw::display::clear();
    draw();
    asw::display::present();

    frames++;

    if (std::chrono::high_resolution_clock::now() - last_second >= 1s) {
      fps = frames;
      frames = 0;
      last_second = last_second + 1s;
    }
  }
#endif
}

template <typename T>
void asw::scene::SceneManager<T>::loopEmscripten() {
  update(timestep / 1ms);
  draw();
}

template <typename T>
int asw::scene::SceneManager<T>::getFPS() const {
  return fps;
}