/// @file util.h
/// @author Allan Legemaate (alegemaate@gmail.com)
/// @brief General utility functions
/// @date 2023-09-20
///
/// @copyright Copyright (c) 2023
///

#ifndef ASW_UTIL_H
#define ASW_UTIL_H

#include <algorithm>
#include <string>

#include "./geometry.h"
#include "./types.h"

namespace asw::util {

  /// @brief Abort program and display error message
  ///
  /// @param message Message to display
  ///
  void abortOnError(const std::string& message);

  /// @brief Make a color from RGB values
  ///
  /// @param r Red value (0-255)
  /// @param g Green value (0-255)
  /// @param b Blue value (0-255)
  /// @return The color
  ///
  asw::Color makeColor(int r, int g, int b);

  /// @brief Make a color from RGBA values
  ///
  /// @param r Red value (0-255)
  /// @param g Green value (0-255)
  /// @param b Blue value (0-255)
  /// @param a Alpha value (0-255)
  /// @return The color
  ///
  asw::Color makeColor(int r, int g, int b, int a);

  /// @brief Get texture size
  ///
  /// @param tex Texture to get size of
  /// @return Size as Vec2
  ///
  asw::Vec2<float> getTextureSize(const asw::Texture& tex);

  /// @brief Get text size
  ///
  /// @param font Font to use
  /// @param text Text to get size of
  /// @return Size as Vec2
  ///
  asw::Vec2<int> getTextSize(const asw::Font& font, const std::string& text);

  /// @brief Lerp between two values
  ///
  /// @param a Start value
  /// @param b End value
  /// @param t Interpolation value (0-1)
  /// @return Interpolated value
  ///
  template <typename T>
  T lerp(const T& a, const T& b, float t) {
    t = std::clamp(t, 0.0F, 1.0F);

    return a + (b - a) * t;
  }

}  // namespace asw::util

#endif  // ASW_UTIL_H