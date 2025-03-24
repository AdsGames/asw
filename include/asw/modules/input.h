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

#include <SDL3/SDL.h>
#include <array>

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

    std::array<bool, ASW_NUM_MOUSE_BUTTONS> pressed{false};
    std::array<bool, ASW_NUM_MOUSE_BUTTONS> released{false};
    std::array<bool, ASW_NUM_MOUSE_BUTTONS> down{false};
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
    std::array<bool, ASW_NUM_KEYS> pressed{false};
    std::array<bool, ASW_NUM_KEYS> released{false};
    std::array<bool, ASW_NUM_KEYS> down{false};

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

  void reset();
}  // namespace asw::input

#endif  // ASW_INPUT_H