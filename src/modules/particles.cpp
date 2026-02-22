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

ParticleEmitter::ParticleEmitter(const ParticleConfig& config, uint32_t maxParticles)
    : config(config)
    , particles(maxParticles)
{
}

void ParticleEmitter::set_emission_rate(float rate)
{
    emission_rate = rate;
}

void ParticleEmitter::emit(uint32_t count)
{
    for (uint32_t i = 0; i < count; ++i) {
        spawn_particle();
    }
}

void ParticleEmitter::start()
{
    emitting = true;
}

void ParticleEmitter::stop()
{
    emitting = false;
}

void ParticleEmitter::update(float dt)
{
    // Auto-emit
    if (emitting && emission_rate > 0.0F) {
        emission_accumulator += emission_rate * dt;
        while (emission_accumulator >= 1.0F) {
            spawn_particle();
            emission_accumulator -= 1.0F;
        }
    }

    // Update alive particles (swap-and-shrink)
    for (uint32_t i = 0; i < alive_count;) {
        auto& p = particles[i];
        p.age += dt;

        if (p.age >= p.lifetime) {
            particles[i] = particles[alive_count - 1];
            alive_count--;
            continue;
        }

        p.velocity += config.gravity * dt;
        p.position += p.velocity * dt;
        ++i;
    }
}

void ParticleEmitter::draw()
{
    for (uint32_t i = 0; i < alive_count; ++i) {
        const auto& p = particles[i];
        const float t = p.age / p.lifetime;

        // Interpolate visuals
        const float size = util::lerp(config.size_start, config.size_end, t);
        const float alpha = util::lerp(config.alpha_start, config.alpha_end, t);

        if (config.texture != nullptr) {
            const auto dest = Quad<float>(
                p.position.x - (size / 2.0F), p.position.y - (size / 2.0F), size, size);

            draw::set_alpha(config.texture, alpha);
            draw::stretch_sprite(config.texture, dest);
            draw::set_alpha(config.texture, 1.0F);
        } else {
            auto r = static_cast<uint8_t>(util::lerp(static_cast<float>(config.color_start.r),
                static_cast<float>(config.color_end.r), t));
            auto g = static_cast<uint8_t>(util::lerp(static_cast<float>(config.color_start.g),
                static_cast<float>(config.color_end.g), t));
            auto b = static_cast<uint8_t>(util::lerp(static_cast<float>(config.color_start.b),
                static_cast<float>(config.color_end.b), t));
            auto a = static_cast<uint8_t>(util::lerp(static_cast<float>(config.color_start.a),
                                              static_cast<float>(config.color_end.a), t)
                * alpha);

            const Color color { r, g, b, a };

            draw::circle_fill(p.position, size / 2.0F, color);
        }
    }
}

uint32_t ParticleEmitter::get_alive_count() const
{
    return alive_count;
}

void ParticleEmitter::spawn_particle()
{
    if (alive_count >= particles.size()) {
        return;
    }

    auto& p = particles[alive_count];
    p.position = transform.position;
    p.age = 0.0F;
    p.lifetime = random::between(config.lifetime_min, config.lifetime_max);
    p.alive = true;

    const float speed = random::between(config.speed_min, config.speed_max);
    const float angle = random::between(config.angle_min, config.angle_max);
    p.velocity = Vec2<float>(std::cos(angle) * speed, std::sin(angle) * speed);

    p.size = config.size_start;
    p.rotation = 0.0F;

    alive_count++;
}

} // namespace asw
