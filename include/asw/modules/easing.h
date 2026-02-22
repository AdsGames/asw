/// @file easing.h
/// @author Allan Legemaate (alegemaate@gmail.com)
/// @brief Easing functions for animations
/// @date 2026-02-13
///
/// @copyright Copyright (c) 2026
///

#ifndef ASW_EASING_H
#define ASW_EASING_H

#include <algorithm>
#include <concepts>
#include <functional>

namespace asw::easing {

// --- Linear ---
float linear(float t);

// --- Quadratic ---
float ease_in_quad(float t);
float ease_out_quad(float t);
float ease_in_out_quad(float t);

// --- Cubic ---
float ease_in_cubic(float t);
float ease_out_cubic(float t);
float ease_in_out_cubic(float t);

// --- Sine ---
float ease_in_sine(float t);
float ease_out_sine(float t);
float ease_in_out_sine(float t);

// --- Exponential ---
float ease_in_expo(float t);
float ease_out_expo(float t);
float ease_in_out_expo(float t);

// --- Elastic ---
float ease_in_elastic(float t);
float ease_out_elastic(float t);

// --- Bounce ---
float ease_in_bounce(float t);
float ease_out_bounce(float t);

// --- Back (overshoot) ---
float ease_in_back(float t);
float ease_out_back(float t);

// --- Convenience ---

/// Apply an easing function and lerp between two values
/// @param a Start value
/// @param b End value
/// @param t Progress (0-1)
/// @param func Easing function to apply
template <typename T, typename Func> T ease(const T& a, const T& b, float t, Func func)
{
    static_assert(std::is_arithmetic_v<T>, "T must be an arithmetic type");
    static_assert(std::is_invocable_r_v<float, Func, float>,
        "Func must be a callable that takes a float and returns a float");

    return a + (b - a) * func(std::clamp(t, 0.0F, 1.0F));
}

} // namespace asw::easing

#endif // ASW_EASING_H