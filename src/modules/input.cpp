#include "./asw/modules/input.h"

asw::input::KeyState asw::input::keyboard{};

asw::input::MouseState asw::input::mouse{};

std::array<asw::input::ControllerState, asw::input::MAX_CONTROLLERS>
    asw::input::controller{};

void asw::input::reset() {
  auto& k_state = asw::input::keyboard;
  auto& m_state = asw::input::mouse;
  auto& c_state = asw::input::controller;

  // Clear key state
  k_state.anyPressed = false;
  k_state.lastPressed = -1;

  for (auto& pressed : k_state.pressed) {
    pressed = false;
  }

  for (auto& released : k_state.released) {
    released = false;
  }

  // Clear mouse state
  m_state.anyPressed = false;
  m_state.xChange = 0;
  m_state.yChange = 0;
  m_state.z = 0;

  for (auto& pressed : m_state.pressed) {
    pressed = false;
  }

  for (auto& released : m_state.released) {
    released = false;
  }

  // Clear controller state
  for (auto& cont : c_state) {
    cont.anyPressed = false;
    cont.lastPressed = -1;

    for (auto& button : cont.pressed) {
      button = false;
    }

    for (auto& button : cont.released) {
      button = false;
    }

    for (auto& axis : cont.axis) {
      axis = 0;
    }
  }
}

bool asw::input::isButtonDown(asw::input::MouseButton button) {
  return mouse.down[static_cast<int>(button)];
}

bool asw::input::wasButtonPressed(asw::input::MouseButton button) {
  return mouse.pressed[static_cast<int>(button)];
}

bool asw::input::wasButtonReleased(asw::input::MouseButton button) {
  return mouse.released[static_cast<int>(button)];
}

bool asw::input::isKeyDown(asw::input::Key key) {
  return keyboard.down[static_cast<int>(key)];
}

bool asw::input::wasKeyPressed(asw::input::Key key) {
  return keyboard.pressed[static_cast<int>(key)];
}

bool asw::input::wasKeyReleased(asw::input::Key key) {
  return keyboard.released[static_cast<int>(key)];
}

void asw::input::setCursor(asw::input::CursorId cursor) {
  auto cursor_int = static_cast<unsigned int>(cursor);

  if (cursor_int < 0 || cursor_int >= cursors.size()) {
    return;
  }

  if (cursors[cursor_int] == nullptr) {
    cursors[cursor_int] =
        SDL_CreateSystemCursor(static_cast<SDL_SystemCursor>(cursor_int));
  }

  SDL_SetCursor(cursors[cursor_int]);
}

bool asw::input::isControllerButtonDown(int index,
                                        asw::input::ControllerButton button) {
  return controller[index].down[static_cast<int>(button)];
}

bool asw::input::wasControllerButtonPressed(
    int index,
    asw::input::ControllerButton button) {
  return controller[index].pressed[static_cast<int>(button)];
}

bool asw::input::wasControllerButtonReleased(
    int index,
    asw::input::ControllerButton button) {
  return controller[index].released[static_cast<int>(button)];
}

float asw::input::getControllerAxis(int index,
                                    asw::input::ControllerAxis axis) {
  return controller[index].axis[static_cast<int>(axis)];
}

void asw::input::setControllerDeadZone(int index, float deadZone) {
  controller[index].deadZone = deadZone;
}

int asw::input::getControllerCount() {
  int* count = nullptr;
  SDL_GetJoysticks(count);
  if (count == nullptr) {
    return 0;
  }

  return *count;
}

std::string asw::input::getControllerName(int index) {
  return SDL_GetJoystickNameForID(index);
}