#include "./input.h"

asw::input::KeyState asw::input::keyboard;

asw::input::MouseState asw::input::mouse;

void asw::input::reset() {
  auto& keyboard = asw::input::keyboard;
  auto& mouse = asw::input::mouse;

  // Clear key state
  keyboard.anyPressed = false;
  keyboard.lastPressed = -1;
  for (unsigned int i = 0; i < keyboard.pressed.size(); i++) {
    keyboard.pressed[i] = false;
  }
  for (unsigned int i = 0; i < keyboard.released.size(); i++) {
    keyboard.released[i] = false;
  }

  // Clear mouse state
  mouse.anyPressed = false;
  mouse.xChange = 0;
  mouse.yChange = 0;
  mouse.z = 0;
  for (unsigned int i = 0; i < mouse.pressed.size(); i++) {
    mouse.pressed[i] = false;
  }
  for (unsigned int i = 0; i < mouse.released.size(); i++) {
    mouse.released[i] = false;
  }
}