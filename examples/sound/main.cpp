/// @file main.cpp
/// @brief Positional sound example
///
/// Demonstrates:
///   - asw::sound::play_positional() with a listener that follows the player
///   - asw::sound::SoundHandle to move a looping sound while it plays
///   - Doppler pitch shift on a fast moving source
///   - Occlusion: a radio behind walls sounds muffled
///   - pitch_variation / volume_variation on footsteps
///   - Buses and ducking: explosions duck the ambient bus
///   - Priority based voice stealing
///   - Stereo and Surround (SDL_mixer 3D) spatial modes
///
/// Controls:
///   WASD / Arrows - move (you are the listener)
///   Left click - explosion at the mouse (high priority, ducks ambient)
///   F - spray 40 low priority pickups to fill every voice
///   Tab - switch Stereo / Surround
///   O - doppler on / off
///   M - mute / unmute the ambient bus
///   Escape - quit
///
/// Scripted run:
///   ASW_EXAMPLE_AUTORUN=1 SDL_VIDEO_DRIVER=dummy SDL_AUDIO_DRIVER=dummy ./example_sound
///
/// Sounds are from other A.D.S. Games projects (Gunner 2.0, JimFarm).

#include <asw/asw.h>
#include <cmath>
#include <cstdlib>
#include <string>
#include <vector>

namespace {
constexpr float WORLD_W = 2000.0F;
constexpr float WORLD_H = 1400.0F;
constexpr float PLAYER_SIZE = 32.0F;
constexpr float PLAYER_SPEED = 260.0F;
constexpr float STEP_INTERVAL = 0.32F;

const asw::Vec2<float> RADIO_POS(350.0F, 900.0F);
const asw::Vec2<float> DRONE_CENTER(1200.0F, 600.0F);
constexpr float DRONE_RADIUS = 380.0F;
constexpr float DRONE_SPEED = 1.5F; // radians per second

// True if any wall blocks the straight line between two points
bool blocked(const asw::Vec2<float>& from, const asw::Vec2<float>& to,
    const std::vector<asw::Quad<float>>& walls)
{
    const float distance = from.distance(to);
    const int steps = std::max(1, static_cast<int>(distance / 8.0F));
    for (int i = 1; i < steps; ++i) {
        const auto point
            = from + ((to - from) * (static_cast<float>(i) / static_cast<float>(steps)));
        for (const auto& wall : walls) {
            if (wall.contains(point)) {
                return true;
            }
        }
    }
    return false;
}
} // namespace

