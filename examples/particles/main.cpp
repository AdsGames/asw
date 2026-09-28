/// @file main.cpp
/// @brief Particle effects example
///
/// Demonstrates:
///   - asw::ParticleConfig lifetime, speed, angle, colour, size and gravity
///   - asw::ParticleEmitter continuous emission (set_emission_rate, start, stop)
///   - asw::ParticleEmitter bursts (emit)
///   - Textured particles from asw::assets::create_radial_gradient()
///   - Additive blending with asw::draw::set_blend_mode() and set_tint()
///
/// Controls:
///   1-4 - toggle the fire, fountain, smoke and snow emitters
///   Mouse - the glowing trail follows the cursor
///   Left click - firework burst at the cursor
///   Escape - quit
///
/// Scripted run:
///   ASW_EXAMPLE_AUTORUN=1 SDL_VIDEO_DRIVER=dummy ./example_particles
///   Moves the mouse, fires two bursts, saves autorun.png and quits.

#include <asw/asw.h>
#include <cmath>
#include <cstdlib>
#include <numbers>
#include <vector>

namespace {
constexpr float PI = std::numbers::pi_v<float>;
constexpr float UP = -PI / 2.0F;

asw::ParticleConfig fire_config(const asw::Texture& glow)
{
    asw::ParticleConfig c;
    c.lifetime_min = 0.4F;
    c.lifetime_max = 0.9F;
    c.speed_min = 60.0F;
    c.speed_max = 140.0F;
    c.angle_min = UP - 0.35F;
    c.angle_max = UP + 0.35F;
    c.size_start = 48.0F;
    c.size_end = 8.0F;
    c.alpha_start = 0.8F;
    c.alpha_end = 0.0F;
    c.gravity = asw::Vec2<float>(0.0F, -120.0F);
    c.texture = glow;
    return c;
}

asw::ParticleConfig fountain_config()
{
    asw::ParticleConfig c;
    c.lifetime_min = 1.2F;
    c.lifetime_max = 1.8F;
    c.speed_min = 300.0F;
    c.speed_max = 380.0F;
    c.angle_min = UP - 0.2F;
    c.angle_max = UP + 0.2F;
    c.color_start = asw::color::lightskyblue;
    c.color_end = asw::color::royalblue;
    c.size_start = 6.0F;
    c.size_end = 3.0F;
    c.gravity = asw::Vec2<float>(0.0F, 500.0F);
    return c;
}

asw::ParticleConfig smoke_config()
{
    asw::ParticleConfig c;
    c.lifetime_min = 2.0F;
    c.lifetime_max = 3.5F;
    c.speed_min = 20.0F;
    c.speed_max = 50.0F;
    c.angle_min = UP - 0.5F;
    c.angle_max = UP + 0.5F;
    c.color_start = asw::color::gray.with_alpha(160);
    c.color_end = asw::color::darkslategray.with_alpha(0);
    c.size_start = 10.0F;
    c.size_end = 60.0F;
    c.gravity = asw::Vec2<float>(15.0F, -10.0F);
    return c;
}

asw::ParticleConfig snow_config()
{
    asw::ParticleConfig c;
    c.lifetime_min = 4.0F;
    c.lifetime_max = 6.0F;
    c.speed_min = 20.0F;
    c.speed_max = 60.0F;
    c.angle_min = PI / 2.0F - 0.4F;
    c.angle_max = PI / 2.0F + 0.4F;
    c.color_start = asw::color::white;
    c.color_end = asw::color::lightsteelblue;
    c.alpha_start = 1.0F;
    c.alpha_end = 0.3F;
    c.size_start = 5.0F;
    c.size_end = 2.0F;
    return c;
}

asw::ParticleConfig trail_config(const asw::Texture& glow)
{
    asw::ParticleConfig c;
    c.lifetime_min = 0.3F;
    c.lifetime_max = 0.6F;
    c.speed_min = 5.0F;
    c.speed_max = 30.0F;
    c.size_start = 28.0F;
    c.size_end = 2.0F;
    c.alpha_start = 0.7F;
    c.texture = glow;
    return c;
}

asw::ParticleConfig firework_config(const asw::Color& color)
{
    asw::ParticleConfig c;
    c.lifetime_min = 0.8F;
    c.lifetime_max = 1.4F;
    c.speed_min = 80.0F;
    c.speed_max = 300.0F;
    c.color_start = color.lighten(0.6F);
    c.color_end = color.with_alpha(0);
    c.size_start = 5.0F;
    c.size_end = 1.0F;
    c.gravity = asw::Vec2<float>(0.0F, 180.0F);
    return c;
}

// Additive glow texture. White, so set_tint picks the final colour.
asw::Texture make_glow(asw::Color tint)
{
    auto tex = asw::assets::create_radial_gradient(
        64, asw::color::white, asw::color::white.with_alpha(0));
    asw::draw::set_blend_mode(tex, asw::BlendMode::Add);
    asw::draw::set_tint(tex, tint);
    return tex;
}

struct Toggle {
    asw::ParticleEmitter* emitter;
    asw::input::Key key;
    bool on;
};
} // namespace

