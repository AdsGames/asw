/// @file main.cpp
/// @brief Action binding example
///
/// Demonstrates:
///   - bind_action() with every binding type: key, mouse button, controller
///     button and controller axis
///   - Several bindings on one action (any of them triggers it)
///   - get_action_down() (pressed this frame), get_action() (held) and
///     get_action_up() (released this frame)
///   - get_action_strength() for analogue movement from a stick
///   - ANY_CONTROLLER so every connected pad drives the same actions
///   - get_last_device() to show keyboard or controller prompts
///
/// Bindings:
///   move_*  - WASD, arrows, D-pad, left stick
///   fire    - Space, left mouse button, controller A
///   quit    - Escape, controller Start
///
/// Controls:
///   Move            - move the ship
///   Tap fire        - small shot (get_action_down)
///   Hold fire       - charge (get_action)
///   Release fire    - big shot if charged (get_action_up)
///
/// Scripted run:
///   ASW_EXAMPLE_AUTORUN=1 SDL_VIDEO_DRIVER=dummy ./example_actions

#include <algorithm>
#include <asw/asw.h>
#include <cstdlib>
#include <format>
#include <string>
#include <vector>

namespace {
using asw::input::ControllerAxis;
using asw::input::ControllerAxisBinding;
using asw::input::ControllerButton;
using asw::input::ControllerButtonBinding;
using asw::input::Key;
using asw::input::KeyBinding;

constexpr float SHIP_SIZE = 40.0F;
constexpr float SPEED = 260.0F; // pixels per second at full strength
constexpr float SHOT_SPEED = 600.0F;
constexpr float FULL_CHARGE = 0.6F; // seconds of holding for a big shot

struct Shot {
    asw::Vec2<float> position;
    float radius;
};

void bind_move(const std::string& name, Key key, Key arrow, ControllerButton dpad,
    ControllerAxis axis, bool positive)
{
    asw::input::bind_action(name, KeyBinding { key });
    asw::input::bind_action(name, KeyBinding { arrow });
    asw::input::bind_action(name, ControllerButtonBinding { dpad, asw::input::ANY_CONTROLLER });
    asw::input::bind_action(
        name, ControllerAxisBinding { axis, asw::input::ANY_CONTROLLER, 0.2F, positive });
}

// Vertical bar showing an action's strength
void draw_strength(const asw::Font& font, const asw::Vec2<float>& pos, const std::string& name)
{
    const float strength = asw::input::get_action_strength(name);
    const asw::Quad<float> area(pos, asw::Vec2<float>(80.0F, 12.0F));
    asw::draw::rect_fill(area, asw::Color(40, 46, 58));
    asw::draw::rect_fill({ area.position, { area.size.x * strength, area.size.y } },
        asw::input::get_action(name) ? asw::color::cyan : asw::color::cyan.darken(0.5F));
    asw::draw::rect(area, asw::color::gray);
    asw::draw::text(font, name, pos + asw::Vec2<float>(90.0F, 2.0F), asw::color::white);
}
} // namespace

