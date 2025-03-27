/// @file input_mouse_buttons.h
/// @author Allan Legemaate (alegemaate@gmail.com)
/// @brief Mappings from SDL mouse buttons to ASW mouse buttons
/// @date 2023-09-20
///
/// @copyright Copyright (c) 2023
///

#ifndef ASW_INPUT_MOUSE_BUTTONS_H
#define ASW_INPUT_MOUSE_BUTTONS_H

#include <SDL3/SDL.h>

namespace asw::input {
  /// @brief Number of mouse buttons in the ASW library.
  constexpr int ASW_NUM_MOUSE_BUTTONS = 5;

  /// @brief Enumeration of mouse buttons in the ASW library.
  enum class MouseButton {
    LEFT = SDL_BUTTON_LEFT,
    MIDDLE = SDL_BUTTON_MIDDLE,
    RIGHT = SDL_BUTTON_RIGHT,
    X1 = SDL_BUTTON_X1,
    X2 = SDL_BUTTON_X2,
    MAX = ASW_NUM_MOUSE_BUTTONS
  };
}  // namespace asw::input

#endif  // ASW_INPUT_MOUSE_BUTTONS_H