#include "./asw/modules/core.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>

#include "./asw/modules/display.h"
#include "./asw/modules/input.h"
#include "./asw/modules/util.h"

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

          default:
            break;
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
        mouse.xChange = e.motion.xrel;
        mouse.yChange = e.motion.yrel;
        mouse.x = e.motion.x;
        mouse.y = e.motion.y;
        break;

      case SDL_MOUSEWHEEL:
        mouse.z = e.wheel.y;
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
      SDL_CreateRenderer(asw::display::window, -1,
                         SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  SDL_RenderSetLogicalSize(asw::display::renderer, width, height);
}

void asw::core::print_info() {
  std::cout << "ASW Info" << std::endl;
  std::cout << "========" << std::endl;

  SDL_version compiled;
  SDL_GetVersion(&compiled);

  SDL_RendererInfo info;
  SDL_GetRendererInfo(asw::display::renderer, &info);

  bool is_accelerated = info.flags & SDL_RENDERER_ACCELERATED;
  bool is_software = info.flags & SDL_RENDERER_SOFTWARE;
  bool is_target_texture = info.flags & SDL_RENDERER_TARGETTEXTURE;
  bool is_vsync = info.flags & SDL_RENDERER_PRESENTVSYNC;

  std::cout << "SDL Version: " << static_cast<int>(compiled.major) << "."
            << static_cast<int>(compiled.minor) << "."
            << static_cast<int>(compiled.patch) << std::endl;

  std::cout << "Renderer: " << info.name << std::endl;
  std::cout << "Accelerated: " << is_accelerated << std::endl;
  std::cout << "Software: " << is_software << std::endl;
  std::cout << "Target Texture: " << is_target_texture << std::endl;
  std::cout << "Vsync: " << is_vsync << std::endl;
}