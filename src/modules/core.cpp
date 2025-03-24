#include "./asw/modules/core.h"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
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
      case SDL_EVENT_WINDOW_RESIZED: {
        // Maintain aspect ratio
        SDL_FPoint scale;
        SDL_GetRenderScale(asw::display::renderer, &scale.x, &scale.y);

        SDL_Point size;
        SDL_GetRenderLogicalPresentation(asw::display::renderer, &size.x,
                                         &size.y, nullptr);

        SDL_SetWindowSize(asw::display::window, size.x * scale.x,
                          size.y * scale.y);
        break;
      }

      case SDL_EVENT_KEY_DOWN:
        if (e.key.repeat == 0) {
          keyboard.pressed[e.key.scancode] = true;
          keyboard.down[e.key.scancode] = true;
          keyboard.anyPressed = true;
          keyboard.lastPressed = e.key.scancode;
        }
        break;

      case SDL_EVENT_KEY_UP:
        if (e.key.repeat == 0) {
          keyboard.released[e.key.scancode] = true;
          keyboard.down[e.key.scancode] = false;
        }
        break;

      case SDL_EVENT_MOUSE_BUTTON_DOWN: {
        auto button = static_cast<int>(e.button.button);
        mouse.pressed[button] = true;
        mouse.down[button] = true;
        mouse.anyPressed = true;
        mouse.lastPressed = button;
        break;
      }

      case SDL_EVENT_MOUSE_BUTTON_UP: {
        auto button = static_cast<int>(e.button.button);
        mouse.released[button] = true;
        mouse.down[button] = false;
        break;
      }

      case SDL_EVENT_MOUSE_MOTION:
        mouse.xChange = e.motion.xrel;
        mouse.yChange = e.motion.yrel;
        mouse.x = e.motion.x;
        mouse.y = e.motion.y;
        break;

      case SDL_EVENT_MOUSE_WHEEL:
        mouse.z = e.wheel.y;
        break;

      case SDL_EVENT_QUIT:
        exit = true;
        break;

      default:
        break;
    }
  }
}

void asw::core::init(int width, int height, int scale) {
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
    asw::util::abortOnError("SDL_Init");
  }

  if (TTF_Init()) {
    asw::util::abortOnError("TTF_Init");
  }

  // Initialize SDL_mixer
  auto spec = SDL_AudioSpec{
      .freq = 44100,
      .format = SDL_AUDIO_S16LE,
      .channels = 2,
  };
  if (!Mix_OpenAudio(0, &spec)) {
    asw::util::abortOnError("Mix_OpenAudio");
  }

  asw::display::window =
      SDL_CreateWindow("", width * scale, height * scale, SDL_WINDOW_RESIZABLE);

  if (!asw::display::window) {
    asw::util::abortOnError("WINDOW");
  }

  // Get window surface
  asw::display::renderer = SDL_CreateRenderer(asw::display::window, nullptr);

  SDL_SetRenderLogicalPresentation(asw::display::renderer, width, height,
                                   SDL_LOGICAL_PRESENTATION_STRETCH);
}

void asw::core::print_info() {
  std::cout << "ASW Info" << std::endl;
  std::cout << "========" << std::endl;

  auto renderer_name = SDL_GetRendererName(asw::display::renderer);

  bool is_software = false;  // info.flags & SDL_SOFTWARE_RENDERER;
  bool is_accelerated = !is_software;
  bool is_target_texture = true;
  bool is_vsync = true;  // info.flags & SDL_RENDERER_PRESENTVSYNC;

  std::cout << "SDL Version: " << static_cast<int>(SDL_MAJOR_VERSION) << "."
            << static_cast<int>(SDL_MINOR_VERSION) << "."
            << static_cast<int>(SDL_MICRO_VERSION) << std::endl;

  std::cout << "Renderer: " << renderer_name << std::endl;
  std::cout << "Accelerated: " << is_accelerated << std::endl;
  std::cout << "Software: " << is_software << std::endl;
  std::cout << "Target Texture: " << is_target_texture << std::endl;
  std::cout << "Vsync: " << is_vsync << std::endl;
}