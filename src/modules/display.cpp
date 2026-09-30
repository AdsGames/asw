#include "./asw/modules/display.h"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <algorithm>
#include <optional>
#include <string>

#include "./asw/modules/config.h"
#include "./asw/modules/draw.h"
#include "./asw/modules/log.h"
#include "./asw/modules/types.h"
#include "./asw/modules/util.h"

namespace {
asw::Renderer* renderer = nullptr;
asw::Window* window = nullptr;
SDL_GLContext gl_context = nullptr;

// Logical size of the screen. Kept here because SDL reports the logical
// presentation of the current render target, which is 0 for textures.
asw::Vec2<int> logical_size;

// Settings from the config. They win over what the game asks for
std::optional<bool> fullscreen_override;
std::optional<bool> vsync_override;
std::optional<int> scale_override;

constexpr int MAX_SCALE = 16;

void read_overrides()
{
    fullscreen_override = asw::config::get_bool("display.fullscreen");
    vsync_override = asw::config::get_bool("display.vsync");
    scale_override = asw::config::get_int("display.scale");

    if (scale_override && (*scale_override < 1 || *scale_override > MAX_SCALE)) {
        asw::log::warn(
            "Ignoring config display.scale = {}, use 1 to {}", *scale_override, MAX_SCALE);
        scale_override.reset();
    }
}

// Window flags and scale, with the overrides applied
SDL_WindowFlags apply_overrides(SDL_WindowFlags flags, int& scale)
{
    if (scale_override) {
        scale = *scale_override;
    }

    if (fullscreen_override.value_or(false)) {
        flags |= SDL_WINDOW_FULLSCREEN;
    }

    return flags;
}
} // namespace

void asw::display::_init(int width, int height, int scale)
{
    read_overrides();

    // High pixel density gives the canvas one pixel per device pixel in the
    // browser. Without it the scene is drawn at CSS size and the browser
    // stretches it, which blurs it on high density screens
    SDL_WindowFlags flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY;

#ifdef __EMSCRIPTEN__
    // The WebGL renderer recreates a window that is not OpenGL. That resets
    // the canvas CSS size, so the page's CSS no longer sizes the canvas
    flags |= SDL_WINDOW_OPENGL;
#endif

    flags = apply_overrides(flags, scale);
    window = SDL_CreateWindow("", width * scale, height * scale, flags);
    if (window == nullptr) {
        asw::util::abort_on_error("WINDOW");
    }

    SDL_SetHint(SDL_HINT_RENDER_VSYNC, vsync_override.value_or(true) ? "1" : "0");

    renderer = SDL_CreateRenderer(window, nullptr);

    // SDL defaults to no blending, which makes see through primitives opaque
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    SDL_SetRenderLogicalPresentation(renderer, width, height, SDL_LOGICAL_PRESENTATION_LETTERBOX);
    logical_size = asw::Vec2<int>(width, height);
}

void asw::display::_init_opengl(int width, int height, int scale)
{
    read_overrides();

    const SDL_WindowFlags flags = apply_overrides(
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY, scale);
    window = SDL_CreateWindow("", width * scale, height * scale, flags);
    if (window == nullptr) {
        asw::util::abort_on_error("WINDOW");
    }

    gl_context = SDL_GL_CreateContext(window);
    if (gl_context == nullptr) {
        asw::util::abort_on_error("SDL_GL_CreateContext");
    }

    if (!SDL_GL_MakeCurrent(window, gl_context)) {
        asw::util::abort_on_error("SDL_GL_MakeCurrent");
    }

    if (vsync_override) {
        SDL_GL_SetSwapInterval(*vsync_override ? 1 : 0);
    }
}

