#include "./modules/input.h"

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