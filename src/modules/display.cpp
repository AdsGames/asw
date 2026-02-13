#include "./asw/modules/display.h"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <string>

#include "./asw/modules/types.h"

asw::Renderer* asw::display::renderer = nullptr;
asw::Window* asw::display::window = nullptr;

void asw::display::setTitle(const std::string& title) {
  SDL_SetWindowTitle(asw::display::window, title.c_str());
}

void asw::display::setIcon(const std::string& path) {
  SDL_Surface* icon = IMG_Load(path.c_str());

  if (icon == nullptr) {
    return;
  }

  SDL_SetWindowIcon(asw::display::window, icon);
}

void asw::display::setFullscreen(bool fullscreen) {
  SDL_SetWindowFullscreen(asw::display::window, fullscreen);
}

void asw::display::setResolution(int w, int h) {
  SDL_SetWindowSize(asw::display::window, w, h);
}

void asw::display::setResizable(bool resizable) {
  SDL_SetWindowResizable(asw::display::window, resizable);
}

SDL_Point asw::display::getSize() {
  SDL_Point size;
  SDL_GetWindowSize(asw::display::window, &size.x, &size.y);
  return size;
}

SDL_Point asw::display::getLogicalSize() {
  SDL_Point size;

  if (asw::display::renderer == nullptr) {
    return size;
  }

  SDL_GetRenderLogicalPresentation(asw::display::renderer, &size.x, &size.y,
                                   nullptr);
  return size;
}

SDL_FPoint asw::display::getScale() {
  SDL_FPoint scale;

  if (asw::display::renderer == nullptr) {
    return scale;
  }

  SDL_GetRenderScale(asw::display::renderer, &scale.x, &scale.y);
  return scale;
}

void asw::display::setRenderTarget(const asw::Texture& texture) {
  if (asw::display::renderer == nullptr) {
    return;
  }

  SDL_SetRenderTarget(asw::display::renderer, texture.get());
}

void asw::display::resetRenderTarget() {
  if (asw::display::renderer == nullptr) {
    return;
  }

  SDL_SetRenderTarget(asw::display::renderer, nullptr);
}

void asw::display::clear() {
  if (asw::display::renderer == nullptr) {
    return;
  }

  SDL_RenderClear(asw::display::renderer);
}

void asw::display::clear(const asw::Color& color) {
  SDL_SetRenderDrawColor(asw::display::renderer, color.r, color.g, color.b,
                         color.a);
  SDL_RenderClear(asw::display::renderer);
}

void asw::display::present() {
  if (asw::display::renderer == nullptr) {
    return;
  }

  SDL_RenderPresent(asw::display::renderer);
}

void asw::display::setBlendMode(asw::BlendMode mode) {
  SDL_SetRenderDrawBlendMode(asw::display::renderer,
                             static_cast<SDL_BlendMode>(mode));
}
