#include "./asw/modules/display.h"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <string>

#include "./asw/modules/types.h"
#include "./asw/modules/util.h"

namespace {
asw::Renderer* renderer = nullptr;
asw::Window* window = nullptr;
} // namespace

void asw::display::_init(int width, int height, int scale)
{
    window = SDL_CreateWindow("", width * scale, height * scale, SDL_WINDOW_RESIZABLE);
    if (window == nullptr) {
        asw::util::abort_on_error("WINDOW");
    }

    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");

    renderer = SDL_CreateRenderer(window, nullptr);

    SDL_SetRenderLogicalPresentation(
        renderer, width, height, SDL_LOGICAL_PRESENTATION_LETTERBOX);
}

void asw::display::_shutdown()
{
    auto* r = renderer;
    renderer = nullptr;
    if (r != nullptr) {
        SDL_DestroyRenderer(r);
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
    SDL_SetWindowFullscreen(window, fullscreen);
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
    asw::Vec2<int> size;

    if (renderer == nullptr) {
        return size;
    }

    SDL_GetRenderLogicalPresentation(renderer, &size.x, &size.y, nullptr);
    return size;
}

asw::Vec2<float> asw::display::get_scale()
{
    asw::Vec2<float> scale;

    if (renderer == nullptr) {
        return scale;
    }

    SDL_GetRenderScale(renderer, &scale.x, &scale.y);
    return scale;
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

    SDL_RenderClear(renderer);
}

void asw::display::clear(const asw::Color& color)
{
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
