#include "./asw/modules/input.h"

asw::input::KeyState asw::input::keyboard;

asw::input::MouseState asw::input::mouse;

void asw::input::reset() {
  auto& k_state = asw::input::keyboard;
  auto& m_state = asw::input::mouse;

  // Clear key state
  k_state.anyPressed = false;
  k_state.lastPressed = -1;

  for (auto& key : k_state.pressed) {
    key = false;
  }

  for (auto& key : k_state.released) {
    key = false;
  }

  // Clear mouse state
  m_state.anyPressed = false;
  m_state.xChange = 0;
  m_state.yChange = 0;
  m_state.z = 0;

  for (auto& button : m_state.pressed) {
    button = false;
  }

  for (auto& button : m_state.released) {
    button = false;
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
  if (activeCursor != nullptr) {
    SDL_SetCursor(nullptr);
  }

  activeCursor = SDL_CreateSystemCursor(static_cast<SDL_SystemCursor>(cursor));
  if (activeCursor == nullptr) {
    return;
  }

  SDL_SetCursor(activeCursor);
}