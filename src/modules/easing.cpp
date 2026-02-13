#include "./asw/modules/easing.h"

// Linear
float asw::easing::linear(float t) {
  return t;
}

// Quadratic
float asw::easing::easeInQuad(float t) {
  return t * t;
}

float asw::easing::easeOutQuad(float t) {
  return t * (2.0F - t);
}

float asw::easing::easeInOutQuad(float t) {
  return t < 0.5F ? 2.0F * t * t : -1.0F + (4.0F - (2.0F * t)) * t;
}

// Cubic
float asw::easing::easeInCubic(float t) {
  return t * t * t;
}

float asw::easing::easeOutCubic(float t) {
  const float u = t - 1.0F;
  return (u * u * u) + 1.0F;
}

float asw::easing::easeInOutCubic(float t) {
  return t < 0.5F ? 4.0F * t * t * t
                  : (t - 1.0F) * (2.0F * t - 2.0F) * (2.0F * t - 2.0F) + 1.0F;
}

// Sine
float asw::easing::easeInSine(float t) {
  return 1.0F - std::cosf(t * M_PI * 0.5F);
}

float asw::easing::easeOutSine(float t) {
  return std::sinf(t * M_PI * 0.5F);
}

float asw::easing::easeInOutSine(float t) {
  return 0.5F * (1.0F - std::cosf(M_PI * t));
}

// Exponential
float asw::easing::easeInExpo(float t) {
  return t == 0.0F ? 0.0F : std::pow(2.0F, 10.0F * (t - 1.0F));
}

float asw::easing::easeOutExpo(float t) {
  return t == 1.0F ? 1.0F : 1.0F - std::pow(2.0F, -10.0F * t);
}

float asw::easing::easeInOutExpo(float t) {
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
float asw::easing::easeInElastic(float t) {
  if (t == 0.0F || t == 1.0F) {
    return t;
  }

  return -std::pow(2.0F, 10.0F * (t - 1.0F)) *
         std::sin((t - 1.075F) * (2.0F * M_PI) / 0.3F);
}

float asw::easing::easeOutElastic(float t) {
  if (t == 0.0F || t == 1.0F) {
    return t;
  }

  return std::pow(2.0F, -10.0F * t) *
             std::sin((t - 0.075F) * (2.0F * M_PI) / 0.3F) +
         1.0F;
}

// Bounce
float asw::easing::easeOutBounce(float t) {
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

float asw::easing::easeInBounce(float t) {
  return 1.0F - easeOutBounce(1.0F - t);
}

// Back (overshoot)
float asw::easing::easeInBack(float t) {
  constexpr float s = 1.70158F;
  return t * t * ((s + 1.0F) * t - s);
}

float asw::easing::easeOutBack(float t) {
  constexpr float s = 1.70158F;
  const float u = t - 1.0F;
  return u * u * ((s + 1.0F) * u + s) + 1.0F;
}