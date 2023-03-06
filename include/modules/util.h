#ifndef ASW_UTIL_H
#define ASW_UTIL_H

#include <string>

#include "./types.h"

namespace asw::util {

  void abortOnError(const std::string& message);

  asw::Color makeColor(int r, int g, int b);

  asw::Color makeColor(int r, int g, int b, int a);

  SDL_Point getTextureSize(asw::Texture tex);

  SDL_Point getTextSize(asw::Font font, const std::string& text);

}  // namespace asw::util

#endif  // ASW_UTIL_H