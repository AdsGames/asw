#include "./asw/modules/easing.h"

#include <numbers>

// Linear
float asw::easing::linear(float t) {
  return t;
}

// Quadratic
float asw::easing::ease_in_quad(float t) {
  return t * t;
}

float asw::easing::ease_out_quad(float t) {
  return t * (2.0F - t);
}

float asw::easing::ease_in_out_quad(float t) {
  return t < 0.5F ? 2.0F * t * t : -1.0F + ((4.0F - (2.0F * t)) * t);
}

// Cubic
float asw::easing::ease_in_cubic(float t) {
  return t * t * t;
}

float asw::easing::ease_out_cubic(float t) {
  const float u = t - 1.0F;
  return (u * u * u) + 1.0F;
}

float asw::easing::ease_in_out_cubic(float t) {
  return t < 0.5F ? 4.0F * t * t * t
                  : ((t - 1.0F) * (2.0F * t - 2.0F) * (2.0F * t - 2.0F)) + 1.0F;
}

// Sine
float asw::easing::ease_in_sine(float t) {
  return 1.0F - std::cosf(t * std::numbers::pi_v<float> * 0.5F);
}

float asw::easing::ease_out_sine(float t) {
  return std::sinf(t * std::numbers::pi_v<float> * 0.5F);
}

float asw::easing::ease_in_out_sine(float t) {
  return 0.5F * (1.0F - std::cosf(std::numbers::pi_v<float> * t));
}

// Exponential
float asw::easing::ease_in_expo(float t) {
  return t == 0.0F ? 0.0F : std::pow(2.0F, 10.0F * (t - 1.0F));
}

float asw::easing::ease_out_expo(float t) {
  return t == 1.0F ? 1.0F : 1.0F - std::pow(2.0F, -10.0F * t);
}

float asw::easing::ease_in_out_expo(float t) {
  if (t == 0.0F) {
    return 0.0F;
  }

  if (t == 1.0F) {
    return 1.0F;
  }

  if (t < 0.5F) {
    return std::pow(2.0F, (20.0F * t) - 10.0F) * 0.5F;
  }

  return (2.0F - std::pow(2.0F, (-20.0F * t) + 10.0F)) * 0.5F;
}

// Elastic
float asw::easing::ease_in_elastic(float t) {
  if (t == 0.0F || t == 1.0F) {
    return t;
  }

  return -std::pow(2.0F, 10.0F * (t - 1.0F)) *
         std::sin((t - 1.075F) * (2.0F * std::numbers::pi_v<float>) / 0.3F);
}

float asw::easing::ease_out_elastic(float t) {
  if (t == 0.0F || t == 1.0F) {
    return t;
  }

  return (std::pow(2.0F, -10.0F * t) *
          std::sin((t - 0.075F) * (2.0F * std::numbers::pi_v<float>) / 0.3F)) +
         1.0F;
}

// Bounce
float asw::easing::ease_out_bounce(float t) {
  if (t < 1.0F / 2.75F) {
    return 7.5625F * t * t;
  }

  if (t < 2.0F / 2.75F) {
    t -= 1.5F / 2.75F;
    return (7.5625F * t * t) + 0.75F;
  }

  if (t < 2.5F / 2.75F) {
    t -= 2.25F / 2.75F;
    return (7.5625F * t * t) + 0.9375F;
  }

  t -= 2.625F / 2.75F;

  return (7.5625F * t * t) + 0.984375F;
}

float asw::easing::ease_in_bounce(float t) {
  return 1.0F - ease_out_bounce(1.0F - t);
}

// Back (overshoot)
float asw::easing::ease_in_back(float t) {
  constexpr float s = 1.70158F;
  return t * t * ((s + 1.0F) * t - s);
}

float asw::easing::ease_out_back(float t) {
  constexpr float s = 1.70158F;
  const float u = t - 1.0F;
  return (u * u * ((s + 1.0F) * u + s)) + 1.0F;
}