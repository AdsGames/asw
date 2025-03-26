#include "./asw/modules/draw.h"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <cmath>

#include "./asw/modules/display.h"
#include "./asw/modules/util.h"

void asw::draw::clearColor(asw::Color color) {
  SDL_SetRenderDrawColor(asw::display::renderer, color.r, color.g, color.b,
                         color.a);
  SDL_RenderClear(asw::display::renderer);
}

void asw::draw::sprite(asw::Texture tex, int x, int y) {
  SDL_FPoint size = asw::util::getTextureSize(tex);

  SDL_FRect dest;
  dest.x = float(x);
  dest.y = float(y);
  dest.w = size.x;
  dest.h = size.y;

  SDL_RenderTexture(asw::display::renderer, tex.get(), nullptr, &dest);
}

void asw::draw::spriteFlip(asw::Texture tex,
                           int x,
                           int y,
                           bool flipX,
                           bool flipY) {
  SDL_FPoint size = asw::util::getTextureSize(tex);

  SDL_FRect dest;
  dest.x = float(x);
  dest.y = float(y);
  dest.w = size.x;
  dest.h = size.y;

  SDL_FlipMode flip = SDL_FLIP_NONE;

  if (flipX) {
    flip = static_cast<SDL_FlipMode>(flip | SDL_FLIP_HORIZONTAL);
  }

  if (flipY) {
    flip = static_cast<SDL_FlipMode>(flip | SDL_FLIP_VERTICAL);
  }

  SDL_RenderTextureRotated(asw::display::renderer, tex.get(), nullptr, &dest, 0,
                           nullptr, flip);
}

void asw::draw::stretchSprite(asw::Texture tex, int x, int y, int w, int h) {
  SDL_FRect dest;
  dest.x = float(x);
  dest.y = float(y);
  dest.w = float(w);
  dest.h = float(h);

  SDL_RenderTexture(asw::display::renderer, tex.get(), nullptr, &dest);
}

void asw::draw::rotateSprite(asw::Texture tex, int x, int y, int angleDeg) {
  SDL_FPoint size = asw::util::getTextureSize(tex);

  SDL_FRect dest;
  dest.x = float(x);
  dest.y = float(y);
  dest.w = float(size.x);
  dest.h = float(size.y);

  SDL_RenderTextureRotated(asw::display::renderer, tex.get(), nullptr, &dest,
                           angleDeg, nullptr, SDL_FLIP_NONE);
}

void asw::draw::stretchSpriteBlit(asw::Texture tex,
                                  int x1,
                                  int y1,
                                  int w1,
                                  int h1,
                                  int x2,
                                  int y2,
                                  int w2,
                                  int h2) {
  SDL_FRect src;
  src.x = float(x1);
  src.y = float(y1);
  src.w = float(w1);
  src.h = float(h1);

  SDL_FRect dest;
  dest.x = float(x2);
  dest.y = float(y2);
  dest.w = float(w2);
  dest.h = float(h2);

  SDL_RenderTexture(asw::display::renderer, tex.get(), &src, &dest);
}

void asw::draw::stretchSpriteRotateBlit(asw::Texture tex,
                                        int x1,
                                        int y1,
                                        int w1,
                                        int h1,
                                        int x2,
                                        int y2,
                                        int w2,
                                        int h2,
                                        int angleDeg) {
  SDL_FRect src;
  src.x = float(x1);
  src.y = float(y1);
  src.w = float(w1);
  src.h = float(h1);

  SDL_FRect dest;
  dest.x = float(x2);
  dest.y = float(y2);
  dest.w = float(w2);
  dest.h = float(h2);

  SDL_RenderTextureRotated(asw::display::renderer, tex.get(), &src, &dest,
                           angleDeg, nullptr, SDL_FLIP_NONE);
}

void asw::draw::text(asw::Font font,
                     const std::string& text,
                     int x,
                     int y,
                     asw::Color color) {
  if (!text.size()) {
    return;
  }

  SDL_Color sdlColor = util::makeColor(color.r, color.g, color.b, color.a);
  SDL_Surface* textSurface =
      TTF_RenderText_Solid(font.get(), text.c_str(), 0, sdlColor);
  SDL_Texture* textTexture =
      SDL_CreateTextureFromSurface(asw::display::renderer, textSurface);

  SDL_FRect dest;
  dest.x = float(x);
  dest.y = float(y);
  dest.w = float(textSurface->w);
  dest.h = float(textSurface->h);

  SDL_RenderTexture(asw::display::renderer, textTexture, nullptr, &dest);
  SDL_DestroySurface(textSurface);
  SDL_DestroyTexture(textTexture);
}

void asw::draw::textCenter(asw::Font font,
                           const std::string& text,
                           int x,
                           int y,
                           asw::Color color) {
  auto size = asw::util::getTextSize(font, text);
  asw::draw::text(font, text, x - size.x / 2, y, color);
}

void asw::draw::textRight(asw::Font font,
                          const std::string& text,
                          int x,
                          int y,
                          asw::Color color) {
  auto size = asw::util::getTextSize(font, text);
  asw::draw::text(font, text, x - size.x, y, color);
}

void asw::draw::point(int x, int y, asw::Color color) {
  SDL_SetRenderDrawColor(asw::display::renderer, color.r, color.g, color.b,
                         color.a);
  SDL_RenderPoint(asw::display::renderer, x, y);
}

void asw::draw::line(int x1, int y1, int x2, int y2, asw::Color color) {
  SDL_SetRenderDrawColor(asw::display::renderer, color.r, color.g, color.b,
                         color.a);
  SDL_RenderLine(asw::display::renderer, x1, y1, x2, y2);
}

void asw::draw::rect(int x, int y, int w, int h, asw::Color color) {
  SDL_SetRenderDrawColor(asw::display::renderer, color.r, color.g, color.b,
                         color.a);
  SDL_FRect rect;
  rect.x = float(x);
  rect.y = float(y);
  rect.w = float(w);
  rect.h = float(h);

  SDL_RenderRect(asw::display::renderer, &rect);
}

void asw::draw::rectFill(int x, int y, int w, int h, asw::Color color) {
  SDL_SetRenderDrawColor(asw::display::renderer, color.r, color.g, color.b,
                         color.a);
  SDL_FRect rect;
  rect.x = float(x);
  rect.y = float(y);
  rect.w = float(w);
  rect.h = float(h);

  SDL_RenderFillRect(asw::display::renderer, &rect);
}

void asw::draw::circle(int x, int y, int r, asw::Color color) {
  SDL_SetRenderDrawColor(asw::display::renderer, color.r, color.g, color.b,
                         color.a);
  for (int i = 0; i < 360; i++) {
    SDL_RenderPoint(asw::display::renderer, x + r * std::cos(i),
                    y + r * std::sin(i));
  }
}

void asw::draw::circleFill(int x, int y, int r, asw::Color color) {
  SDL_SetRenderDrawColor(asw::display::renderer, color.r, color.g, color.b,
                         color.a);
  for (int i = 0; i < 360; i++) {
    SDL_RenderLine(asw::display::renderer, x, y, x + r * std::cos(i),
                   y + r * std::sin(i));
  }
}

void asw::draw::setBlendMode(asw::Texture texture, asw::BlendMode mode) {
  SDL_SetTextureBlendMode(texture.get(), static_cast<SDL_BlendMode>(mode));
}
