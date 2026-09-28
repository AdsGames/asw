/// @file main.cpp
/// @brief Render targets, blend modes and sprite drawing example
///
/// Demonstrates:
///   - asw::assets::create_texture() and asw::display::set_render_target()
///     to draw textures at runtime
///   - asw::draw::set_scale_mode() Nearest vs Linear on the same art
///   - asw::draw::rotate_sprite(), sprite_flip(), stretch_sprite()
///   - asw::draw::stretch_sprite_rotate() to scale, rotate and flip at once
///   - asw::draw::set_tint() and set_alpha()
///   - A light map: radial gradients drawn with BlendMode::Add into a
///     texture, then drawn over the scene with BlendMode::Modulate
///
/// Controls:
///   Mouse - move the torch
///   Left click - drop a coloured light
///   L - toggle lighting
///   1 / 2 / 3 - dusk, night and pitch black
///   Escape - quit
///
/// Scripted run:
///   ASW_EXAMPLE_AUTORUN=1 SDL_VIDEO_DRIVER=dummy ./example_lighting
///   Drops two lights, moves the torch, saves autorun.png and quits.

#include <array>
#include <asw/asw.h>
#include <cmath>
#include <cstdlib>
#include <numbers>
#include <vector>

namespace {
constexpr int SCREEN_W = 800;
constexpr int SCREEN_H = 600;
constexpr std::size_t MAX_LIGHTS = 8;

struct Light {
    asw::Vec2<float> position;
    asw::Color color;
    float radius;
};

// A 16x16 critter drawn pixel by pixel into a texture
asw::Texture make_critter()
{
    constexpr std::array<const char*, 16> ART = {
        "................",
        "....########....",
        "...##########...",
        "..############..",
        "..##..####..##..",
        "..##..####..##..",
        "..############..",
        "..############..",
        "..###.####.###..",
        "..####....####..",
        "...##########...",
        "....########....",
        "....##....##....",
        "...###....###...",
        "...##......##...",
        "................",
    };

    auto tex = asw::assets::create_texture(16, 16);
    asw::display::set_render_target(tex);
    asw::display::clear(asw::Color(0, 0, 0, 0));

    for (int y = 0; y < 16; ++y) {
        for (int x = 0; x < 16; ++x) {
            if (ART[static_cast<std::size_t>(y)][x] == '#') {
                asw::draw::point(
                    { static_cast<float>(x), static_cast<float>(y) }, asw::color::white);
            }
        }
    }

    asw::display::reset_render_target();
    return tex;
}

// The level is drawn once into a texture and reused every frame
asw::Texture make_floor()
{
    auto tex = asw::assets::create_texture(SCREEN_W, SCREEN_H);
    asw::display::set_render_target(tex);
    asw::display::clear(asw::Color(60, 52, 44));

    constexpr float TILE = 40.0F;
    for (int y = 0; y < SCREEN_H / 40; ++y) {
        for (int x = 0; x < SCREEN_W / 40; ++x) {
            const asw::Quad<float> tile(
                static_cast<float>(x) * TILE, static_cast<float>(y) * TILE, TILE, TILE);
            const auto shade = asw::random::between(0.0F, 0.15F);
            asw::draw::rect_fill(tile, asw::Color(120, 104, 88).darken(shade));
            asw::draw::rect(tile, asw::Color(80, 70, 60));
        }
    }

    // Some pillars
    for (int i = 0; i < 5; ++i) {
        const asw::Quad<float> pillar(
            80.0F + static_cast<float>(i) * 160.0F, 440.0F, 40.0F, 120.0F);
        asw::draw::rect_fill(pillar, asw::color::slategray);
        asw::draw::rect(pillar, asw::color::lightslategray);
    }

    asw::display::reset_render_target();
    return tex;
}

// White glow so each light can tint it to its own colour
asw::Texture make_glow()
{
    auto tex = asw::assets::create_radial_gradient(256, asw::color::white, asw::color::black);
    asw::draw::set_blend_mode(tex, asw::BlendMode::Add);
    return tex;
}

asw::Color rainbow(float t)
{
    constexpr float THIRD = std::numbers::pi_v<float> * 2.0F / 3.0F;
    auto channel = [](float v) { return static_cast<uint8_t>(127.5F + 127.5F * std::sin(v)); };
    return { channel(t), channel(t + THIRD), channel(t + THIRD * 2.0F) };
}
} // namespace

