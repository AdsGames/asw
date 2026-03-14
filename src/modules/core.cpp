#include "./asw/modules/core.h"

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <algorithm>
#include <format>

#include "./asw/modules/display.h"
#include "./asw/modules/input.h"
#include "./asw/modules/log.h"
#include "./asw/modules/sound.h"
#include "./asw/modules/util.h"

namespace {
bool exiting = false;
}

void asw::core::update()
{
    if (exiting) {
        return;
    }

    asw::input::reset();

    auto& mouse = asw::input::mouse;
    auto& keyboard = asw::input::keyboard;

    SDL_Event e;

    while (SDL_PollEvent(&e)) {
        switch (e.type) {
        case SDL_EVENT_WINDOW_RESIZED: {
            if (asw::display::renderer == nullptr) {
                break;
            }

            // Maintain aspect ratio
            SDL_Point window_size;
            SDL_GetRenderOutputSize(asw::display::renderer, &window_size.x, &window_size.y);

            SDL_Point render_size;
            SDL_GetRenderLogicalPresentation(
                asw::display::renderer, &render_size.x, &render_size.y, nullptr);

            const auto x_scale
                = static_cast<float>(window_size.x) / static_cast<float>(render_size.x);

            const auto y_scale
                = static_cast<float>(window_size.y) / static_cast<float>(render_size.y);

            const auto scale = std::min(x_scale, y_scale);

            SDL_SetWindowSize(asw::display::window,
                static_cast<int>(static_cast<float>(render_size.x) * scale),
                static_cast<int>(static_cast<float>(render_size.y) * scale));
            break;
        }

        case SDL_EVENT_KEY_DOWN: {
            if (!e.key.repeat) {
                keyboard.pressed[e.key.scancode] = true;
                keyboard.down[e.key.scancode] = true;
                keyboard.any_pressed = true;
                keyboard.last_pressed = e.key.scancode;
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
            mouse.any_pressed = true;
            mouse.last_pressed = button;
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
            if (asw::display::renderer != nullptr) {
                SDL_ConvertEventToRenderCoordinates(asw::display::renderer, &e);
            }

            mouse.change.x = e.motion.xrel;
            mouse.change.y = e.motion.yrel;
            mouse.position.x = e.motion.x;
            mouse.position.y = e.motion.y;
            break;
        }

        case SDL_EVENT_MOUSE_WHEEL: {
            mouse.z = e.wheel.y;
            break;
        }

        case SDL_EVENT_GAMEPAD_AXIS_MOTION: {
            asw::input::_controller_axis_motion(e.gaxis.which, e.gaxis.axis, e.gaxis.value);
            break;
        }

        case SDL_EVENT_GAMEPAD_BUTTON_DOWN: {
            asw::input::_controller_button_down(e.gbutton.which, e.gbutton.button);
            break;
        }

        case SDL_EVENT_GAMEPAD_BUTTON_UP: {
            asw::input::_controller_button_up(e.gbutton.which, e.gbutton.button);
            break;
        }

        case SDL_EVENT_GAMEPAD_ADDED: {
            asw::input::_controller_added(e.gdevice.which);
            break;
        }

        case SDL_EVENT_GAMEPAD_REMOVED: {
            asw::input::_controller_removed(e.gdevice.which);
            break;
        }

        case SDL_EVENT_TEXT_INPUT: {
            asw::input::text_input += e.text.text;
            break;
        }

        case SDL_EVENT_QUIT: {
            exit();
            break;
        }

        default:
            break;
        }
    }
}

void asw::core::init(int width, int height, int scale)
{
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
        asw::util::abort_on_error("SDL_Init");
    }

    if (!TTF_Init()) {
        asw::util::abort_on_error("TTF_Init");
    }

    if (!asw::sound::_init()) {
        asw::util::abort_on_error("Sound initialization failed");
    }

    asw::display::window
        = SDL_CreateWindow("", width * scale, height * scale, SDL_WINDOW_RESIZABLE);
    if (asw::display::window == nullptr) {
        asw::util::abort_on_error("WINDOW");
    }

    // Hints
    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");

    // Get window surface
    asw::display::renderer = SDL_CreateRenderer(asw::display::window, nullptr);

    SDL_SetRenderLogicalPresentation(
        asw::display::renderer, width, height, SDL_LOGICAL_PRESENTATION_LETTERBOX);
}

void asw::core::print_info()
{
    asw::log::info("ASW Info");
    asw::log::info("========");

    const char* renderer_name = "none";
    if (asw::display::renderer != nullptr) {
        renderer_name = SDL_GetRendererName(asw::display::renderer);
    }

    asw::log::info(
        "SDL Version: {}.{}.{}", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_MICRO_VERSION);
    asw::log::info("Renderer: {}", renderer_name);
}

void asw::core::exit()
{
    exiting = true;
    MIX_Quit();
    TTF_Quit();
    SDL_Quit();
}

bool asw::core::is_exiting()
{
    return exiting;
}