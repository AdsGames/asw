/**
 * @file draw.h
 * @author Allan Legemaate (alegemaate@gmail.com)
 * @brief Routines for drawing sprites and primitives to the screen
 * @date 2023-09-20
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef ASW_DRAW_H
#define ASW_DRAW_H

#include "./types.h"

namespace asw::draw {

  /**
   * @brief Clear the screen to a color.
   *
   * @param color The color to clear the screen to.
   */
  void clearColor(asw::Color color);

  /**
   * @brief Draw a sprite.
   *
   * @param tex The texture to draw.
   * @param x The x position to draw the sprite at.
   * @param y The y position to draw the sprite at.
   */
  void sprite(asw::Texture tex, int x, int y);

  /**
   * @brief Draw a sprite with the option to flip it.
   *
   * @param tex The texture to draw.
   * @param x The x position to draw the sprite at.
   * @param y The y position to draw the sprite at.
   * @param flipX - Whether or not to flip the sprite on the x axis.
   * @param flipY - Whether or not to flip the sprite on the y axis.
   */
  void spriteFlip(asw::Texture tex, int x, int y, bool flipX, bool flipY);

  /**
   * @brief Draw a sprite with the option to stretch it.
   *
   * @param tex The texture to draw.
   * @param x The x position to draw the sprite at.
   * @param y The y position to draw the sprite at.
   * @param w The width to stretch the sprite to.
   * @param h The height to stretch the sprite to.
   */
  void stretchSprite(asw::Texture tex, int x, int y, int w, int h);

  /**
   * @brief Draw a sprite with the option to rotate it.
   *
   * @param tex The texture to draw.
   * @param x The x position to draw the sprite at.
   * @param y The y position to draw the sprite at.
   * @param angleDeg - The angle to rotate the sprite by in degrees.
   */
  void rotateSprite(asw::Texture tex, int x, int y, int angleDeg);

  /**
   * @brief Draw a sprite with the option to stretch a portion of it.
   *
   * @param tex The texture to draw.
   * @param x1 The x position to draw the sprite at.
   * @param y1 The y position to draw the sprite at.
   * @param w1 The width to stretch the sprite to.
   * @param h1 The height to stretch the sprite to.
   * @param x2 The x position of the portion of the sprite to stretch.
   * @param y2 The y position of the portion of the sprite to stretch.
   * @param w2 The width of the portion of the sprite to stretch.
   * @param h2 The height of the portion of the sprite to stretch.
   */
  void stretchSpriteBlit(asw::Texture tex,
                         int x1,
                         int y1,
                         int w1,
                         int h1,
                         int x2,
                         int y2,
                         int w2,
                         int h2);

  /**
   * @brief Draw text left aligned.
   *
   * @param font The font to use.
   * @param text The text to draw.
   * @param x The x position to draw the text at.
   * @param y The y position to draw the text at.
   * @param color The color to draw the text.
   */
  void text(asw::Font font,
            const std::string& text,
            int x,
            int y,
            asw::Color color);

  /**
   * @brief Draw text center aligned.
   *
   * @param font The font to use.
   * @param text The text to draw.
   * @param x The x position to draw the text at.
   * @param y The y position to draw the text at.
   * @param color The color to draw the text.
   */
  void textCenter(asw::Font font,
                  const std::string& text,
                  int x,
                  int y,
                  asw::Color color);

  /**
   * @brief Draw text right aligned.
   *
   * @param font The font to use.
   * @param text The text to draw.
   * @param x The x position to draw the text at.
   * @param y The y position to draw the text at.
   * @param color The color to draw the text.
   */
  void textRight(asw::Font font,
                 const std::string& text,
                 int x,
                 int y,
                 asw::Color color);

  /**
   * @brief Draw a point.
   *
   * @param x The x position of the point.
   * @param y The y position of the point.
   * @param color The color of the point.
   */
  void point(int x, int y, asw::Color color);

  /**
   * @brief Draw a line.
   *
   * @param x1 The x position of the start of the line.
   * @param y1 The y position of the start of the line.
   * @param x2 The x position of the end of the line.
   * @param y2 The y position of the end of the line.
   * @param color The color of the line.
   */
  void line(int x1, int y1, int x2, int y2, asw::Color color);

  /**
   * @brief Draw a rectangle.
   *
   * @param x The x position of the rectangle.
   * @param y The y position of the rectangle.
   * @param w The width of the rectangle.
   * @param h The height of the rectangle.
   * @param color The color of the rectangle.
   */
  void rect(int x, int y, int w, int h, asw::Color color);

  /**
   * @brief Draw a filled rectangle.
   *
   * @param x The x position of the rectangle.
   * @param y The y position of the rectangle.
   * @param w The width of the rectangle.
   * @param h The height of the rectangle.
   * @param color The color of the rectangle.
   */
  void rectFill(int x, int y, int w, int h, asw::Color color);

  /**
   * @brief Draw a cirle.
   *
   * @param x The x position of the center of the circle.
   * @param y The y position of the center of the circle.
   * @param r The radius of the circle.
   * @param color The color of the circle.
   */
  void circle(int x, int y, int r, asw::Color color);

  /**
   * @brief Draw a filled circle.
   *
   * @param x The x position of the center of the circle.
   * @param y The y position of the center of the circle.
   * @param r The radius of the circle.
   * @param color The color of the circle.
   */
  void circleFill(int x, int y, int r, asw::Color color);

  /**
   * @brief Set the blend mode of a texture.
   *
   * @param texture The texture to set the blend mode of.
   * @param mode The blend mode to set.
   */
  void setBlendMode(asw::Texture texture, asw::BlendMode mode);

}  // namespace asw::draw

#endif  // ASW_DRAW_H