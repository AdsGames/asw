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
    // Lifetime in seconds
    float lifetime_min { 1.0F };
    float lifetime_max { 2.0F };

    // Speed in pixels per second
    float speed_min { 50.0F };
    float speed_max { 100.0F };

    // Direction (radians)
    float angle_min { 0.0F };
    float angle_max { 6.2832F }; // Full circle

    // Visual
    Color color_start { 255, 255, 255, 255 };
    Color color_end { 255, 255, 255, 0 };
    float alpha_start { 1.0F };
    float alpha_end { 0.0F };
    float size_start { 4.0F };
    float size_end { 1.0F };

    // Physics in pixels per second squared
    Vec2<float> gravity { 0.0F, 0.0F };

    // Optional texture (nullptr = use circleFill)
    Texture texture { nullptr };
};

/// @brief Particle emitter that integrates with the scene system.
///
class ParticleEmitter : public game::GameObject {
public:
    /// @brief Create a default ParticleEmitter with no particles.
    ///
    ParticleEmitter() = default;

    /// @brief Construct a new ParticleEmitter.
    ///
    /// @param config The particle configuration.
    /// @param maxParticles Maximum number of particles in the pool.
    ///
    explicit ParticleEmitter(const ParticleConfig& config, uint32_t maxParticles = 256);

    /// @brief Set emission rate (particles per second), 0 to disable auto-emit.
    ///
    /// @param rate Particles per second.
    ///
    void set_emission_rate(float rate);

    /// @brief Emit a burst of particles at current position.
    ///
    /// @param count Number of particles to emit.
    ///
    void emit(uint32_t count);

    /// @brief Start continuous emission.
    ///
    void start();

    /// @brief Stop continuous emission.
    ///
    void stop();

    /// @brief Update particles.
    ///
    /// @param dt The time in seconds since the last update.
    ///
    void update(float dt) override;

    /// @brief Draw alive particles.
    ///
    void draw() override;

    /// @brief Get the number of alive particles.
    ///
    /// @return The alive particle count.
    ///
    uint32_t get_alive_count() const;

private:
    struct Particle {
        Vec2<float> position;
        Vec2<float> velocity;
        float lifetime { 0.0F };
        float age { 0.0F };
        float size { 0.0F };
        float rotation { 0.0F };
        bool alive { false };
    };

    void spawn_particle();

    ParticleConfig config;
    std::vector<Particle> particles;
    uint32_t alive_count { 0 };

    // Emission rate in particles per second
    float emission_rate { 0.0F };
    float emission_accumulator { 0.0F };
    bool emitting { false };
};

} // namespace asw

#endif // ASW_PARTICLES_H
