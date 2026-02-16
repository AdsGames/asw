/// @file particles.h
/// @author Allan Legemaate (alegemaate@gmail.com)
/// @brief Lightweight particle emitter system
/// @date 2025-03-26
///
/// @copyright Copyright (c) 2025
///

#ifndef ASW_PARTICLES_H
#define ASW_PARTICLES_H

#include <vector>

#include "./color.h"
#include "./game.h"
#include "./geometry.h"
#include "./types.h"

namespace asw {

  /// @brief Configuration for particle emitters.
  ///
  struct ParticleConfig {
    // Lifetime
    float lifetimeMin{1.0F};
    float lifetimeMax{2.0F};

    // Speed
    float speedMin{50.0F};
    float speedMax{100.0F};

    // Direction (radians)
    float angleMin{0.0F};
    float angleMax{6.2832F};  // Full circle

    // Visual
    Color colorStart{255, 255, 255, 255};
    Color colorEnd{255, 255, 255, 0};
    float alphaStart{1.0F};
    float alphaEnd{0.0F};
    float sizeStart{4.0F};
    float sizeEnd{1.0F};

    // Physics
    Vec2<float> gravity{0.0F, 0.0F};

    // Optional texture (nullptr = use circleFill)
    Texture texture{nullptr};
  };

  /// @brief Particle emitter that integrates with the scene system.
  ///
  class ParticleEmitter : public game::GameObject {
   public:
    /// @brief Construct a new ParticleEmitter.
    ///
    /// @param config The particle configuration.
    /// @param maxParticles Maximum number of particles in the pool.
    ///
    explicit ParticleEmitter(const ParticleConfig& config,
                             int maxParticles = 256);

    /// @brief Set emission rate (particles per second), 0 to disable auto-emit.
    ///
    /// @param rate Particles per second.
    ///
    void setEmissionRate(float rate);

    /// @brief Emit a burst of particles at current position.
    ///
    /// @param count Number of particles to emit.
    ///
    void emit(int count);

    /// @brief Start continuous emission.
    ///
    void start();

    /// @brief Stop continuous emission.
    ///
    void stop();

    /// @brief Update particles.
    ///
    /// @param deltaTime The time since the last update.
    ///
    void update(float deltaTime) override;

    /// @brief Draw alive particles.
    ///
    void draw() override;

    /// @brief Get the number of alive particles.
    ///
    /// @return The alive particle count.
    ///
    int getAliveCount() const;

   private:
    struct Particle {
      Vec2<float> position;
      Vec2<float> velocity;
      float lifetime{0.0F};
      float age{0.0F};
      float size{0.0F};
      float rotation{0.0F};
      bool alive{false};
    };

    void spawnParticle();

    ParticleConfig config;
    std::vector<Particle> particles;
    int aliveCount{0};
    float emissionRate{0.0F};
    float emissionAccumulator{0.0F};
    bool emitting{false};
  };

}  // namespace asw

#endif  // ASW_PARTICLES_H