int main()
{
    asw::core::init(800, 600);
    asw::display::set_title("ASW Example - Action Bindings");

    const bool autorun = std::getenv("ASW_EXAMPLE_AUTORUN") != nullptr;
    const auto font = asw::assets::load_font("assets/font.ttf", 8.0F, asw::FontStyle::Pixel);

    // --- Register actions ---
    bind_move("move_left", Key::A, Key::Left, ControllerButton::DPadLeft, ControllerAxis::LeftX,
        false);
    bind_move("move_right", Key::D, Key::Right, ControllerButton::DPadRight, ControllerAxis::LeftX,
        true);
    bind_move("move_up", Key::W, Key::Up, ControllerButton::DPadUp, ControllerAxis::LeftY, false);
    bind_move("move_down", Key::S, Key::Down, ControllerButton::DPadDown, ControllerAxis::LeftY,
        true);

    asw::input::bind_action("fire", KeyBinding { Key::Space });
    asw::input::bind_action("fire", asw::input::MouseButtonBinding { asw::input::MouseButton::Left });
    asw::input::bind_action(
        "fire", ControllerButtonBinding { ControllerButton::A, asw::input::ANY_CONTROLLER });

    asw::input::bind_action("quit", KeyBinding { Key::Escape });
    asw::input::bind_action(
        "quit", ControllerButtonBinding { ControllerButton::Start, asw::input::ANY_CONTROLLER });

    asw::Vec2<float> pos { 380.0F, 420.0F };
    std::vector<Shot> shots;
    float charge = 0.0F;
    int shots_fired = 0;
    int frame = 0;

    while (!asw::core::is_exiting()) {
        // Scripted input: move left, tap fire, then hold and release for a big shot
        if (autorun) {
            if (frame == 2) {
                asw::input::simulate_key_down(Key::A);
            }
            if (frame == 20) {
                asw::input::simulate_key_up(Key::A);
                asw::input::simulate_key_down(Key::Space);
            }
            if (frame == 22) {
                asw::input::simulate_key_up(Key::Space);
            }
            if (frame == 26) {
                asw::input::simulate_key_down(Key::Space);
            }
            if (frame == 70) {
                asw::input::simulate_key_up(Key::Space);
            }
        }

        asw::core::update();
        const float dt = autorun ? 1.0F / 60.0F : asw::core::get_delta_time();

        if (asw::input::get_action_down("quit")) {
            asw::core::exit();
        }

        // Analogue strength, so a stick gives smooth speed
        const asw::Vec2<float> move(asw::input::get_action_strength("move_right")
                - asw::input::get_action_strength("move_left"),
            asw::input::get_action_strength("move_down")
                - asw::input::get_action_strength("move_up"));
        pos += move * (SPEED * dt);

        const auto win = asw::display::get_logical_size();
        pos.x = std::clamp(pos.x, 0.0F, static_cast<float>(win.x) - SHIP_SIZE);
        pos.y = std::clamp(pos.y, 0.0F, static_cast<float>(win.y) - SHIP_SIZE);

        const asw::Vec2<float> nose(pos.x + (SHIP_SIZE / 2.0F), pos.y);

        // Down: small shot. Held: charge. Up: big shot if fully charged.
        if (asw::input::get_action_down("fire")) {
            shots.push_back({ nose, 4.0F });
            shots_fired++;
            charge = 0.0F;
        }
        if (asw::input::get_action("fire")) {
            charge = std::min(FULL_CHARGE, charge + dt);
        }
        if (asw::input::get_action_up("fire")) {
            if (charge >= FULL_CHARGE) {
                shots.push_back({ nose, 16.0F });
                shots_fired++;
            }
            charge = 0.0F;
        }

        for (auto& s : shots) {
            s.position.y -= SHOT_SPEED * dt;
        }
        std::erase_if(shots, [](const Shot& s) { return s.position.y < -s.radius; });

        // --- Draw ---
        asw::display::clear(asw::Color(24, 28, 36));

        for (const auto& s : shots) {
            asw::draw::circle_fill(s.position, s.radius, asw::color::yellow);
        }

        // Charge ring grows while held, turns orange when full
        if (charge > 0.0F) {
            const float t = charge / FULL_CHARGE;
            asw::draw::circle(nose, 6.0F + (t * 14.0F),
                t >= 1.0F ? asw::color::orange : asw::color::white.with_alpha(160));
        }

        const asw::Quad<float> ship(pos, asw::Vec2<float>(SHIP_SIZE, SHIP_SIZE));
        const bool moving = move.x != 0.0F || move.y != 0.0F;
        asw::draw::rect_fill(ship, moving ? asw::color::cornflowerblue : asw::color::steelblue);
        asw::draw::rect(ship, asw::color::white);

        // Strength bars
        draw_strength(font, { 10.0F, 10.0F }, "move_left");
        draw_strength(font, { 10.0F, 26.0F }, "move_right");
        draw_strength(font, { 10.0F, 42.0F }, "move_up");
        draw_strength(font, { 10.0F, 58.0F }, "move_down");
        draw_strength(font, { 10.0F, 74.0F }, "fire");

        asw::draw::text_shadow(font, std::format("Shots fired: {}", shots_fired),
            asw::Vec2<float>(790.0F, 10.0F), asw::color::white, asw::color::black,
            asw::Vec2<float>(2.0F, 2.0F), asw::TextJustify::Right);
        // Prompts follow whichever device was used last
        const bool on_pad
            = asw::input::get_last_device() == asw::input::InputDevice::Controller;
        const std::string fire = on_pad ? "A" : "Space";
        asw::draw::text_shadow(font, "Tap " + fire + ": small  Hold + release " + fire + ": big",
            asw::Vec2<float>(400.0F, 580.0F), asw::color::lightgray, asw::color::black,
            asw::Vec2<float>(2.0F, 2.0F), asw::TextJustify::Center);

        if (autorun && frame == 74) {
            const bool saved = asw::display::screenshot("autorun.png");
            asw::log::info(saved ? "Saved autorun.png" : "Screenshot failed");
            asw::log::info("Shots fired: {} (expected 3)", shots_fired);
            asw::core::exit();
        }

        asw::display::present();
        frame++;
    }

    asw::core::shutdown();
    return 0;
}
