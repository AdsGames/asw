/// @file particles.cpp
/// @author Allan Legemaate (alegemaate@gmail.com)
/// @brief Particle emitter implementation
/// @date 2025-03-26
///
/// @copyright Copyright (c) 2025
///

#include "asw/modules/particles.h"

#include <cmath>

#include "asw/modules/draw.h"
#include "asw/modules/random.h"
#include "asw/modules/util.h"

namespace asw {

  ParticleEmitter::ParticleEmitter(const ParticleConfig& config,
                                   int maxParticles)
      : config(config), particles(maxParticles) {}

  void ParticleEmitter::setEmissionRate(float rate) {
    emissionRate = rate;
  }

  void ParticleEmitter::emit(int count) {
    for (int i = 0; i < count; ++i) {
      spawnParticle();
    }
  }

  void ParticleEmitter::start() {
    emitting = true;
  }

  void ParticleEmitter::stop() {
    emitting = false;
  }

  void ParticleEmitter::update(float deltaTime) {
    // Auto-emit
    if (emitting && emissionRate > 0.0F) {
      emissionAccumulator += emissionRate * deltaTime;
      while (emissionAccumulator >= 1.0F) {
        spawnParticle();
        emissionAccumulator -= 1.0F;
      }
    }

    // Update alive particles (swap-and-shrink)
    for (int i = 0; i < aliveCount;) {
      auto& p = particles[i];
      p.age += deltaTime;

      if (p.age >= p.lifetime) {
        particles[i] = particles[aliveCount - 1];
        aliveCount--;
        continue;
      }

      p.velocity += config.gravity * deltaTime;
      p.position += p.velocity * deltaTime;
      ++i;
    }
  }

  void ParticleEmitter::draw() {
    for (int i = 0; i < aliveCount; ++i) {
      const auto& p = particles[i];
      float t = p.age / p.lifetime;

      // Interpolate visuals
      float size = util::lerp(config.sizeStart, config.sizeEnd, t);
      float alpha = util::lerp(config.alphaStart, config.alphaEnd, t);

      auto r = static_cast<Uint8>(
          util::lerp(static_cast<float>(config.colorStart.r),
                     static_cast<float>(config.colorEnd.r), t));
      auto g = static_cast<Uint8>(
          util::lerp(static_cast<float>(config.colorStart.g),
                     static_cast<float>(config.colorEnd.g), t));
      auto b = static_cast<Uint8>(
          util::lerp(static_cast<float>(config.colorStart.b),
                     static_cast<float>(config.colorEnd.b), t));
      auto a = static_cast<Uint8>(
          util::lerp(static_cast<float>(config.colorStart.a),
                     static_cast<float>(config.colorEnd.a), t) *
          alpha);

      const Color color{r, g, b, a};

      if (config.texture != nullptr) {
        const Quad<float> dest(p.position.x - size / 2.0F,
                               p.position.y - size / 2.0F, size, size);

        draw::setAlpha(config.texture, alpha);
        draw::stretchSprite(config.texture, dest);
        draw::setAlpha(config.texture, 1.0F);
      } else {
        draw::circleFill(p.position, size / 2.0F, color);
      }
    }
  }

  int ParticleEmitter::getAliveCount() const {
    return aliveCount;
  }

  void ParticleEmitter::spawnParticle() {
    if (aliveCount >= static_cast<int>(particles.size())) {
      return;
    }

    auto& p = particles[aliveCount];
    p.position = transform.position;
    p.age = 0.0F;
    p.lifetime = random::between(config.lifetimeMin, config.lifetimeMax);
    p.alive = true;

    const float speed = random::between(config.speedMin, config.speedMax);
    const float angle = random::between(config.angleMin, config.angleMax);
    p.velocity = Vec2<float>(std::cos(angle) * speed, std::sin(angle) * speed);

    p.size = config.sizeStart;
    p.rotation = 0.0F;

    aliveCount++;
  }

}  // namespace asw
