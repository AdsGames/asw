/// @file draw.h
/// @author Allan Legemaate (alegemaate@gmail.com)
/// @brief Routines for drawing sprites and primitives to the screen
/// @date 2023-09-20
///
/// @copyright Copyright (c) 2023
///

#ifndef ASW_DRAW_H
#define ASW_DRAW_H

#include "./color.h"
#include "./geometry.h"
#include "./types.h"

namespace asw::draw {

/// @brief Clear the screen to a color.
///
/// @param color The color to clear the screen to.
///
void clear_color(asw::Color color);

/// @brief Draw a sprite.
///
/// @param tex The texture to draw.
/// @param position The position to draw the sprite at.
///
void sprite(const asw::Texture& tex, const asw::Vec2<float>& position);

/// @brief Draw a sprite with the option to flip it.
///
/// @param tex The texture to draw.
/// @param position The position to draw the sprite at.
/// @param flip_x Whether or not to flip the sprite on the x axis.
/// @param flip_y Whether or not to flip the sprite on the y axis.
///
void sprite_flip(
    const asw::Texture& tex, const asw::Vec2<float>& position, bool flip_x, bool flip_y);

/// @brief Draw a sprite with the option to stretch it.
///
/// @param tex The texture to draw.
/// @param position The quad defining the position and size to stretch the
/// sprite to.
///
void stretch_sprite(const asw::Texture& tex, const asw::Quad<float>& position);

/// @brief Draw a sprite with the option to rotate it.
///
/// @param tex The texture to draw.
/// @param position The position to draw the sprite at.
/// @param angle The angle to rotate the sprite by in radians.
///
void rotate_sprite(const asw::Texture& tex, const asw::Vec2<float>& position, float angle);

/// @brief Draw a sprite with the option to stretch a portion of it.
///
/// @param tex The texture to draw.
/// @param source The quad defining the portion of the texture to stretch.
/// @param dest The quad defining the position and size to stretch the sprite
/// to.
///
void stretch_sprite_blit(
    const asw::Texture& tex, const asw::Quad<float>& source, const asw::Quad<float>& dest);

/// @brief Draw a sprite with the option to stretch and rotate a portion of
/// it.
///
/// @param tex The texture to draw.
/// @param source The quad defining the portion of the texture to stretch.
/// @param dest The quad defining the position and size to stretch the sprite
/// to.
/// @param angle The angle to rotate the sprite by in radians.
///
void stretch_sprite_rotate_blit(const asw::Texture& tex, const asw::Quad<float>& source,
    const asw::Quad<float>& dest, float angle);

/// @brief Draw text
///
/// @param font The font to use.
/// @param text The text to draw.
/// @param position The position to draw the text at.
/// @param color The color to draw the text.
/// @param justify The justification of the text.
///
void text(const asw::Font& font, const std::string& text, const asw::Vec2<float>& position,
    asw::Color color, asw::TextJustify justify = asw::TextJustify::Left);

/// @brief Draw a point.
///
/// @param position The position of the point.
/// @param color The color of the point.
///
void point(const asw::Vec2<float>& position, asw::Color color);

/// @brief Draw a line.
///
/// @param position1 The starting position of the line.
/// @param position2 The ending position of the line.
/// @param color The color of the line.
///
void line(const asw::Vec2<float>& position1, const asw::Vec2<float>& position2, asw::Color color);

/// @brief Draw a rectangle.
///
/// @param position The quad defining the position and size of the rectangle.
/// @param color The color of the rectangle.
///
void rect(const asw::Quad<float>& position, asw::Color color);

/// @brief Draw a filled rectangle.
///
/// @param position The quad defining the position and size of the rectangle.
/// @param color The color of the rectangle.
///
void rect_fill(const asw::Quad<float>& position, asw::Color color);

/// @brief Draw a circle.
///
/// @param position The position of the center of the circle.
/// @param radius The radius of the circle.
/// @param color The color of the circle.
///
void circle(const asw::Vec2<float>& position, float radius, asw::Color color);

/// @brief Draw a filled circle.
///
/// @param position The position of the center of the circle.
/// @param radius The radius of the circle.
/// @param color The color of the circle.
///
void circle_fill(const asw::Vec2<float>& position, float radius, asw::Color color);

/// @brief Set the blend mode of a texture.
///
/// @param texture The texture to set the blend mode of.
/// @param mode The blend mode to set.
///
void set_blend_mode(const asw::Texture& texture, asw::BlendMode mode);

/// @brief Set the alpha of a texture.
///
/// @param texture The texture to set the alpha of.
/// @param alpha The alpha to set.
///
void set_alpha(const asw::Texture& texture, float alpha);
} // namespace asw::draw

#endif // ASW_DRAW_H
