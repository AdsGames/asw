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