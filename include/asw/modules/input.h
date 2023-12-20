/**
 * @file input.h
 * @author Allan Legemaate (alegemaate@gmail.com)
 * @brief Input module for the ASW library
 * @date 2023-09-20
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef ASW_INPUT_H
#define ASW_INPUT_H

#include <SDL2/SDL.h>
#include <array>
#include <string>

#include "./input_game_controller.h"
#include "./input_keys.h"
#include "./input_mouse_buttons.h"

namespace asw::input {

  /**
   * @brief Mouse state stores the current state of the mouse. It is updated by
   * the core.
   */
  using MouseState = struct MouseState {
    bool anyPressed{false};
    int lastPressed{-1};

    int xChange{0};
    int yChange{0};

    int x{0};
    int y{0};
    int z{0};

    std::array<bool, NUM_MOUSE_BUTTONS> pressed{false};
    std::array<bool, NUM_MOUSE_BUTTONS> released{false};
    std::array<bool, NUM_MOUSE_BUTTONS> down{false};
  };

  /**
   * @brief Global mouse state.
   */
  extern MouseState mouse;

  /**
   * @brief Check if a button is down.
   *
   * @param button The button to check.
   * @return true - If the button is down.
   * @return false - If the button is not down.
   */
  bool isButtonDown(asw::input::MouseButton button);

  /**
   * @brief Check if a button was pressed since the last update.
   *
   * @param button The button to check.
   * @return true - If the button was pressed.
   * @return false - If the button was not pressed.
   */
  bool wasButtonPressed(asw::input::MouseButton button);

  /**
   * @brief Check if a button was released since the last update.
   *
   * @param button The button to check.
   * @return true - If the button was released.
   * @return false - If the button was not released.
   */
  bool wasButtonReleased(asw::input::MouseButton button);

  /**
   * @brief Keyboard state stores the current state of the keyboard. It is
   * updated by the core.
   */
  using KeyState = struct KeyState {
    std::array<bool, NUM_KEYS> pressed{false};
    std::array<bool, NUM_KEYS> released{false};
    std::array<bool, NUM_KEYS> down{false};

    bool anyPressed{false};
    int lastPressed{-1};
  };

  /**
   * @brief Global keyboard state.
   */
  extern KeyState keyboard;

  /**
   * @brief Check if a key is down.
   *
   * @param key The key to check.
   * @return true - If the key is down.
   * @return false - If the key is not down.
   */
  bool isKeyDown(asw::input::Key key);

  /**
   * @brief Check if a key was pressed since the last update.
   *
   * @param key The key to check.
   * @return true - If the key was pressed.
   * @return false - If the key was not pressed.
   */
  bool wasKeyPressed(asw::input::Key key);

  /**
   * @brief Check if a key was released since the last update.
   *
   * @param key The key to check.
   * @return true - If the key was released.
   * @return false - If the key was not released.
   */
  bool wasKeyReleased(asw::input::Key key);

  /**
   * @brief Controller state stores the current state of a controller. It is
   * updated by the core.
   */
  using ControllerState = struct ControllerState {
    std::array<bool, NUM_CONTROLLER_BUTTONS> pressed{false};
    std::array<bool, NUM_CONTROLLER_BUTTONS> released{false};
    std::array<bool, NUM_CONTROLLER_BUTTONS> down{false};

    bool anyPressed{false};
    int lastPressed{-1};
    float deadZone{0.25f};

    std::array<float, NUM_CONTROLLER_AXES> axis{0};
  };

  /**
   * @brief Maximum number of controllers supported
   */
  constexpr int MAX_CONTROLLERS = 8;

  /**
   * @brief Global controller state.
   */
  extern std::array<ControllerState, MAX_CONTROLLERS> controller;

  /**
   * @brief Check if a controller button is down.
   *
   * @param index The index of the controller to check.
   * @param button The button to check.
   * @return true - If the button is down.
   * @return false - If the button is not down.
   */
  bool isControllerButtonDown(int index, asw::input::ControllerButton button);

  /**
   * @brief Check if a controller button was pressed since the last update.
   *
   * @param index The index of the controller to check.
   * @param button The button to check.
   * @return true - If the button was pressed.
   * @return false - If the button was not pressed.
   */
  bool wasControllerButtonPressed(int index,
                                  asw::input::ControllerButton button);

  /**
   * @brief Check if a controller button was released since the last update.
   *
   * @param index The index of the controller to check.
   * @param button The button to check.
   * @return true - If the button was released.
   * @return false - If the button was not released.
   */
  bool wasControllerButtonReleased(int index,
                                   asw::input::ControllerButton button);

  /**
   * @brief Get the value of a controller axis.
   *
   * @param index The index of the controller to check.
   * @param axis The axis to check.
   * @return float - The value of the axis between -1.0f and 1.0f.
   */
  float getControllerAxis(int index, asw::input::ControllerAxis axis);

  /**
   * @breif Set the joystick deadzone for a controller.
   */
  void setControllerDeadZone(int index, float deadZone);

  /**
   * @breif Get the number of controllers connected.
   */
  int getControllerCount();

  /**
   * @breif Get the name of a controller.
   */
  std::string getControllerName(int index);

  /**
   * @brief Reset all input states. Called by the core.
   */
  void reset();
}  // namespace asw::input

#endif  // ASW_INPUT_H