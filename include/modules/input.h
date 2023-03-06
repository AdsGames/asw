#ifndef ASW_KEY_STATE_H
#define ASW_KEY_STATE_H

#include <SDL2/SDL.h>
#include <array>

#define ASW_NUM_KEYS SDL_NUM_SCANCODES
#define ASW_NUM_MOUSE_BUTTONS 5

namespace asw::input {
  typedef struct MouseState {
    std::array<bool, ASW_NUM_MOUSE_BUTTONS> pressed{false};
    std::array<bool, ASW_NUM_MOUSE_BUTTONS> released{false};
    std::array<bool, ASW_NUM_MOUSE_BUTTONS> down{false};

    bool anyPressed{false};
    int lastPressed{-1};

    int xChange{0};
    int yChange{0};

    int x{0};
    int y{0};
    int z{0};
  } MouseState;

  extern MouseState mouse;

  typedef struct KeyState {
    std::array<bool, ASW_NUM_KEYS> pressed{false};
    std::array<bool, ASW_NUM_KEYS> released{false};
    std::array<bool, ASW_NUM_KEYS> down{false};

    bool anyPressed{false};
    int lastPressed{-1};
  } KeyState;

  extern KeyState keyboard;

  void reset();
}  // namespace asw::input

#endif  // ASW_KEY_STATE_H