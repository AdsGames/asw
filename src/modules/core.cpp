#include "./asw/modules/core.h"

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <algorithm>
#include <format>

#include "./asw/modules/action.h"
#include "./asw/modules/assets.h"
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
    asw::input::reset();

    SDL_Event e;

    while (SDL_PollEvent(&e)) {
        switch (e.type) {
        case SDL_EVENT_WINDOW_RESIZED: {
            auto* r = asw::display::get_renderer();
            if (r == nullptr) {
                break;
            }

            // Maintain aspect ratio
            SDL_Point window_size;
            SDL_GetRenderOutputSize(r, &window_size.x, &window_size.y);

            SDL_Point render_size;
            SDL_GetRenderLogicalPresentation(r, &render_size.x, &render_size.y, nullptr);

            const auto x_scale
                = static_cast<float>(window_size.x) / static_cast<float>(render_size.x);

            const auto y_scale
                = static_cast<float>(window_size.y) / static_cast<float>(render_size.y);

            const auto scale = std::min(x_scale, y_scale);

            SDL_SetWindowSize(asw::display::get_window(),
                static_cast<int>(static_cast<float>(render_size.x) * scale),
                static_cast<int>(static_cast<float>(render_size.y) * scale));
            break;
        }

        case SDL_EVENT_KEY_DOWN: {
            if (!e.key.repeat) {
                asw::input::_key_down(e.key.scancode);
            }
            break;
        }

        case SDL_EVENT_KEY_UP: {
            if (!e.key.repeat) {
                asw::input::_key_up(e.key.scancode);
            }
            break;
        }

        case SDL_EVENT_MOUSE_BUTTON_DOWN: {
            asw::input::_mouse_button_down(e.button.button);
            break;
        }

        case SDL_EVENT_MOUSE_BUTTON_UP: {
            asw::input::_mouse_button_up(e.button.button);
            break;
        }

        case SDL_EVENT_MOUSE_MOTION: {
            // Ensure scale is applied to mouse coordinates
            auto* r = asw::display::get_renderer();
            if (r != nullptr) {
                SDL_ConvertEventToRenderCoordinates(r, &e);
            }

            asw::input::_mouse_motion(e.motion.x, e.motion.y, e.motion.xrel, e.motion.yrel);
            break;
        }

        case SDL_EVENT_MOUSE_WHEEL: {
            asw::input::_mouse_wheel(e.wheel.y);
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
            asw::input::_append_text(e.text.text);
            break;
        }

        case SDL_EVENT_QUIT: {
            exit();
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

    asw::display::_init(width, height, scale);
}

void asw::core::print_info()
{
    asw::log::info("ASW Info");
    asw::log::info("========");

    const char* renderer_name = "none";
    if (auto* r = asw::display::get_renderer(); r != nullptr) {
        renderer_name = SDL_GetRendererName(r);
    }

    asw::log::info(
        "SDL Version: {}.{}.{}", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_MICRO_VERSION);
    asw::log::info("Renderer: {}", renderer_name);
}

void asw::core::exit()
{
    exiting = true;
}

void asw::core::shutdown()
{
    asw::input::clear_actions();

    // Clear asset caches while SDL resources are still valid — SDL_Destroy*
    // calls in the shared_ptr deleters are safe at this point.
    asw::assets::clear_all();

    // _shutdown() nulls the renderer/window pointers BEFORE calling
    // SDL_DestroyRenderer/SDL_DestroyWindow.  Any outstanding shared_ptr<SDL_Texture>
    // held by user code will see a null renderer in their deleter and skip the
    // SDL_DestroyTexture call, preventing use-after-free.
    asw::display::_shutdown();

    // Same pattern for sound: null mixer pointer before teardown so any
    // surviving Sample/Music shared_ptrs become no-ops.
    asw::sound::_shutdown();

    TTF_Quit();
    SDL_Quit();
}

bool asw::core::is_exiting()
{
    return exiting;
}
