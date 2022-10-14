#ifndef ASW_DRAW_H
#define ASW_DRAW_H

#include "./types.h"

namespace asw::draw {

  void clearColor(asw::Color color);

  void sprite(asw::Texture tex, int x, int y);

  void spriteHFlip(asw::Texture tex, int x, int y);

  void spriteVFlip(asw::Texture tex, int x, int y);

  void spriteVHFlip(asw::Texture tex, int x, int y);

  void stretchSprite(asw::Texture tex, int x, int y, int w, int h);

  void stretchSpriteBlit(asw::Texture tex,
                         int x1,
                         int y1,
                         int w1,
                         int h1,
                         int x2,
                         int y2,
                         int w2,
                         int h2);

  void text(asw::Font font, std::string text, int x, int y, asw::Color color);

  void textCenter(asw::Font font,
                  std::string text,
                  int x,
                  int y,
                  asw::Color color);

  void point(int x, int y, asw::Color color);

  void line(int x1, int y1, int x2, int y2, asw::Color color);

  void rect(int x, int y, int w, int h, asw::Color color);

  void rectFill(int x, int y, int w, int h, asw::Color color);

  void circle(int x, int y, int r, asw::Color color);

  void circleFill(int x, int y, int r, asw::Color color);

}  // namespace asw::draw

#endif  // ASW_DRAW_H