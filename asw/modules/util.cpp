#include "./util.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

void asw::util::abortOnError(const std::string& message) {
  SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", message.c_str(),
                           nullptr);
  exit(-1);
}

asw::Color asw::util::makeColor(int r, int g, int b) {
  asw::Color color;
  color.r = r;
  color.g = g;
  color.b = b;
  color.a = 255;
  return color;
}

asw::Color asw::util::makeColor(int r, int g, int b, int a) {
  asw::Color color;
  color.r = r;
  color.g = g;
  color.b = b;
  color.a = a;
  return color;
}

SDL_Point asw::util::getTextureSize(asw::Texture tex) {
  SDL_Point size;
  SDL_QueryTexture(tex.get(), nullptr, nullptr, &size.x, &size.y);
  return size;
}

SDL_Point asw::util::getTextSize(asw::Font font, const std::string& text) {
  SDL_Point size;
  TTF_SizeText(font.get(), text.c_str(), &size.x, &size.y);
  return size;
}