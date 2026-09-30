/// @file main.cpp
/// @brief Lighting module example
///
/// Demonstrates:
///   - asw::lighting::LightMap with a camera, so lights stay put as it scrolls
///   - Point lights and spot lights, with Linear, Smooth and Quadratic falloff
///   - Flicker and pulse on lights
///   - Occluders with Cast and Visibility shadows
///   - add_glow() for sprites that give off light
///   - asw::lighting::AmbientCycle for day and night
///   - asw::lighting::TileLight for light that spreads across a tile grid
///   - asw::geometry::visibility() as a field of view
///   - asw::draw::polygon_fill(), polygon() and triangle_fill()
///
/// Controls:
///   WASD / arrows - move
///   Mouse - aim the flashlight
///   Left click - drop a coloured light
///   F - flashlight (spot light) on or off
///   1 / 2 / 3 - shadows off, cast, visibility
///   4 - next falloff
///   Q - flicker and pulse on or off
///   E - glows on or off
///   N - day and night cycle on or off
///   T - tile lighting on or off
///   V - show field of view
///   L - lighting on or off
///   Escape - quit
///
/// Scripted run:
///   ASW_EXAMPLE_AUTORUN=1 SDL_VIDEO_DRIVER=dummy ./example_advanced_lighting
///   Turns on cast shadows and the flashlight, walks right, saves
///   autorun.png and quits.

#include <array>
#include <asw/asw.h>
#include <cmath>
#include <cstdlib>
#include <numbers>
#include <string>
#include <vector>

namespace {
constexpr float SCREEN_W = 800.0F;
constexpr float SCREEN_H = 600.0F;
constexpr float WORLD_W = 1600.0F;
constexpr float WORLD_H = 1200.0F;
constexpr float PLAYER_SIZE = 20.0F;
constexpr float PLAYER_SPEED = 240.0F;
constexpr std::size_t MAX_DROPPED = 8;

// Tile lighting area
constexpr int GRID_W = 14;
constexpr int GRID_H = 12;
constexpr float TILE = 40.0F;
const asw::Vec2<float> GRID_ORIGIN(1000.0F, 80.0F);

// Solid tiles, one row per string
constexpr std::array<const char*, GRID_H> GRID_MAP { {
    "##############",
    "#............#",
    "#..##....##..#",
    "#..#......#..#",
    "#............#",
    "#.....##.....#",
    "#.....##.....#",
    "#............#",
    "#..#......#..#",
    "#..##....##..#",
    "#............#",
    "######..######",
} };

struct Torch {
    asw::Vec2<float> position;
    asw::Color color;
    bool pulses;
};

asw::Texture make_blob(int size, asw::Color color)
{
    auto tex = asw::assets::create_texture(size, size);
    asw::display::set_render_target(tex);
    asw::display::clear(asw::Color(0, 0, 0, 0));
    const float half = static_cast<float>(size) / 2.0F;
    asw::draw::circle_fill({ half, half }, half - 1.0F, color);
    asw::draw::circle_fill({ half, half }, half * 0.5F, color.lighten(0.5F));
    asw::display::reset_render_target();
    return tex;
}

asw::Color rainbow(float t)
{
    constexpr float THIRD = std::numbers::pi_v<float> * 2.0F / 3.0F;
    auto channel = [](float v) { return static_cast<uint8_t>(127.5F + 127.5F * std::sin(v)); };
    return { channel(t), channel(t + THIRD), channel(t + THIRD * 2.0F) };
}

const char* shadow_name(asw::lighting::ShadowMode mode)
{
    switch (mode) {
    case asw::lighting::ShadowMode::Cast:
        return "cast";
    case asw::lighting::ShadowMode::Visibility:
        return "visibility";
    default:
        return "off";
    }
}

const char* falloff_name(asw::Falloff falloff)
{
    switch (falloff) {
    case asw::Falloff::Linear:
        return "linear";
    case asw::Falloff::Quadratic:
        return "quadratic";
    default:
        return "smooth";
    }
}

const char* on_off(bool value)
{
    return value ? "on" : "off";
}
} // namespace

