#ifndef ASW_INPUT_H
#define ASW_INPUT_H

#include <SDL2/SDL.h>
#include <array>

#include "./input_keys.h"
#include "./input_mouse_buttons.h"

namespace asw::input {

  using MouseState = struct MouseState {
    bool isButtonDown(asw::input::MouseButton button) {
      return down[static_cast<int>(button)];
    }

    bool wasButtonPressed(asw::input::MouseButton button) {
      return pressed[static_cast<int>(button)];
    }

    bool wasButtonReleased(asw::input::MouseButton button) {
      return released[static_cast<int>(button)];
    }

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

  extern MouseState mouse;

  using KeyState = struct KeyState {
    bool isKeyDown(asw::input::Key key) { return down[static_cast<int>(key)]; }

    bool wasKeyPressed(asw::input::Key key) {
      return pressed[static_cast<int>(key)];
    }

    bool wasKeyReleased(asw::input::Key key) {
      return released[static_cast<int>(key)];
    }

    std::array<bool, ASW_NUM_KEYS> pressed{false};
    std::array<bool, ASW_NUM_KEYS> released{false};
    std::array<bool, ASW_NUM_KEYS> down{false};

    bool anyPressed{false};
    int lastPressed{-1};
  };

  extern KeyState keyboard;

  void reset();
}  // namespace asw::input

#endif  // ASW_INPUT_H