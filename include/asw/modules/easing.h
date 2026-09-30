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
#include <limits>
#include <type_traits>

#include "./types.h"

namespace asw::easing {

namespace detail {

    /// @brief a + (b - a) * f. Integer types are worked in floating point, so a
    /// falling unsigned range does not wrap, and a result past the range of T,
    /// e.g. from an overshooting easing, is clamped to it.
    ///
    /// @param a Start value
    /// @param b End value
    /// @param f Amount of the way from a to b
    /// @return The value f of the way from a to b
    ///
    template <typename T> T mix(const T& a, const T& b, float f)
    {
        if constexpr (std::is_integral_v<T>) {
            const auto lo = static_cast<double>(std::numeric_limits<T>::lowest());
            const auto hi = static_cast<double>(std::numeric_limits<T>::max());
            const double v = static_cast<double>(a)
                + ((static_cast<double>(b) - static_cast<double>(a)) * static_cast<double>(f));
            if (v <= lo) {
                return std::numeric_limits<T>::lowest();
            }
            if (v >= hi) {
                return std::numeric_limits<T>::max();
            }
            return static_cast<T>(v);
        } else {
            return a + (b - a) * f;
        }
    }

} // namespace detail

// --- Linear ---
float linear(float t);

// --- Smoothstep ---

/// Soft at both ends, 3t^2 - 2t^3
float smoothstep(float t);

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

/// Get how far a gradient or light has faded at a distance from its centre
/// @param t Distance from the centre, from 0 at the centre to 1 at the edge
/// @param mode How it fades
/// @return How far it has faded, from 0 at the centre to 1 at the edge
float falloff(float t, asw::Falloff mode);

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

    return detail::mix(a, b, func(std::clamp(t, 0.0F, 1.0F)));
}

} // namespace asw::easing

#endif // ASW_EASING_H