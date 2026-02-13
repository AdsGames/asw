#include "./asw/modules/core.h"

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <algorithm>

#include "./asw/modules/display.h"
#include "./asw/modules/input.h"
#include "./asw/modules/log.h"
#include "./asw/modules/util.h"

bool asw::core::exit = false;

void asw::core::update() {
  asw::input::reset();

  auto& mouse = asw::input::mouse;
  auto& keyboard = asw::input::keyboard;
  auto& controller = asw::input::controller;

  SDL_Event e;

  while (SDL_PollEvent(&e)) {
    switch (e.type) {
      case SDL_EVENT_WINDOW_RESIZED: {
        if (asw::display::renderer == nullptr) {
          break;
        }

        // Maintain aspect ratio
        SDL_Point window_size;
        SDL_GetRenderOutputSize(asw::display::renderer, &window_size.x,
                                &window_size.y);

        SDL_Point render_size;
        SDL_GetRenderLogicalPresentation(asw::display::renderer, &render_size.x,
                                         &render_size.y, nullptr);

        const auto x_scale = static_cast<float>(window_size.x) /
                             static_cast<float>(render_size.x);

        const auto y_scale = static_cast<float>(window_size.y) /
                             static_cast<float>(render_size.y);

        const auto scale = std::min(x_scale, y_scale);

        SDL_SetWindowSize(asw::display::window,
                          static_cast<float>(render_size.x) * scale,
                          static_cast<float>(render_size.y) * scale);
        break;
      }

      case SDL_EVENT_KEY_DOWN: {
        if (!e.key.repeat) {
          keyboard.pressed[e.key.scancode] = true;
          keyboard.down[e.key.scancode] = true;
          keyboard.anyPressed = true;
          keyboard.lastPressed = e.key.scancode;
        }
        break;
      }

      case SDL_EVENT_KEY_UP: {
        if (!e.key.repeat) {
          keyboard.released[e.key.scancode] = true;
          keyboard.down[e.key.scancode] = false;
        }
        break;
      }

      case SDL_EVENT_MOUSE_BUTTON_DOWN: {
        const auto button = static_cast<int>(e.button.button);
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

      case SDL_EVENT_MOUSE_MOTION: {
        // Ensure scale is applied to mouse coordinates
        SDL_ConvertEventToRenderCoordinates(asw::display::renderer, &e);
        mouse.xChange = e.motion.xrel;
        mouse.yChange = e.motion.yrel;
        mouse.position.x = e.motion.x;
        mouse.position.y = e.motion.y;
        break;
      }

      case SDL_EVENT_MOUSE_WHEEL: {
        mouse.z = e.wheel.y;
        break;
      }

      case SDL_EVENT_GAMEPAD_AXIS_MOTION: {
        if (e.gaxis.which >= asw::input::MAX_CONTROLLERS) {
          break;
        }

        auto motion = static_cast<float>(e.gaxis.value) / 32768.0F;
        auto& current = controller[e.gaxis.which];

        if (motion > current.deadZone) {
          current.axis[e.gaxis.axis] = motion;
        } else if (motion < -current.deadZone) {
          current.axis[e.gaxis.axis] = motion;
        } else {
          current.axis[e.gaxis.axis] = 0.0F;
        }

        break;
      }

      case SDL_EVENT_GAMEPAD_BUTTON_DOWN: {
        if (e.gbutton.which >= asw::input::MAX_CONTROLLERS) {
          break;
        }

        auto button = static_cast<int>(e.gbutton.button);
        controller[e.gbutton.which].pressed[button] = true;
        controller[e.gbutton.which].down[button] = true;
        controller[e.gbutton.which].anyPressed = true;
        controller[e.gbutton.which].lastPressed = button;
        break;
      }

      case SDL_EVENT_GAMEPAD_BUTTON_UP: {
        if (e.gbutton.which >= asw::input::MAX_CONTROLLERS) {
          break;
        }

        auto button = static_cast<int>(e.gbutton.button);
        controller[e.gbutton.which].released[button] = true;
        controller[e.gbutton.which].down[button] = false;
        break;
      }

      case SDL_EVENT_GAMEPAD_ADDED: {
        if (e.gdevice.which >= asw::input::MAX_CONTROLLERS ||
            !SDL_IsGamepad(e.gdevice.which) ||
            SDL_OpenGamepad(e.gdevice.which) == nullptr) {
          // TODO: Log error
          break;
        }

        SDL_OpenGamepad(e.gdevice.which);

        break;
      }

      case SDL_EVENT_GAMEPAD_REMOVED: {
        if (e.gdevice.which >= asw::input::MAX_CONTROLLERS) {
          break;
        }

        auto* controller = SDL_GetGamepadFromID(e.gdevice.which);

        if (controller) {
          SDL_CloseGamepad(controller);
        }

        break;
      }

      case SDL_EVENT_QUIT: {
        exit = true;
        break;
      }

      default:
        break;
    }
  }
}

void asw::core::init(int width, int height, int scale) {
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD)) {
    asw::util::abortOnError("SDL_Init");
  }

  if (!TTF_Init()) {
    asw::util::abortOnError("TTF_Init");
  }

  // Initialize SDL_mixer
  SDL_AudioSpec spec;
  spec.format = SDL_AUDIO_S16LE;
  spec.freq = 44100;
  spec.channels = 2;

  if (!Mix_OpenAudio(0, &spec)) {
    asw::util::abortOnError("Mix_OpenAudio");
  }

  asw::display::window =
      SDL_CreateWindow("", width * scale, height * scale, SDL_WINDOW_RESIZABLE);

  if (asw::display::window == nullptr) {
    asw::util::abortOnError("WINDOW");
  }

  // Hints
  SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");

  // Get window surface
  asw::display::renderer = SDL_CreateRenderer(asw::display::window, nullptr);

  SDL_SetRenderLogicalPresentation(asw::display::renderer, width, height,
                                   SDL_LOGICAL_PRESENTATION_LETTERBOX);
}

void asw::core::print_info() {
  asw::log::info("ASW Info");
  asw::log::info("========");

  const char* renderer_name = "None";
  if (asw::display::renderer != nullptr) {
    renderer_name = SDL_GetRendererName(asw::display::renderer);
  }

  const bool is_software = false;  // info.flags & SDL_SOFTWARE_RENDERER;
  const bool is_accelerated = !is_software;
  const bool is_target_texture = true;
  const bool is_vsync = true;  // info.flags & SDL_RENDERER_PRESENTVSYNC;

  const std::string sdl_version = std::to_string(SDL_MAJOR_VERSION) + "." +
                                  std::to_string(SDL_MINOR_VERSION) + "." +
                                  std::to_string(SDL_MICRO_VERSION);

  asw::log::info("SDL Version: " + sdl_version);
  asw::log::info("Renderer: " + std::string(renderer_name));
  asw::log::info("Accelerated: " + std::to_string(is_accelerated));
  asw::log::info("Software: " + std::to_string(is_software));
  asw::log::info("Target Texture: " + std::to_string(is_target_texture));
  asw::log::info("Vsync: " + std::to_string(is_vsync));
}