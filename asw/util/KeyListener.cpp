#include "KeyListener.h"

#include "../asw.h"

bool KeyListener::keyPressed[KL_KEY_MAX] = {false};
bool KeyListener::keyReleased[KL_KEY_MAX] = {false};
bool KeyListener::keyDown[KL_KEY_MAX] = {false};
bool KeyListener::lastTicksKey[KL_KEY_MAX] = {false};
bool KeyListener::anyKeyPressed = false;
int KeyListener::lastKeyPressed = -1;

// Check those keys!
void KeyListener::update() {
  SDL_PumpEvents();
  const Uint8* key = SDL_GetKeyboardState(NULL);

  lastKeyPressed = -1;
  anyKeyPressed = false;

  // Check key just pressed
  for (int i = 0; i < KL_KEY_MAX; i++) {
    // Clear old values
    keyPressed[i] = false;
    keyReleased[i] = false;
    keyDown[i] = false;

    // Is down
    if (key[i]) {
      keyDown[i] = true;
      anyKeyPressed = true;
    }

    // Pressed since last tick?
    if (key[i] && !lastTicksKey[i]) {
      keyPressed[i] = true;
      lastKeyPressed = i;
    }

    // Released since last tick?
    if (!key[i] && lastTicksKey[i]) {
      keyReleased[i] = true;
    }
  }

  // Get new values
  for (int i = 0; i < KL_KEY_MAX; i++) {
    if (lastTicksKey[i] != (bool)key[i]) {
      lastTicksKey[i] = key[i];
    }
  }
}