int main()
{
    asw::core::init(800, 600);
    asw::display::set_title("ASW Example - Sound");

    const bool autorun = std::getenv("ASW_EXAMPLE_AUTORUN") != nullptr;

    const auto font = asw::assets::load_font("assets/font.ttf", 8.0F, asw::FontStyle::Pixel);
    const auto radio_sample = asw::assets::load_sample("assets/radio.wav");
    const auto drone_sample = asw::assets::load_sample("assets/laser.wav");
    const auto explosion_sample = asw::assets::load_sample("assets/big_explosion.wav");
    const auto pickup_sample = asw::assets::load_sample("assets/pickup.wav");
    const std::vector<asw::Sample> steps = { asw::assets::load_sample("assets/step_1.wav"),
        asw::assets::load_sample("assets/step_2.wav") };

    asw::Camera camera(asw::Vec2<float>(800.0F, 600.0F));
    camera.set_bounds(asw::Quad<float>(0.0F, 0.0F, WORLD_W, WORLD_H));

    asw::Quad<float> player(700.0F, 700.0F, PLAYER_SIZE, PLAYER_SIZE);
    camera.snap_to(player.get_center());

    // A room around the radio with a door on the right
    const std::vector<asw::Quad<float>> walls = {
        { 180.0F, 740.0F, 340.0F, 24.0F }, // top
        { 180.0F, 1040.0F, 340.0F, 24.0F }, // bottom
        { 180.0F, 740.0F, 24.0F, 324.0F }, // left
        { 496.0F, 740.0F, 24.0F, 110.0F }, // right, above the door
        { 496.0F, 950.0F, 24.0F, 114.0F }, // right, below the door
    };

    // Looping positional sounds, kept alive and moved through their handles
    asw::sound::PlayOptions radio_options;
    radio_options.loop = true;
    radio_options.bus = asw::sound::Bus::Ambient;
    radio_options.priority = 5;
    radio_options.attenuation = { 80.0F, 900.0F, asw::sound::Rolloff::Inverse };
    const auto radio = asw::sound::play_positional(radio_sample, RADIO_POS, radio_options);

    asw::sound::PlayOptions drone_options;
    drone_options.loop = true;
    drone_options.volume = 0.6F;
    drone_options.pitch = 0.6F;
    drone_options.priority = 5;
    drone_options.attenuation = { 60.0F, 1100.0F, asw::sound::Rolloff::Inverse };
    const auto drone = asw::sound::play_positional(
        drone_sample, DRONE_CENTER + asw::Vec2<float>(DRONE_RADIUS, 0.0F), drone_options);

    auto spatial_mode = asw::sound::SpatialMode::Stereo;
    bool doppler = true;
    bool ambient_muted = false;
    float drone_angle = 0.0F;
    float radio_occlusion = 0.0F;
    float step_timer = 0.0F;
    int step_index = 0;

    int frame = 0;

    while (!asw::core::is_exiting()) {
        // Scripted input: walk towards the radio room, fill every voice,
        // then set off an explosion
        if (autorun) {
            if (frame == 5) {
                asw::input::simulate_key_down(asw::input::Key::A);
            }
            if (frame == 60) {
                asw::input::simulate_key_up(asw::input::Key::A);
                asw::input::simulate_key_down(asw::input::Key::F);
            }
            if (frame == 61) {
                asw::input::simulate_key_up(asw::input::Key::F);
                asw::input::simulate_mouse_move(asw::Vec2<float>(600.0F, 200.0F));
            }
            if (frame == 62) {
                asw::input::simulate_mouse_button_down(asw::input::MouseButton::Left);
            }
            if (frame == 63) {
                asw::input::simulate_mouse_button_up(asw::input::MouseButton::Left);
            }
        }

        asw::core::update();

        // Real frame time, but a fixed step for scripted runs so they repeat
        const float dt = autorun ? 1.0F / 60.0F : asw::core::get_delta_time();

        if (asw::input::get_key_down(asw::input::Key::Escape)) {
            asw::core::exit();
        }

        // Toggles
        if (asw::input::get_key_down(asw::input::Key::Tab)) {
            spatial_mode = spatial_mode == asw::sound::SpatialMode::Stereo
                ? asw::sound::SpatialMode::Surround
                : asw::sound::SpatialMode::Stereo;
            asw::sound::set_spatial_mode(spatial_mode);
        }
        if (asw::input::get_key_down(asw::input::Key::O)) {
            doppler = !doppler;
            asw::sound::set_doppler(doppler ? 1.0F : 0.0F);
        }
        if (asw::input::get_key_down(asw::input::Key::M)) {
            ambient_muted = !ambient_muted;
            asw::sound::set_bus_volume(asw::sound::Bus::Ambient, ambient_muted ? 0.0F : 1.0F);
        }

        // Move
        asw::Vec2<float> move(0.0F, 0.0F);
        if (asw::input::get_key(asw::input::Key::W) || asw::input::get_key(asw::input::Key::Up)) {
            move.y -= 1.0F;
        }
        if (asw::input::get_key(asw::input::Key::S) || asw::input::get_key(asw::input::Key::Down)) {
            move.y += 1.0F;
        }
        if (asw::input::get_key(asw::input::Key::A) || asw::input::get_key(asw::input::Key::Left)) {
            move.x -= 1.0F;
        }
        if (asw::input::get_key(asw::input::Key::D)
            || asw::input::get_key(asw::input::Key::Right)) {
            move.x += 1.0F;
        }
        const auto player_velocity = move.normalized() * PLAYER_SPEED;
        player.position += player_velocity * dt;
        for (const auto& wall : walls) {
            player.position += player.get_push_out(wall);
        }

        // The player is the listener
        asw::sound::set_listener(player.get_center(), player_velocity);

        // Footsteps, never quite the same twice
        if (move.x != 0.0F || move.y != 0.0F) {
            step_timer -= dt;
            if (step_timer <= 0.0F) {
                asw::sound::PlayOptions step;
                step.volume = 0.1F;
                step.pitch_variation = 0.08F;
                step.volume_variation = 0.2F;
                step.priority = 1;
                asw::sound::play(steps[static_cast<std::size_t>(step_index)], step);
                step_index = (step_index + 1) % static_cast<int>(steps.size());
                step_timer = STEP_INTERVAL;
            }
        } else {
            step_timer = 0.0F;
        }

        // Drone circles fast, so its pitch rises as it comes and falls as it goes
        drone_angle += DRONE_SPEED * dt;
        const auto drone_pos = DRONE_CENTER
            + asw::Vec2<float>(std::cos(drone_angle), std::sin(drone_angle)) * DRONE_RADIUS;
        const auto drone_velocity = asw::Vec2<float>(-std::sin(drone_angle), std::cos(drone_angle))
            * (DRONE_RADIUS * DRONE_SPEED);
        drone.set_position(drone_pos);
        drone.set_velocity(drone_velocity);

        // Radio is muffled when a wall is in the way. Ease so it does not snap.
        const float occlusion_target = blocked(player.get_center(), RADIO_POS, walls) ? 1.0F : 0.0F;
        radio_occlusion += (occlusion_target - radio_occlusion) * std::min(1.0F, dt * 6.0F);
        radio.set_occlusion(radio_occlusion);

        // Explosion: high priority, so it plays even when every voice is busy
        if (asw::input::get_mouse_button_down(asw::input::MouseButton::Left)) {
            const auto target = camera.screen_to_world(asw::input::get_mouse().position);

            asw::sound::PlayOptions boom;
            boom.priority = 10;
            boom.pitch_variation = 0.1F;
            boom.attenuation = { 150.0F, 1500.0F, asw::sound::Rolloff::Inverse };
            asw::sound::play_positional(explosion_sample, target, boom);

            asw::sound::duck(asw::sound::Bus::Ambient, 0.2F, 0.8F, 0.15F);
            camera.shake(14.0F);
        }

        // Fill every voice with low priority sounds
        if (asw::input::get_key_down(asw::input::Key::F)) {
            for (int i = 0; i < 40; ++i) {
                asw::sound::PlayOptions pickup;
                pickup.volume = 0.3F;
                pickup.pitch_variation = 0.3F;
                pickup.priority = 0;
                const auto offset = asw::Vec2<float>(
                    asw::random::between(-400.0F, 400.0F), asw::random::between(-300.0F, 300.0F));
                asw::sound::play_positional(pickup_sample, player.get_center() + offset, pickup);
            }
        }

        camera.follow(player.get_center(), dt);
        camera.update(dt);

        // Draw
        asw::display::clear(asw::Color(24, 28, 36));

        const auto view = camera.get_view();
        for (float x = 0.0F; x <= WORLD_W; x += 100.0F) {
            if (x >= view.position.x && x <= view.position.x + view.size.x) {
                asw::draw::line(camera.world_to_screen(asw::Vec2<float>(x, 0.0F)),
                    camera.world_to_screen(asw::Vec2<float>(x, WORLD_H)), asw::Color(40, 46, 58));
            }
        }
        for (float y = 0.0F; y <= WORLD_H; y += 100.0F) {
            if (y >= view.position.y && y <= view.position.y + view.size.y) {
                asw::draw::line(camera.world_to_screen(asw::Vec2<float>(0.0F, y)),
                    camera.world_to_screen(asw::Vec2<float>(WORLD_W, y)), asw::Color(40, 46, 58));
            }
        }

        // Hearing ranges
        asw::draw::circle(camera.world_to_screen(RADIO_POS), radio_options.attenuation.max_distance,
            asw::Color(80, 160, 90, 90));
        asw::draw::circle(camera.world_to_screen(drone_pos), drone_options.attenuation.max_distance,
            asw::Color(200, 120, 60, 70));

        for (const auto& wall : walls) {
            asw::draw::rect_fill(camera.world_to_screen(wall), asw::color::steelblue);
        }

        // Line of sight to the radio, red when blocked
        asw::draw::line(camera.world_to_screen(player.get_center()),
            camera.world_to_screen(RADIO_POS),
            occlusion_target > 0.0F ? asw::Color(220, 60, 60, 140) : asw::Color(90, 220, 90, 140));

        asw::draw::circle_fill(camera.world_to_screen(RADIO_POS), 12.0F, asw::color::lime);
        asw::draw::circle_fill(camera.world_to_screen(drone_pos), 10.0F, asw::color::orange);
        asw::draw::rect_fill(camera.world_to_screen(player), asw::color::white);

        // HUD
        const auto hud = [&](int line, const std::string& text) {
            asw::draw::text_shadow(font, text,
                asw::Vec2<float>(10.0F, 10.0F + (static_cast<float>(line) * 14.0F)),
                asw::color::white);
        };
        hud(0,
            std::string("Mode (Tab): ")
                + (spatial_mode == asw::sound::SpatialMode::Stereo ? "Stereo" : "Surround"));
        hud(1, std::string("Doppler (O): ") + (doppler ? "on" : "off"));
        hud(2, std::string("Ambient bus (M): ") + (ambient_muted ? "muted" : "on"));
        hud(3,
            "Radio muffled: " + std::to_string(static_cast<int>(radio_occlusion * 100.0F)) + "%");
        hud(4, "Click: explosion  F: fill voices");

        if (autorun && frame == 70) {
            const bool saved = asw::display::screenshot("autorun.png");
            asw::log::info(saved ? "Saved autorun.png" : "Screenshot failed");
            asw::log::info("Radio playing: " + std::string(radio.is_playing() ? "yes" : "no"));
            asw::log::info("Drone playing: " + std::string(drone.is_playing() ? "yes" : "no"));
            asw::log::info("Radio occlusion: " + std::to_string(radio_occlusion));
            asw::core::exit();
        }

        asw::display::present();
        frame++;
    }

    asw::core::shutdown();
    return 0;
}
