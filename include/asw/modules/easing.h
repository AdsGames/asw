/// @file easing.h
/// @author Allan Legemaate (alegemaate@gmail.com)
/// @brief Easing functions for animations
/// @date 2026-02-13
///
/// @copyright Copyright (c) 2026
///

#ifndef ASW_DRAW_H
#define ASW_DRAW_H

#include <algorithm>
#include <cmath>

namespace asw::easing {
  /// Type alias for easing functions
  using EaseFunc = float (*)(float);

  // --- Linear ---
  float linear(float t);

  // --- Quadratic ---
  float easeInQuad(float t);
  float easeOutQuad(float t);
  float easeInOutQuad(float t);

  // --- Cubic ---
  float easeInCubic(float t);
  float easeOutCubic(float t);
  float easeInOutCubic(float t);

  // --- Sine ---
  float easeInSine(float t);
  float easeOutSine(float t);
  float easeInOutSine(float t);

  // --- Exponential ---
  float easeInExpo(float t);
  float easeOutExpo(float t);
  float easeInOutExpo(float t);

  // --- Elastic ---
  float easeInElastic(float t);
  float easeOutElastic(float t);

  // --- Bounce ---
  float easeInBounce(float t);
  float easeOutBounce(float t);

  // --- Back (overshoot) ---
  float easeInBack(float t);
  float easeOutBack(float t);

  // --- Convenience ---

  /// Apply an easing function and lerp between two values
  /// @param a Start value
  /// @param b End value
  /// @param t Progress (0-1)
  /// @param func Easing function to apply
  template <typename T>
  T ease(const T& a, const T& b, float t, EaseFunc func) {
    return a + (b - a) * func(std::clamp(t, 0.0F, 1.0F));
  }

}  // namespace asw::easing

#endif  // ASW_EASING_H