int main()
{
    asw::core::init(800, 600);
    asw::display::set_title("ASW Example - Particles");

    const bool autorun = std::getenv("ASW_EXAMPLE_AUTORUN") != nullptr;

    const auto fire_glow = make_glow(asw::color::orangered);
    const auto trail_glow = make_glow(asw::color::mediumorchid);

    asw::ParticleEmitter fire(fire_config(fire_glow), 256);
    fire.transform.position = asw::Vec2<float>(130.0F, 520.0F);
    fire.set_emission_rate(120.0F);

    asw::ParticleEmitter fountain(fountain_config(), 512);
    fountain.transform.position = asw::Vec2<float>(330.0F, 560.0F);
    fountain.set_emission_rate(200.0F);

    asw::ParticleEmitter smoke(smoke_config(), 256);
    smoke.transform.position = asw::Vec2<float>(530.0F, 540.0F);
    smoke.set_emission_rate(25.0F);

    asw::ParticleEmitter snow(snow_config(), 512);
    snow.set_emission_rate(60.0F);

    asw::ParticleEmitter trail(trail_config(trail_glow), 256);
    trail.set_emission_rate(90.0F);
    trail.start();

    std::vector<Toggle> toggles = {
        { &fire, asw::input::Key::Num1, true },
        { &fountain, asw::input::Key::Num2, true },
        { &smoke, asw::input::Key::Num3, true },
        { &snow, asw::input::Key::Num4, true },
    };
    for (auto& t : toggles) {
        t.emitter->start();
    }

    // Fireworks are bursts only, one emitter per colour
    const std::vector<asw::Color> firework_colors
        = { asw::color::gold, asw::color::hotpink, asw::color::springgreen, asw::color::cyan };
    std::vector<asw::ParticleEmitter> fireworks;
    for (const auto& color : firework_colors) {
        fireworks.emplace_back(firework_config(color), 256);
    }
    std::size_t next_firework = 0;

    int frame = 0;

    asw::core::run([&]() {
        if (autorun) {
            const float t = static_cast<float>(frame) / 60.0F;
            asw::input::simulate_mouse_move(
                { 400.0F + std::cos(t * 2.0F) * 200.0F, 250.0F + std::sin(t * 3.0F) * 100.0F });

            if (frame == 60 || frame == 100) {
                asw::input::simulate_mouse_button_down(asw::input::MouseButton::Left);
            }
            if (frame == 61 || frame == 101) {
                asw::input::simulate_mouse_button_up(asw::input::MouseButton::Left);
            }
        }

        asw::core::update();

        // Real frame time, but a fixed step for scripted runs so they repeat
        const float dt = autorun ? 1.0F / 60.0F : asw::core::get_delta_time();

        if (asw::input::get_key_down(asw::input::Key::Escape)) {
            asw::core::exit();
        }

        for (auto& t : toggles) {
            if (asw::input::get_key_down(t.key)) {
                t.on = !t.on;
                t.on ? t.emitter->start() : t.emitter->stop();
            }
        }

        const auto mouse = asw::input::get_mouse().position;
        trail.transform.position = mouse;

        if (asw::input::get_mouse_button_down(asw::input::MouseButton::Left)) {
            auto& burst = fireworks[next_firework];
            burst.transform.position = mouse;
            burst.emit(120);
            next_firework = (next_firework + 1) % fireworks.size();
        }

        // Snow falls from a random point along the top edge
        snow.transform.position = asw::Vec2<float>(asw::random::between(0.0F, 800.0F), -10.0F);

        fire.update(dt);
        fountain.update(dt);
        smoke.update(dt);
        snow.update(dt);
        trail.update(dt);
        for (auto& f : fireworks) {
            f.update(dt);
        }

        // Draw
        asw::display::clear(asw::Color(16, 18, 32));

        // Ground and emitter bases
        asw::draw::rect_fill({ 0.0F, 560.0F, 800.0F, 40.0F }, asw::Color(30, 34, 50));
        asw::draw::rect_fill({ 110.0F, 520.0F, 40.0F, 40.0F }, asw::color::saddlebrown);
        asw::draw::rect_fill({ 300.0F, 550.0F, 60.0F, 10.0F }, asw::color::slategray);
        asw::draw::rect_fill({ 515.0F, 540.0F, 30.0F, 20.0F }, asw::color::dimgray);

        smoke.draw();
        fire.draw();
        fountain.draw();
        snow.draw();
        for (auto& f : fireworks) {
            f.draw();
        }
        trail.draw();

        // One lamp per emitter toggle
        for (std::size_t i = 0; i < toggles.size(); ++i) {
            const asw::Vec2<float> pos(20.0F + static_cast<float>(i) * 24.0F, 20.0F);
            if (toggles[i].on) {
                asw::draw::circle_fill(pos, 7.0F, asw::color::lime);
            }
            asw::draw::circle(pos, 7.0F, asw::color::white);
        }

        if (autorun && frame == 150) {
            const bool saved = asw::display::screenshot("autorun.png");
            asw::log::info(saved ? "Saved autorun.png" : "Screenshot failed");
            asw::log::info("Fountain particles: " + std::to_string(fountain.get_alive_count()));
            asw::core::exit();
        }

        asw::display::present();
        frame++;
    });

    asw::core::shutdown();
    return 0;
}
