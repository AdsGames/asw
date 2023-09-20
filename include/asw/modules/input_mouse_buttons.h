#ifndef ASW_INPUT_MOUSE_BUTTONS_H
#define ASW_INPUT_MOUSE_BUTTONS_H

#include <SDL2/SDL.h>

namespace asw::input {
  constexpr int ASW_NUM_MOUSE_BUTTONS = 5;

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