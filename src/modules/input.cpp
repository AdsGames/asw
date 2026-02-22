#include "./asw/modules/input.h"

namespace {
  /// @brief Active cursor stores the current active cursor. It is updated by
  /// the core.
  std::array<SDL_Cursor*, asw::input::NUM_CURSORS> cursors{nullptr};
}  // namespace

asw::input::KeyState asw::input::keyboard{};

asw::input::MouseState asw::input::mouse{};

std::array<asw::input::ControllerState, asw::input::MAX_CONTROLLERS>
    asw::input::controller{};

void asw::input::reset() {
  auto& k_state = asw::input::keyboard;
  auto& m_state = asw::input::mouse;
  auto& c_state = asw::input::controller;

  // Clear key state
  k_state.any_pressed = false;
  k_state.last_pressed = -1;

  for (auto& pressed : k_state.pressed) {
    pressed = false;
  }

  for (auto& released : k_state.released) {
    released = false;
  }

  // Clear mouse state
  m_state.any_pressed = false;
  m_state.change = {0.0F, 0.0F};
  m_state.z = 0;

  for (auto& pressed : m_state.pressed) {
    pressed = false;
  }

  for (auto& released : m_state.released) {
    released = false;
  }

  // Clear controller state
  for (auto& cont : c_state) {
    cont.any_pressed = false;
    cont.last_pressed = -1;

    for (auto& button : cont.pressed) {
      button = false;
    }

    for (auto& button : cont.released) {
      button = false;
    }
  }
}

bool asw::input::get_mouse_button(asw::input::MouseButton button) {
  return mouse.down[static_cast<int>(button)];
}

bool asw::input::get_mouse_button_down(asw::input::MouseButton button) {
  return mouse.pressed[static_cast<int>(button)];
}

bool asw::input::get_mouse_button_up(asw::input::MouseButton button) {
  return mouse.released[static_cast<int>(button)];
}

bool asw::input::get_key(asw::input::Key key) {
  return keyboard.down[static_cast<int>(key)];
}

bool asw::input::get_key_down(asw::input::Key key) {
  return keyboard.pressed[static_cast<int>(key)];
}

bool asw::input::get_key_up(asw::input::Key key) {
  return keyboard.released[static_cast<int>(key)];
}

void asw::input::set_cursor(asw::input::CursorId cursor) {
  auto cursor_int = static_cast<uint32_t>(cursor);

  if (cursor_int >= cursors.size()) {
    return;
  }

  if (cursors[cursor_int] == nullptr) {
    cursors[cursor_int] =
        SDL_CreateSystemCursor(static_cast<SDL_SystemCursor>(cursor_int));
  }

  SDL_SetCursor(cursors[cursor_int]);
}

bool asw::input::get_controller_button(uint32_t index,
                                       asw::input::ControllerButton button) {
  if (index >= controller.size()) {
    return false;
  }

  return controller[index].down[static_cast<int>(button)];
}

bool asw::input::get_controller_button_down(
    uint32_t index,
    asw::input::ControllerButton button) {
  if (index >= controller.size()) {
    return false;
  }

  return controller[index].pressed[static_cast<int>(button)];
}

bool asw::input::get_controller_button_up(uint32_t index,
                                          asw::input::ControllerButton button) {
  if (index >= controller.size()) {
    return false;
  }

  return controller[index].released[static_cast<int>(button)];
}

float asw::input::get_controller_axis(uint32_t index,
                                      asw::input::ControllerAxis axis) {
  if (index >= controller.size()) {
    return 0.0F;
  }

  return controller[index].axis[static_cast<int>(axis)];
}

void asw::input::set_controller_dead_zone(uint32_t index, float dead_zone) {
  if (index >= controller.size()) {
    return;
  }

  controller[index].dead_zone = dead_zone;
}

int asw::input::get_controller_count() {
  int count = 0;
  SDL_GetJoysticks(&count);
  return count;
}

std::string asw::input::get_controller_name(uint32_t index) {
  if (index >= controller.size()) {
    return "";
  }

  return SDL_GetGamepadNameForID(index);
}