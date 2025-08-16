#include "./asw/modules/draw.h"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <cmath>

#include "./asw/modules/display.h"
#include "./asw/modules/util.h"

void asw::draw::clearColor(asw::Color color) {
  if (!asw::display::renderer) {
    return;
  }

  SDL_SetRenderDrawColor(asw::display::renderer, color.r, color.g, color.b,
                         color.a);
  SDL_RenderClear(asw::display::renderer);
}

void asw::draw::sprite(const asw::Texture& tex,
                       const asw::Vec2<float>& position) {
  if (!asw::display::renderer) {
    return;
  }

  auto size = asw::util::getTextureSize(tex);

  SDL_FRect dest;
  dest.x = position.x;
  dest.y = position.y;
  dest.w = size.x;
  dest.h = size.y;

  SDL_RenderTexture(asw::display::renderer, tex.get(), nullptr, &dest);
}

void asw::draw::spriteFlip(const asw::Texture& tex,
                           const asw::Vec2<float>& position,
                           bool flipX,
                           bool flipY) {
  if (!asw::display::renderer) {
    return;
  }

  auto size = asw::util::getTextureSize(tex);

  SDL_FRect dest;
  dest.x = position.x;
  dest.y = position.y;
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

void asw::draw::stretchSprite(const asw::Texture& tex,
                              const asw::Quad<float>& position) {
  SDL_FRect dest;
  dest.x = position.position.x;
  dest.y = position.position.y;
  dest.w = position.size.x;
  dest.h = position.size.y;

  SDL_RenderTexture(asw::display::renderer, tex.get(), nullptr, &dest);
}

void asw::draw::rotateSprite(const asw::Texture& tex,
                             const asw::Vec2<float>& position,
                             double angle) {
  if (!asw::display::renderer) {
    return;
  }

  auto size = asw::util::getTextureSize(tex);

  SDL_FRect dest;
  dest.x = position.x;
  dest.y = position.y;
  dest.w = size.x;
  dest.h = size.y;

  SDL_RenderTextureRotated(asw::display::renderer, tex.get(), nullptr, &dest,
                           angle, nullptr, SDL_FLIP_NONE);
}

void asw::draw::stretchSpriteBlit(const asw::Texture& tex,
                                  const asw::Quad<float>& source,
                                  const asw::Quad<float>& dest) {
  if (!asw::display::renderer) {
    return;
  }

  SDL_FRect r_src;
  r_src.x = source.position.x;
  r_src.y = source.position.y;
  r_src.w = source.size.x;
  r_src.h = source.size.y;

  SDL_FRect r_dest;
  r_dest.x = dest.position.x;
  r_dest.y = dest.position.y;
  r_dest.w = dest.size.x;
  r_dest.h = dest.size.y;

  SDL_RenderTexture(asw::display::renderer, tex.get(), &r_src, &r_dest);
}

void asw::draw::stretchSpriteRotateBlit(const asw::Texture& tex,
                                        const asw::Quad<float>& source,
                                        const asw::Quad<float>& dest,
                                        double angle) {
  if (!asw::display::renderer) {
    return;
  }

  SDL_FRect r_src;
  r_src.x = source.position.x;
  r_src.y = source.position.y;
  r_src.w = source.size.x;
  r_src.h = source.size.y;

  SDL_FRect r_dest;
  r_dest.x = dest.position.x;
  r_dest.y = dest.position.y;
  r_dest.w = dest.size.x;
  r_dest.h = dest.size.y;

  SDL_RenderTextureRotated(asw::display::renderer, tex.get(), &r_src, &r_dest,
                           angle, nullptr, SDL_FLIP_NONE);
}

void asw::draw::text(const asw::Font& font,
                     const std::string& text,
                     const asw::Vec2<float>& position,
                     asw::Color color) {
  if (text.empty() || !asw::display::renderer) {
    return;
  }

  const SDL_Color sdlColor =
      asw::util::makeColor(color.r, color.g, color.b, color.a);
  SDL_Surface* textSurface =
      TTF_RenderText_Solid(font.get(), text.c_str(), 0, sdlColor);
  SDL_Texture* textTexture =
      SDL_CreateTextureFromSurface(asw::display::renderer, textSurface);

  SDL_SetTextureBlendMode(textTexture, SDL_BLENDMODE_BLEND);
  SDL_SetTextureScaleMode(textTexture, SDL_SCALEMODE_NEAREST);

  SDL_FRect dest;
  dest.x = position.x;
  dest.y = position.y;
  dest.w = float(textSurface->w);
  dest.h = float(textSurface->h);

  SDL_RenderTexture(asw::display::renderer, textTexture, nullptr, &dest);
  SDL_DestroySurface(textSurface);
  SDL_DestroyTexture(textTexture);
}

void asw::draw::textCenter(const asw::Font& font,
                           const std::string& text,
                           const asw::Vec2<float>& position,
                           asw::Color color) {
  if (!asw::display::renderer) {
    return;
  }

  auto size = asw::util::getTextSize(font, text);
  asw::draw::text(font, text, position - asw::Vec2<float>(size.x / 2, 0),
                  color);
}

void asw::draw::textRight(const asw::Font& font,
                          const std::string& text,
                          const asw::Vec2<float>& position,
                          asw::Color color) {
  if (!asw::display::renderer) {
    return;
  }

  auto size = asw::util::getTextSize(font, text);
  asw::draw::text(font, text, position - asw::Vec2<float>(size.x, 0), color);
}

void asw::draw::point(const asw::Vec2<float>& position, asw::Color color) {
  if (!asw::display::renderer) {
    return;
  }

  SDL_SetRenderDrawColor(asw::display::renderer, color.r, color.g, color.b,
                         color.a);
  SDL_RenderPoint(asw::display::renderer, position.x, position.y);
}

void asw::draw::line(const asw::Vec2<float>& position1,
                     const asw::Vec2<float>& position2,
                     asw::Color color) {
  if (!asw::display::renderer) {
    return;
  }

  SDL_SetRenderDrawColor(asw::display::renderer, color.r, color.g, color.b,
                         color.a);
  SDL_RenderLine(asw::display::renderer, position1.x, position1.y, position2.x,
                 position2.y);
}

void asw::draw::rect(const asw::Quad<float>& position, asw::Color color) {
  if (!asw::display::renderer) {
    return;
  }

  SDL_SetRenderDrawColor(asw::display::renderer, color.r, color.g, color.b,
                         color.a);
  SDL_FRect rect;
  rect.x = position.position.x;
  rect.y = position.position.y;
  rect.w = position.size.x;
  rect.h = position.size.y;

  SDL_RenderRect(asw::display::renderer, &rect);
}

void asw::draw::rectFill(const asw::Quad<float>& position, asw::Color color) {
  if (!asw::display::renderer) {
    return;
  }

  SDL_SetRenderDrawColor(asw::display::renderer, color.r, color.g, color.b,
                         color.a);
  SDL_FRect rect;
  rect.x = position.position.x;
  rect.y = position.position.y;
  rect.w = position.size.x;
  rect.h = position.size.y;

  SDL_RenderFillRect(asw::display::renderer, &rect);
}

void asw::draw::circle(const asw::Vec2<float>& position,
                       float radius,
                       asw::Color color) {
  if (!asw::display::renderer) {
    return;
  }

  SDL_SetRenderDrawColor(asw::display::renderer, color.r, color.g, color.b,
                         color.a);
  for (int i = 0; i < 360; i++) {
    SDL_RenderPoint(asw::display::renderer,
                    position.x + static_cast<float>(radius * std::cos(i)),
                    position.y + static_cast<float>(radius * std::sin(i)));
  }
}

void asw::draw::circleFill(const asw::Vec2<float>& position,
                           float radius,
                           asw::Color color) {
  if (!asw::display::renderer) {
    return;
  }

  SDL_SetRenderDrawColor(asw::display::renderer, color.r, color.g, color.b,
                         color.a);
  for (int i = 0; i < 360; i++) {
    SDL_RenderLine(asw::display::renderer, position.x, position.y,
                   position.x + static_cast<float>(radius * std::cos(i)),
                   position.y + static_cast<float>(radius * std::sin(i)));
  }
}

void asw::draw::setBlendMode(const asw::Texture& texture, asw::BlendMode mode) {
  SDL_SetTextureBlendMode(texture.get(), static_cast<SDL_BlendMode>(mode));
}

void asw::draw::setAlpha(const asw::Texture& texture, float alpha) {
  SDL_SetTextureAlphaModFloat(texture.get(), alpha);
}