int main()
{
    asw::core::init(static_cast<int>(SCREEN_W), static_cast<int>(SCREEN_H));
    asw::display::set_title("ASW Example - Lights");

    const bool autorun = std::getenv("ASW_EXAMPLE_AUTORUN") != nullptr;
    const auto font = asw::assets::load_font("assets/font.ttf", 8.0F, asw::FontStyle::Pixel);

    asw::Camera camera(asw::Vec2<float>(SCREEN_W, SCREEN_H));
    camera.set_bounds(asw::Quad<float>(0.0F, 0.0F, WORLD_W, WORLD_H));

    asw::Quad<float> player(200.0F, 200.0F, PLAYER_SIZE, PLAYER_SIZE);
    camera.snap_to(player.get_center());

    // --- Occluders ---
    const std::vector<asw::Quad<float>> walls = {
        { 300.0F, 300.0F, 80.0F, 200.0F },
        { 560.0F, 150.0F, 200.0F, 60.0F },
        { 500.0F, 700.0F, 60.0F, 300.0F },
        { 150.0F, 900.0F, 250.0F, 60.0F },
        { 780.0F, 480.0F, 120.0F, 120.0F },
    };
    const std::vector<asw::Polygonf> shapes = {
        { { 1100.0F, 800.0F }, { 1250.0F, 1000.0F }, { 980.0F, 1000.0F } },
        { { 650.0F, 1000.0F }, { 850.0F, 1000.0F }, { 850.0F, 1050.0F }, { 700.0F, 1050.0F },
            { 700.0F, 1150.0F }, { 650.0F, 1150.0F } },
    };

    asw::lighting::LightMap light_map;
    light_map.set_camera(&camera);
    light_map.set_shadow_mode(asw::lighting::ShadowMode::Cast);
    for (const auto& wall : walls) {
        light_map.add_occluder(wall);
    }
    for (const auto& shape : shapes) {
        light_map.add_occluder(shape);
    }

    // --- Lights ---
    const std::vector<Torch> torches = {
        { { 450.0F, 400.0F }, asw::color::orange, false },
        { { 660.0F, 260.0F }, asw::color::orange, false },
        { { 280.0F, 820.0F }, asw::color::orange, false },
        { { 1000.0F, 1120.0F }, asw::color::cyan, true },
        { { 400.0F, 1100.0F }, asw::color::violet, true },
    };
    const asw::Vec2<float> alarm(1350.0F, 750.0F);

    // --- Glows ---
    const auto lava = make_blob(64, asw::Color(255, 90, 20));
    const auto mushroom = make_blob(16, asw::Color(80, 255, 140));
    const std::vector<asw::Quad<float>> lava_pools = {
        { 900.0F, 300.0F, 96.0F, 64.0F },
        { 200.0F, 600.0F, 64.0F, 64.0F },
        { 1300.0F, 1050.0F, 128.0F, 80.0F },
    };
    std::vector<asw::Quad<float>> mushrooms;
    for (int i = 0; i < 12; ++i) {
        const auto fi = static_cast<float>(i);
        mushrooms.emplace_back(560.0F + std::sin(fi * 2.3F) * 180.0F,
            820.0F + std::cos(fi * 1.7F) * 90.0F, 12.0F, 12.0F);
    }

    // --- Ambient ---
    asw::lighting::AmbientCycle day(20.0F);
    day.add(0.0F, asw::Color(230, 225, 210));
    day.add(5.0F, asw::Color(200, 110, 80));
    day.add(10.0F, asw::Color(12, 14, 40));
    day.add(15.0F, asw::Color(140, 110, 150));
    const asw::Color night(30, 30, 50);

    // --- Tile lighting ---
    asw::lighting::TileLight tile_light(GRID_W, GRID_H, TILE);
    tile_light.set_falloff(0.12F);
    for (int y = 0; y < GRID_H; ++y) {
        for (int x = 0; x < GRID_W; ++x) {
            tile_light.set_solid(x, y, GRID_MAP[static_cast<std::size_t>(y)][x] == '#');
        }
    }

    // --- State ---
    bool lighting = true;
    bool flashlight = false;
    bool animate = true;
    bool glows = true;
    bool day_night = false;
    bool tiles = true;
    bool show_fov = false;
    std::size_t falloff = 1;
    constexpr std::array<asw::Falloff, 3> FALLOFFS { asw::Falloff::Linear, asw::Falloff::Smooth,
        asw::Falloff::Quadratic };

    std::vector<asw::lighting::Light> dropped;
    std::size_t next_drop = 0;

    float time = 0.0F;
    int frame = 0;

    asw::core::run([&]() {
        if (autorun) {
            if (frame == 2) {
                asw::input::simulate_key_down(asw::input::Key::F);
                asw::input::simulate_mouse_move({ 700.0F, 300.0F });
            }
            if (frame == 3) {
                asw::input::simulate_key_up(asw::input::Key::F);
                asw::input::simulate_key_down(asw::input::Key::D);
            }
            if (frame == 60) {
                asw::input::simulate_key_up(asw::input::Key::D);
            }
        }

        asw::core::update();

        const float dt = autorun ? 1.0F / 60.0F : asw::core::get_delta_time();
        time += dt;

        using asw::input::Key;
        auto pressed = [](Key key) { return asw::input::get_key_down(key); };

        if (pressed(Key::Escape)) {
            asw::core::exit();
        }
        if (pressed(Key::L)) {
            lighting = !lighting;
        }
        if (pressed(Key::F)) {
            flashlight = !flashlight;
        }
        if (pressed(Key::Num1)) {
            light_map.set_shadow_mode(asw::lighting::ShadowMode::None);
        }
        if (pressed(Key::Num2)) {
            light_map.set_shadow_mode(asw::lighting::ShadowMode::Cast);
        }
        if (pressed(Key::Num3)) {
            light_map.set_shadow_mode(asw::lighting::ShadowMode::Visibility);
        }
        if (pressed(Key::Num4)) {
            falloff = (falloff + 1) % FALLOFFS.size();
        }
        if (pressed(Key::Q)) {
            animate = !animate;
        }
        if (pressed(Key::E)) {
            glows = !glows;
        }
        if (pressed(Key::N)) {
            day_night = !day_night;
        }
        if (pressed(Key::T)) {
            tiles = !tiles;
        }
        if (pressed(Key::V)) {
            show_fov = !show_fov;
        }

        // Move, and keep out of walls
        asw::Vec2<float> move(0.0F, 0.0F);
        if (asw::input::get_key(Key::W) || asw::input::get_key(Key::Up)) {
            move.y -= 1.0F;
        }
        if (asw::input::get_key(Key::S) || asw::input::get_key(Key::Down)) {
            move.y += 1.0F;
        }
        if (asw::input::get_key(Key::A) || asw::input::get_key(Key::Left)) {
            move.x -= 1.0F;
        }
        if (asw::input::get_key(Key::D) || asw::input::get_key(Key::Right)) {
            move.x += 1.0F;
        }
        player.position += move.normalized() * (PLAYER_SPEED * dt);
        for (const auto& wall : walls) {
            player.position += player.get_push_out(wall);
        }
        player.position.x = std::clamp(player.position.x, 0.0F, WORLD_W - PLAYER_SIZE);
        player.position.y = std::clamp(player.position.y, 0.0F, WORLD_H - PLAYER_SIZE);

        camera.follow(player.get_center(), dt);
        camera.update(dt);
        light_map.update(dt);

        const auto mouse = camera.screen_to_world(asw::input::get_mouse().position);
        const auto center = player.get_center();

        if (asw::input::get_mouse_button_down(asw::input::MouseButton::Left)) {
            asw::lighting::Light light;
            light.position = mouse;
            light.color = rainbow(time * 3.0F);
            light.radius = asw::random::between(140.0F, 260.0F);
            if (dropped.size() < MAX_DROPPED) {
                dropped.push_back(light);
            } else {
                dropped[next_drop] = light;
            }
            next_drop = (next_drop + 1) % MAX_DROPPED;
        }

        // Tile lights: two lamps, and the player when inside the grid
        tile_light.clear_lights();
        tile_light.add_light(2, 1, asw::Color(255, 220, 120));
        tile_light.add_light(11, 10, asw::Color(220, 80, 255));
        const auto player_tile = (center - GRID_ORIGIN) / TILE;
        tile_light.add_light(static_cast<int>(std::floor(player_tile.x)),
            static_cast<int>(std::floor(player_tile.y)), asw::Color(200, 200, 200));
        tile_light.compute();

        // --- Scene ---
        asw::display::clear(asw::Color(40, 40, 44));

        const auto view = camera.get_view();
        for (float y = 0.0F; y < WORLD_H; y += 50.0F) {
            for (float x = 0.0F; x < WORLD_W; x += 50.0F) {
                const asw::Quad<float> cell(x, y, 50.0F, 50.0F);
                if (!view.collides(cell)) {
                    continue;
                }
                const bool dark
                    = (static_cast<int>(x / 50.0F) + static_cast<int>(y / 50.0F)) % 2 == 0;
                asw::draw::rect_fill(camera.world_to_screen(cell),
                    dark ? asw::Color(120, 110, 100) : asw::Color(140, 130, 118));
            }
        }

        // Tile grid
        for (int y = 0; y < GRID_H; ++y) {
            for (int x = 0; x < GRID_W; ++x) {
                const asw::Quad<float> cell(GRID_ORIGIN.x + static_cast<float>(x) * TILE,
                    GRID_ORIGIN.y + static_cast<float>(y) * TILE, TILE, TILE);
                const bool solid = tile_light.is_solid(x, y);
                asw::draw::rect_fill(camera.world_to_screen(cell),
                    solid ? asw::Color(70, 60, 80) : asw::Color(150, 150, 160));
            }
        }

        for (const auto& wall : walls) {
            asw::draw::rect_fill(camera.world_to_screen(wall), asw::color::slategray);
        }
        for (const auto& shape : shapes) {
            std::vector<asw::Vec2<float>> screen;
            for (const auto& p : shape) {
                screen.push_back(camera.world_to_screen(p));
            }
            asw::draw::polygon_fill(screen, asw::color::slategray);
            asw::draw::polygon(screen, asw::color::lightslategray);
        }

        for (const auto& pool : lava_pools) {
            asw::draw::stretch_sprite(lava, camera.world_to_screen(pool));
        }
        for (const auto& m : mushrooms) {
            asw::draw::stretch_sprite(mushroom, camera.world_to_screen(m));
        }
        for (const auto& torch : torches) {
            const auto p = camera.world_to_screen(torch.position);
            asw::draw::triangle_fill(p + asw::Vec2<float>(0.0F, -10.0F),
                p + asw::Vec2<float>(6.0F, 6.0F), p + asw::Vec2<float>(-6.0F, 6.0F), torch.color);
        }
        asw::draw::circle_fill(camera.world_to_screen(alarm), 8.0F, asw::color::red);
        asw::draw::rect_fill(camera.world_to_screen(player), asw::color::white);

        // --- Light ---
        const auto shape_falloff = FALLOFFS[falloff];
        light_map.set_ambient(day_night ? day.at(time) : night);
        light_map.clear();

        if (lighting) {
            asw::lighting::Light own;
            own.position = center;
            own.falloff = shape_falloff;
            own.color = asw::color::navajowhite;
            own.shadows = true;
            if (flashlight) {
                own.radius = 460.0F;
                own.cone = 0.9F;
                own.direction = std::atan2(mouse.y - center.y, mouse.x - center.x);
            } else {
                own.radius = 220.0F;
                own.flicker = animate ? 0.15F : 0.0F;
            }
            light_map.add(own);

            for (std::size_t i = 0; i < torches.size(); ++i) {
                asw::lighting::Light light;
                light.position = torches[i].position;
                light.color = torches[i].color;
                light.radius = 200.0F;
                light.falloff = shape_falloff;
                light.seed = static_cast<int>(i);
                if (animate) {
                    light.flicker = torches[i].pulses ? 0.0F : 0.3F;
                    light.pulse = torches[i].pulses ? 0.4F : 0.0F;
                    light.pulse_speed = 0.5F;
                }
                light_map.add(light);
            }

            // Turning red spot light
            asw::lighting::Light siren;
            siren.position = alarm;
            siren.color = asw::color::red;
            siren.radius = 380.0F;
            siren.cone = 0.6F;
            siren.direction = animate ? time * 1.5F : 0.0F;
            siren.falloff = shape_falloff;
            light_map.add(siren);

            for (auto light : dropped) {
                light.falloff = shape_falloff;
                light_map.add(light);
            }

            if (glows) {
                for (const auto& pool : lava_pools) {
                    light_map.add_glow(lava, pool);
                    const auto c = pool.get_center();
                    const float r = std::max(pool.size.x, pool.size.y) * 1.2F;
                    asw::lighting::Light heat;
                    heat.position = c;
                    heat.radius = r;
                    heat.color = asw::Color(255, 90, 20);
                    heat.intensity = 0.6F;
                    heat.shadows = false;
                    light_map.add(heat);
                }
                for (const auto& m : mushrooms) {
                    light_map.add_glow(mushroom, m);
                }
            }

            if (tiles) {
                light_map.add_tiles(tile_light, GRID_ORIGIN);
            }

            light_map.draw();
        }

        // Field of view
        if (show_fov) {
            const auto seen = asw::geometry::visibility(center, 500.0F, light_map.get_occluders());
            std::vector<asw::Vec2<float>> screen;
            for (const auto& p : seen) {
                screen.push_back(camera.world_to_screen(p));
            }
            asw::draw::polygon(screen, asw::color::yellow);
        }

        // --- HUD ---
        const std::array<std::string, 3> hud {
            std::string("1/2/3 shadows: ") + shadow_name(light_map.get_shadow_mode())
                + "   4 falloff: " + falloff_name(shape_falloff)
                + "   F flashlight: " + on_off(flashlight),
            std::string("Q flicker: ") + on_off(animate) + "   E glows: " + on_off(glows)
                + "   N day/night: " + on_off(day_night) + "   T tiles: " + on_off(tiles),
            std::string("V field of view: ") + on_off(show_fov)
                + "   L lighting: " + on_off(lighting) + "   click: drop light",
        };
        for (std::size_t i = 0; i < hud.size(); ++i) {
            asw::draw::text_shadow(font, hud[i], { 8.0F, 8.0F + static_cast<float>(i) * 12.0F },
                asw::color::white, asw::color::black);
        }

        if (autorun && frame == 70) {
            const bool saved = asw::display::screenshot("autorun.png");
            asw::log::info(saved ? "Saved autorun.png" : "Screenshot failed");
            asw::core::exit();
        }

        asw::display::present();
        frame++;
    });

    asw::core::shutdown();
    return 0;
}
