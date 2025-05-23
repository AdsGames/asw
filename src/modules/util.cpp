#include "./asw/modules/util.h"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

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

asw::Vec2<float> asw::util::getTextureSize(const asw::Texture& tex) {
  asw::Vec2<float> size;
  SDL_GetTextureSize(tex.get(), &size.x, &size.y);
  return size;
}

asw::Vec2<int> asw::util::getTextSize(const asw::Font& font,
                                      const std::string& text) {
  TTF_Text* ttf_text = TTF_CreateText(nullptr, font.get(), text.c_str(), 0);
  asw::Vec2<int> size;
  TTF_GetTextSize(ttf_text, &size.x, &size.y);
  return size;
}