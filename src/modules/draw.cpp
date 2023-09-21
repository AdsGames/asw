#include "./asw/modules/draw.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "./asw/modules/display.h"
#include "./asw/modules/util.h"

void asw::draw::clearColor(asw::Color color) {
  SDL_SetRenderDrawColor(asw::display::renderer, color.r, color.g, color.b,
                         color.a);
  SDL_RenderClear(asw::display::renderer);
}

void asw::draw::sprite(asw::Texture tex, int x, int y) {
  SDL_Point size = asw::util::getTextureSize(tex);
  SDL_Rect dest = {x, y, size.x, size.y};
  SDL_RenderCopy(asw::display::renderer, tex.get(), nullptr, &dest);
}

void asw::draw::spriteFlip(asw::Texture tex,
                           int x,
                           int y,
                           bool flipX,
                           bool flipY) {
  SDL_Point size = asw::util::getTextureSize(tex);
  SDL_Rect dest = {x, y, size.x, size.y};
  SDL_RendererFlip flip = SDL_FLIP_NONE;

  if (flipX) {
    flip = static_cast<SDL_RendererFlip>(flip | SDL_FLIP_HORIZONTAL);
  }

  if (flipY) {
    flip = static_cast<SDL_RendererFlip>(flip | SDL_FLIP_VERTICAL);
  }

  SDL_RenderCopyEx(asw::display::renderer, tex.get(), nullptr, &dest, 0,
                   nullptr, flip);
}

void asw::draw::stretchSprite(asw::Texture tex, int x, int y, int w, int h) {
  SDL_Rect dest = {x, y, w, h};
  SDL_RenderCopy(asw::display::renderer, tex.get(), nullptr, &dest);
}

void asw::draw::rotateSprite(asw::Texture tex, int x, int y, int angleDeg) {
  SDL_Point size = asw::util::getTextureSize(tex);
  SDL_Rect dest = {x, y, size.x, size.y};
  SDL_RenderCopyEx(asw::display::renderer, tex.get(), nullptr, &dest, angleDeg,
                   nullptr, SDL_FLIP_NONE);
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
  const SDL_Rect src = {x1, y1, w1, h1};
  SDL_Rect dest = {x2, y2, w2, h2};
  SDL_RenderCopy(asw::display::renderer, tex.get(), &src, &dest);
}

void asw::draw::text(asw::Font font,
                     const std::string& text,
                     int x,
                     int y,
                     asw::Color color) {
  if (!text.size()) {
    return;
  }

  SDL_Color sdlColor = {color.r, color.g, color.b, color.a};
  SDL_Surface* textSurface =
      TTF_RenderText_Solid(font.get(), text.c_str(), sdlColor);
  SDL_Texture* textTexture =
      SDL_CreateTextureFromSurface(asw::display::renderer, textSurface);
  SDL_Rect dest = {x, y, textSurface->w, textSurface->h};
  SDL_RenderCopy(asw::display::renderer, textTexture, nullptr, &dest);
  SDL_FreeSurface(textSurface);
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
  SDL_RenderDrawPoint(asw::display::renderer, x, y);
}

void asw::draw::line(int x1, int y1, int x2, int y2, asw::Color color) {
  SDL_SetRenderDrawColor(asw::display::renderer, color.r, color.g, color.b,
                         color.a);
  SDL_RenderDrawLine(asw::display::renderer, x1, y1, x2, y2);
}

void asw::draw::rect(int x, int y, int w, int h, asw::Color color) {
  SDL_SetRenderDrawColor(asw::display::renderer, color.r, color.g, color.b,
                         color.a);
  SDL_Rect rect = {x, y, w, h};
  SDL_RenderDrawRect(asw::display::renderer, &rect);
}

void asw::draw::rectFill(int x, int y, int w, int h, asw::Color color) {
  SDL_SetRenderDrawColor(asw::display::renderer, color.r, color.g, color.b,
                         color.a);
  SDL_Rect rect = {x, y, w, h};
  SDL_RenderFillRect(asw::display::renderer, &rect);
}

void asw::draw::circle(int x, int y, int r, asw::Color color) {
  SDL_SetRenderDrawColor(asw::display::renderer, color.r, color.g, color.b,
                         color.a);
  for (int i = 0; i < 360; i++) {
    SDL_RenderDrawPoint(asw::display::renderer, x + r * cos(i), y + r * sin(i));
  }
}

void asw::draw::circleFill(int x, int y, int r, asw::Color color) {
  SDL_SetRenderDrawColor(asw::display::renderer, color.r, color.g, color.b,
                         color.a);
  for (int i = 0; i < 360; i++) {
    SDL_RenderDrawLine(asw::display::renderer, x, y, x + r * cos(i),
                       y + r * sin(i));
  }
}

void asw::draw::setBlendMode(asw::Texture texture, asw::BlendMode mode) {
  SDL_SetTextureBlendMode(texture.get(), static_cast<SDL_BlendMode>(mode));
}
