#include "./asw/modules/display.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

asw::Renderer* asw::display::renderer = nullptr;
asw::Window* asw::display::window = nullptr;

void asw::display::setTitle(const std::string& title) {
  SDL_SetWindowTitle(asw::display::window, title.c_str());
}

void asw::display::setIcon(const std::string& path) {
  SDL_Surface* icon = IMG_Load(path.c_str());

  if (!icon) {
    return;
  }

  SDL_SetWindowIcon(asw::display::window, icon);
}

void asw::display::setFullscreen(bool fullscreen) {
  if (fullscreen) {
    SDL_SetWindowFullscreen(asw::display::window, SDL_WINDOW_FULLSCREEN);
  } else {
    SDL_SetWindowFullscreen(asw::display::window, 0);
  }
}

void asw::display::setResolution(int w, int h) {
  SDL_SetWindowSize(asw::display::window, w, h);
}

SDL_Point asw::display::getSize() {
  SDL_Point size;
  SDL_GetWindowSize(asw::display::window, &size.x, &size.y);
  return size;
}

SDL_Point asw::display::getLogicalSize() {
  SDL_Point size;
  SDL_RenderGetLogicalSize(asw::display::renderer, &size.x, &size.y);
  return size;
}

SDL_FPoint asw::display::getScale() {
  SDL_FPoint scale;
  SDL_RenderGetScale(asw::display::renderer, &scale.x, &scale.y);
  return scale;
}

void asw::display::setRenderTarget(asw::Texture texture) {
  SDL_SetRenderTarget(asw::display::renderer, texture.get());
}

void asw::display::resetRenderTarget() {
  SDL_SetRenderTarget(asw::display::renderer, nullptr);
}

void asw::display::clear() {
  SDL_RenderClear(asw::display::renderer);
}

void asw::display::present() {
  SDL_RenderPresent(asw::display::renderer);
}