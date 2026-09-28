/// @file main.cpp
/// @brief Scrolling world example
///
/// Demonstrates:
///   - asw::Camera following a player, clamped to the world, with shake
///   - asw::Quad::get_push_out() to stop the player walking through walls
///   - asw::SpriteSheet and asw::Animation for a frame animated sprite
///   - asw::ParticleEmitter drawn in world space through a camera
///   - asw::display::screenshot()
///   - asw::input::simulate_*() to script a run without a keyboard
///
/// Controls:
///   WASD / Arrows - move
///   Space - shake the camera and burst particles
///   F12 - save screenshot.png
///   Escape - quit
///
/// Scripted run:
///   ASW_EXAMPLE_AUTORUN=1 SDL_VIDEO_DRIVER=dummy ./example_camera
///   Walks right, bumps into a wall, saves autorun.png and quits.

#include <asw/asw.h>
#include <cstdlib>
#include <vector>

namespace {
constexpr float WORLD_W = 2400.0F;
constexpr float WORLD_H = 1600.0F;
constexpr float PLAYER_SIZE = 48.0F;
constexpr float PLAYER_SPEED = 320.0F;

// Build a four frame sheet at runtime so the example needs no image files
asw::Texture make_sheet()
{
    auto sheet = asw::assets::create_texture(128, 32);
    asw::display::set_render_target(sheet);
    asw::display::clear(asw::Color(0, 0, 0, 0));

    const std::vector<asw::Color> colors
        = { asw::color::orange, asw::color::yellow, asw::color::orange, asw::color::red };
    for (int i = 0; i < 4; ++i) {
        const float x = static_cast<float>(i) * 32.0F;
        asw::draw::circle_fill({ x + 16.0F, 16.0F }, 10.0F + static_cast<float>(i % 2) * 4.0F,
            colors[static_cast<std::size_t>(i)]);
    }

    asw::display::reset_render_target();
    return sheet;
}
} // namespace

int main()
{
    asw::core::init(800, 600);
    asw::display::set_title("ASW Example - Camera");

    const bool autorun = std::getenv("ASW_EXAMPLE_AUTORUN") != nullptr;

    asw::Camera camera(asw::Vec2<float>(800.0F, 600.0F));
    camera.set_bounds(asw::Quad<float>(0.0F, 0.0F, WORLD_W, WORLD_H));

    asw::Quad<float> player(200.0F, 200.0F, PLAYER_SIZE, PLAYER_SIZE);
    camera.snap_to(player.get_center());

    const std::vector<asw::Quad<float>> walls = {
        { 600.0F, 100.0F, 60.0F, 500.0F },
        { 900.0F, 700.0F, 500.0F, 60.0F },
        { 1500.0F, 300.0F, 80.0F, 800.0F },
    };

    const asw::SpriteSheet sheet(make_sheet(), asw::Vec2<float>(32.0F, 32.0F));
    asw::Animation flame(sheet.get_frame_count(), 0.12F);

    asw::ParticleConfig sparks;
    sparks.color_start = asw::color::yellow;
    sparks.color_end = asw::color::red;
    sparks.speed_min = 80.0F;
    sparks.speed_max = 260.0F;
    sparks.lifetime_min = 0.3F;
    sparks.lifetime_max = 0.7F;
    asw::ParticleEmitter emitter(sparks, 256);

    int frame = 0;

    while (!asw::core::is_exiting()) {
        // Scripted input: walk right into the first wall, then burst
        if (autorun) {
            if (frame == 5) {
                asw::input::simulate_key_down(asw::input::Key::D);
            }
            if (frame == 100) {
                asw::input::simulate_key_up(asw::input::Key::D);
                asw::input::simulate_key_down(asw::input::Key::Space);
            }
            if (frame == 101) {
                asw::input::simulate_key_up(asw::input::Key::Space);
            }
        }

        asw::core::update();

        // Real frame time, but a fixed step for scripted runs so they repeat
        const float dt = autorun ? 1.0F / 60.0F : asw::core::get_delta_time();

        if (asw::input::get_key_down(asw::input::Key::Escape)) {
            asw::core::exit();
        }

        // Move
        asw::Vec2<float> move(0.0F, 0.0F);
        if (asw::input::get_key(asw::input::Key::W) || asw::input::get_key(asw::input::Key::Up)) {
            move.y -= 1.0F;
        }
        if (asw::input::get_key(asw::input::Key::S)
            || asw::input::get_key(asw::input::Key::Down)) {
            move.y += 1.0F;
        }
        if (asw::input::get_key(asw::input::Key::A)
            || asw::input::get_key(asw::input::Key::Left)) {
            move.x -= 1.0F;
        }
        if (asw::input::get_key(asw::input::Key::D)
            || asw::input::get_key(asw::input::Key::Right)) {
            move.x += 1.0F;
        }
        player.position += move.normalized() * (PLAYER_SPEED * dt);

        // Walls are solid
        for (const auto& wall : walls) {
            player.position += player.get_push_out(wall);
        }

        if (asw::input::get_key_down(asw::input::Key::Space)) {
            camera.shake(18.0F);
            emitter.transform.position = player.get_center();
            emitter.emit(60);
        }

        camera.follow(player.get_center(), dt);
        camera.update(dt);
        flame.update(dt);
        emitter.update(dt);

        // Draw
        asw::display::clear(asw::color::darkslategray);

        // Grid, only the lines in view
        const auto view = camera.get_view();
        for (float x = 0.0F; x <= WORLD_W; x += 100.0F) {
            if (x >= view.position.x && x <= view.position.x + view.size.x) {
                asw::draw::line(camera.world_to_screen(asw::Vec2<float>(x, 0.0F)),
                    camera.world_to_screen(asw::Vec2<float>(x, WORLD_H)),
                    asw::color::slategray);
            }
        }
        for (float y = 0.0F; y <= WORLD_H; y += 100.0F) {
            if (y >= view.position.y && y <= view.position.y + view.size.y) {
                asw::draw::line(camera.world_to_screen(asw::Vec2<float>(0.0F, y)),
                    camera.world_to_screen(asw::Vec2<float>(WORLD_W, y)), asw::color::slategray);
            }
        }

        for (const auto& wall : walls) {
            if (view.collides(wall)) {
                asw::draw::rect_fill(camera.world_to_screen(wall), asw::color::steelblue);
            }
        }

        asw::draw::rect_fill(camera.world_to_screen(player), asw::color::white);
        sheet.draw_frame(flame.get_frame(),
            camera.world_to_screen(asw::Quad<float>(
                player.position.x + 8.0F, player.position.y - 36.0F, 32.0F, 32.0F)));

        emitter.draw(camera);

        if (asw::input::get_key_down(asw::input::Key::F12)) {
            asw::display::screenshot("screenshot.png");
        }

        if (autorun && frame == 110) {
            const bool saved = asw::display::screenshot("autorun.png");
            asw::log::info(saved ? "Saved autorun.png" : "Screenshot failed");
            asw::log::info("Player x: " + std::to_string(player.position.x));
            asw::core::exit();
        }

        asw::display::present();
        frame++;
    }

    asw::core::shutdown();
    return 0;
}