void asw::display::_shutdown()
{
    // Release cached text textures while their renderer is still alive.
    asw::draw::clear_text_cache();

    auto* r = renderer;
    renderer = nullptr;
    if (r != nullptr) {
        SDL_DestroyRenderer(r);
    }

    if (gl_context != nullptr) {
        SDL_GL_DestroyContext(gl_context);
        gl_context = nullptr;
    }

    auto* w = window;
    window = nullptr;
    if (w != nullptr) {
        SDL_DestroyWindow(w);
    }
}

asw::Renderer* asw::display::get_renderer()
{
    return renderer;
}

asw::Window* asw::display::get_window()
{
    return window;
}

void asw::display::set_title(const std::string& title)
{
    SDL_SetWindowTitle(window, title.c_str());
}

void asw::display::set_icon(const std::string& path)
{
    SDL_Surface* icon = IMG_Load(path.c_str());

    if (icon == nullptr) {
        return;
    }

    SDL_SetWindowIcon(window, icon);
    SDL_DestroySurface(icon);
}

void asw::display::set_fullscreen(bool fullscreen)
{
    SDL_SetWindowFullscreen(window, fullscreen_override.value_or(fullscreen));
    SDL_SyncWindow(window);
}

bool asw::display::is_fullscreen()
{
    if (window == nullptr) {
        return false;
    }

    return (SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN) != 0;
}

void asw::display::set_resolution(int w, int h)
{
    SDL_SetWindowSize(window, w, h);
}

void asw::display::set_resizable(bool resizable)
{
    SDL_SetWindowResizable(window, resizable);
}

asw::Vec2<int> asw::display::get_size()
{
    asw::Vec2<int> size;
    SDL_GetWindowSize(window, &size.x, &size.y);
    return size;
}

asw::Vec2<int> asw::display::get_logical_size()
{
    if (renderer == nullptr) {
        return { };
    }

    return logical_size;
}

asw::Vec2<float> asw::display::get_scale()
{
    asw::Vec2<float> scale(1.0F, 1.0F);

    if (renderer == nullptr || logical_size.x <= 0 || logical_size.y <= 0) {
        return scale;
    }

    // Letterboxing keeps the aspect ratio, so both axes use the same scale
    int out_w = 0;
    int out_h = 0;
    SDL_GetRenderOutputSize(renderer, &out_w, &out_h);
    const float s = std::min(static_cast<float>(out_w) / static_cast<float>(logical_size.x),
        static_cast<float>(out_h) / static_cast<float>(logical_size.y));
    return { s, s };
}

void asw::display::set_render_target(const asw::Texture& texture)
{
    if (renderer == nullptr) {
        return;
    }

    SDL_SetRenderTarget(renderer, texture.get());
}

void asw::display::reset_render_target()
{
    if (renderer == nullptr) {
        return;
    }

    SDL_SetRenderTarget(renderer, nullptr);
}

void asw::display::clear()
{
    if (renderer == nullptr) {
        return;
    }

    // Draw calls leave their colour set, so pick one rather than clearing to
    // whatever was drawn last
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

void asw::display::clear(const asw::Color& color)
{
    if (renderer == nullptr) {
        return;
    }

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderClear(renderer);
}

void asw::display::present()
{
    if (renderer == nullptr) {
        return;
    }

    SDL_RenderPresent(renderer);
}

void asw::display::set_blend_mode(asw::BlendMode mode)
{
    SDL_SetRenderDrawBlendMode(renderer, static_cast<SDL_BlendMode>(mode));
}

void asw::display::warp_mouse(float x, float y)
{
    SDL_WarpMouseInWindow(window, x, y);
}

void asw::display::swap_window()
{
    SDL_GL_SwapWindow(window);
}

bool asw::display::screenshot(const std::string& path)
{
    if (renderer == nullptr) {
        return false;
    }

    SDL_Surface* surface = SDL_RenderReadPixels(renderer, nullptr);
    if (surface == nullptr) {
        return false;
    }

    const bool saved = IMG_SavePNG(surface, path.c_str());
    SDL_DestroySurface(surface);
    return saved;
}
