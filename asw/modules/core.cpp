#include "./core.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>

#include "./display.h"
#include "./input.h"
#include "./util.h"

bool asw::core::exit = false;

void asw::core::update() {
  asw::input::reset();
  auto& mouse = asw::input::mouse;
  auto& keyboard = asw::input::keyboard;

  SDL_Event e;

  while (SDL_PollEvent(&e)) {
    switch (e.type) {
      case SDL_WINDOWEVENT:
        switch (e.window.event) {
          case SDL_WINDOWEVENT_RESIZED: {
            // Maintain aspect ratio
            SDL_FPoint scale;
            SDL_RenderGetScale(asw::display::renderer, &scale.x, &scale.y);

            SDL_Point size;
            SDL_RenderGetLogicalSize(asw::display::renderer, &size.x, &size.y);

            SDL_SetWindowSize(asw::display::window, size.x * scale.x,
                              size.y * scale.y);
            break;
          }
        }
        break;

      case SDL_KEYDOWN:
        if (e.key.repeat == 0) {
          keyboard.pressed[e.key.keysym.scancode] = true;
          keyboard.down[e.key.keysym.scancode] = true;
          keyboard.anyPressed = true;
          keyboard.lastPressed = e.key.keysym.scancode;
        }
        break;

      case SDL_KEYUP:
        if (e.key.repeat == 0) {
          keyboard.released[e.key.keysym.scancode] = true;
          keyboard.down[e.key.keysym.scancode] = false;
        }
        break;

      case SDL_MOUSEBUTTONDOWN: {
        auto button = static_cast<int>(e.button.button);
        mouse.pressed[button] = true;
        mouse.down[button] = true;
        mouse.anyPressed = true;
        mouse.lastPressed = button;
        break;
      }

      case SDL_MOUSEBUTTONUP: {
        auto button = static_cast<int>(e.button.button);
        mouse.released[button] = true;
        mouse.down[button] = false;
        break;
      }

      case SDL_MOUSEMOTION:
        mouse.x = e.motion.x;
        mouse.y = e.motion.y;
        break;

      case SDL_QUIT:
        exit = true;
        break;

      default:
        break;
    }
  }
}

void asw::core::init(int width, int height, int scale) {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
    asw::util::abortOnError("SDL_Init");
  }

  // Initialize PNG loading
  int imgFlags = IMG_INIT_PNG;
  if (!(IMG_Init(imgFlags) & imgFlags)) {
    asw::util::abortOnError("IMG_Init");
  }

  if (TTF_Init()) {
    asw::util::abortOnError("TTF_Init");
  }

  // Initialize SDL_mixer
  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
    asw::util::abortOnError("Mix_OpenAudio");
  }

  asw::display::window = SDL_CreateWindow(
      "", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width * scale,
      height * scale, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

  if (!asw::display::window) {
    asw::util::abortOnError("WINDOW");
  }

  // Get window surface
  asw::display::renderer =
      SDL_CreateRenderer(asw::display::window, -1, SDL_RENDERER_ACCELERATED);

  SDL_RenderSetLogicalSize(asw::display::renderer, width, height);
}