int main()
{
    asw::core::init(SCREEN_W, SCREEN_H);
    asw::display::set_title("ASW Example - Lighting");

    const bool autorun = std::getenv("ASW_EXAMPLE_AUTORUN") != nullptr;

    const auto floor = make_floor();
    const auto glow = make_glow();

    // Same art twice, one scaled with hard pixels and one smoothed
    const auto critter_sharp = make_critter();
    const auto critter_smooth = make_critter();
    asw::draw::set_scale_mode(critter_sharp, asw::ScaleMode::Nearest);
    asw::draw::set_scale_mode(critter_smooth, asw::ScaleMode::Linear);

    // Scene brightness is multiplied by the light map
    const auto light_map = asw::assets::create_texture(SCREEN_W, SCREEN_H);
    asw::draw::set_blend_mode(light_map, asw::BlendMode::Modulate);

    const std::vector<asw::Color> ambients
        = { asw::Color(150, 120, 140), asw::Color(40, 44, 80), asw::Color(0, 0, 0) };
    std::size_t ambient = 1;
    bool lighting = true;

    std::vector<Light> lights;
    std::size_t next_light = 0;

    float time = 0.0F;
    int frame = 0;

    asw::core::run([&]() {
        if (autorun) {
            if (frame == 5) {
                asw::input::simulate_mouse_move({ 200.0F, 200.0F });
                asw::input::simulate_mouse_button_down(asw::input::MouseButton::Left);
            }
            if (frame == 10) {
                asw::input::simulate_mouse_move({ 600.0F, 450.0F });
                asw::input::simulate_mouse_button_down(asw::input::MouseButton::Left);
            }
            if (frame == 6 || frame == 11) {
                asw::input::simulate_mouse_button_up(asw::input::MouseButton::Left);
            }
            if (frame == 20) {
                asw::input::simulate_mouse_move({ 420.0F, 320.0F });
            }
        }

        asw::core::update();

        // Real frame time, but a fixed step for scripted runs so they repeat
        const float dt = autorun ? 1.0F / 60.0F : asw::core::get_delta_time();
        time += dt;

        if (asw::input::get_key_down(asw::input::Key::Escape)) {
            asw::core::exit();
        }
        if (asw::input::get_key_down(asw::input::Key::L)) {
            lighting = !lighting;
        }
        if (asw::input::get_key_down(asw::input::Key::Num1)) {
            ambient = 0;
        }
        if (asw::input::get_key_down(asw::input::Key::Num2)) {
            ambient = 1;
        }
        if (asw::input::get_key_down(asw::input::Key::Num3)) {
            ambient = 2;
        }

        const auto mouse = asw::input::get_mouse().position;

        if (asw::input::get_mouse_button_down(asw::input::MouseButton::Left)) {
            const Light light { mouse, rainbow(time * 3.0F), asw::random::between(140.0F, 260.0F) };
            if (lights.size() < MAX_LIGHTS) {
                lights.push_back(light);
            } else {
                lights[next_light] = light;
            }
            next_light = (next_light + 1) % MAX_LIGHTS;
        }

        // --- Scene ---
        asw::display::clear(asw::color::black);
        asw::draw::sprite(floor, { 0.0F, 0.0F });

        // Scale mode: same texture, stretched 6x
        asw::draw::stretch_sprite(critter_sharp, { 120.0F, 120.0F, 96.0F, 96.0F });
        asw::draw::stretch_sprite(critter_smooth, { 260.0F, 120.0F, 96.0F, 96.0F });

        // Tint cycles through colours, alpha pulses
        asw::draw::set_tint(critter_sharp, rainbow(time));
        asw::draw::set_alpha(critter_sharp, 0.6F + 0.4F * std::sin(time * 2.0F));
        asw::draw::stretch_sprite(critter_sharp, { 440.0F, 120.0F, 96.0F, 96.0F });
        asw::draw::set_tint(critter_sharp, asw::color::white);
        asw::draw::set_alpha(critter_sharp, 1.0F);

        // Rotation and flipping at native size, in a row
        for (int i = 0; i < 4; ++i) {
            const asw::Vec2<float> pos(140.0F + static_cast<float>(i) * 40.0F, 300.0F);
            asw::draw::sprite_flip(critter_sharp, pos, (i & 1) != 0, (i & 2) != 0);
        }
        asw::draw::rotate_sprite(critter_sharp, { 440.0F, 300.0F }, time * 2.0F);
        asw::draw::rotate_sprite(critter_smooth, { 500.0F, 300.0F }, -time * 2.0F);

        // Scaled 3x, rocking side to side, and turning to face the way it rocks
        const float rock = std::sin(time * 2.0F) * 0.4F;
        asw::draw::stretch_sprite_rotate(
            critter_sharp, { 580.0F, 276.0F, 48.0F, 48.0F }, rock, rock < 0.0F, false);

        // --- Light map ---
        if (lighting) {
            asw::display::set_render_target(light_map);
            asw::display::clear(ambients[ambient]);

            auto draw_light = [&glow](const Light& light) {
                asw::draw::set_tint(glow, light.color);
                asw::draw::stretch_sprite(glow,
                    { light.position.x - light.radius, light.position.y - light.radius,
                        light.radius * 2.0F, light.radius * 2.0F });
            };

            // Torch flickers a little
            const float flicker
                = 180.0F + std::sin(time * 13.0F) * 6.0F + std::sin(time * 7.0F) * 4.0F;
            draw_light({ mouse, asw::color::navajowhite, flicker });

            for (const auto& light : lights) {
                draw_light(light);
            }

            asw::display::reset_render_target();
            asw::draw::sprite(light_map, { 0.0F, 0.0F });
        }

        // Light positions, drawn after the light map so they stay visible
        for (const auto& light : lights) {
            asw::draw::circle_fill(light.position, 4.0F, light.color);
        }

        if (autorun && frame == 60) {
            const bool saved = asw::display::screenshot("autorun.png");
            asw::log::info(saved ? "Saved autorun.png" : "Screenshot failed");
            asw::log::info("Lights: " + std::to_string(lights.size()));
            asw::core::exit();
        }

        asw::display::present();
        frame++;
    });

    asw::core::shutdown();
    return 0;
}
