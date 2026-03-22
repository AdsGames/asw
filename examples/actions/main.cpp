/// @file main.cpp
/// @brief Action binding example
///
/// Demonstrates:
///   - Registering named actions with bind_action()
///   - Binding the same action to multiple input sources (keyboard + controller)
///   - is_action_down(), is_action_up(), is_action()
///   - get_action_strength() for analogue movement via a controller axis
///
/// Bindings:
///   "move_left"   → A key, Left arrow, controller left axis (negative X)
///   "move_right"  → D key, Right arrow, controller left axis (positive X)
///   "move_up"     → W key, Up arrow, controller left axis (negative Y)
///   "move_down"   → S key, Down arrow, controller left axis (positive Y)
///   "fire"        → Space, controller A button
///
/// Controls:
///   WASD / arrows / left stick - move the box
///   Space / A button           - fire (flash indicator)
///   Escape                     - quit

#include <asw/asw.h>

int main()
{
    asw::core::init(800, 600);
    asw::display::set_title("ASW Example - Action Bindings");
    asw::core::print_info();

    // --- Register actions ---
    asw::input::bind_action("move_left", asw::input::KeyBinding { asw::input::Key::A });
    asw::input::bind_action("move_left", asw::input::KeyBinding { asw::input::Key::Left });
    asw::input::bind_action("move_left",
        asw::input::ControllerButtonBinding { asw::input::ControllerButton::DPadLeft, 0 });
    asw::input::bind_action("move_left",
        asw::input::ControllerAxisBinding { asw::input::ControllerAxis::LeftX, 0, 0.2F, false });

    asw::input::bind_action("move_right", asw::input::KeyBinding { asw::input::Key::D });
    asw::input::bind_action("move_right", asw::input::KeyBinding { asw::input::Key::Right });
    asw::input::bind_action("move_right",
        asw::input::ControllerButtonBinding { asw::input::ControllerButton::DPadRight, 0 });
    asw::input::bind_action("move_right",
        asw::input::ControllerAxisBinding { asw::input::ControllerAxis::LeftX, 0, 0.2F, true });

    asw::input::bind_action("move_up", asw::input::KeyBinding { asw::input::Key::W });
    asw::input::bind_action("move_up", asw::input::KeyBinding { asw::input::Key::Up });
    asw::input::bind_action(
        "move_up", asw::input::ControllerButtonBinding { asw::input::ControllerButton::DPadUp, 0 });
    asw::input::bind_action("move_up",
        asw::input::ControllerAxisBinding { asw::input::ControllerAxis::LeftY, 0, 0.2F, false });

    asw::input::bind_action("move_down", asw::input::KeyBinding { asw::input::Key::S });
    asw::input::bind_action("move_down", asw::input::KeyBinding { asw::input::Key::Down });
    asw::input::bind_action("move_down",
        asw::input::ControllerButtonBinding { asw::input::ControllerButton::DPadDown, 0 });
    asw::input::bind_action("move_down",
        asw::input::ControllerAxisBinding { asw::input::ControllerAxis::LeftY, 0, 0.2F, true });

    asw::input::bind_action("fire", asw::input::KeyBinding { asw::input::Key::Space });
    asw::input::bind_action(
        "fire", asw::input::ControllerButtonBinding { asw::input::ControllerButton::A });

    asw::input::bind_action("quit", asw::input::KeyBinding { asw::input::Key::Escape });
    asw::input::bind_action(
        "quit", asw::input::ControllerButtonBinding { asw::input::ControllerButton::Start });

    asw::Vec2<float> pos { 375.0F, 275.0F };
    constexpr float box_size = 50.0F;
    constexpr float base_speed = 4.0F;

    int fire_frames = 0;

    while (!asw::core::is_exiting()) {
        asw::core::update();

        if (asw::input::is_action("quit")) {
            asw::core::exit();
        }

        // Move using analogue strength so a controller stick gives smooth speed
        pos.x -= asw::input::get_action_strength("move_left") * base_speed;
        pos.x += asw::input::get_action_strength("move_right") * base_speed;
        pos.y -= asw::input::get_action_strength("move_up") * base_speed;
        pos.y += asw::input::get_action_strength("move_down") * base_speed;

        // Clamp to window
        const auto win = asw::display::get_logical_size();
        if (pos.x < 0) {
            pos.x = 0;
        }
        if (pos.y < 0) {
            pos.y = 0;
        }
        if (pos.x + box_size > static_cast<float>(win.x)) {
            pos.x = static_cast<float>(win.x) - box_size;
        }
        if (pos.y + box_size > static_cast<float>(win.y)) {
            pos.y = static_cast<float>(win.y) - box_size;
        }

        // Fire
        if (asw::input::is_action("fire")) {
            fire_frames = 12;
            asw::log::info("Fire!");
        }
        if (fire_frames > 0) {
            --fire_frames;
        }

        // --- Draw ---
        asw::display::clear(asw::color::darkslategray);

        // Fire flash
        if (fire_frames > 0) {
            const float alpha = static_cast<float>(fire_frames) / 12.0F;
            asw::draw::circle_fill({ pos.x + box_size / 2.0F, pos.y + box_size / 2.0F },
                box_size * 1.5F, asw::Color(255, 200, 0, static_cast<uint8_t>(alpha * 200)));
        }

        // Player box
        const bool moving = asw::input::is_action("move_left")
            || asw::input::is_action("move_right") || asw::input::is_action("move_up")
            || asw::input::is_action("move_down");
        asw::draw::rect_fill({ pos, { box_size, box_size } },
            moving ? asw::color::cornflowerblue : asw::color::steelblue);
        asw::draw::rect({ pos, { box_size, box_size } }, asw::color::white);

        // Strength bars for left/right
        const float sl = asw::input::get_action_strength("move_left");
        const float sr = asw::input::get_action_strength("move_right");
        const float su = asw::input::get_action_strength("move_up");
        const float sd = asw::input::get_action_strength("move_down");

        constexpr float bar_w = 100.0F;
        constexpr float bar_h = 14.0F;
        const float by = static_cast<float>(win.y) - 60.0F;

        asw::draw::rect({ 50.0F, by, bar_w, bar_h }, asw::color::gray);
        asw::draw::rect_fill({ 50.0F, by, bar_w * sl, bar_h }, asw::color::cyan);

        asw::draw::rect({ 650.0F, by, bar_w, bar_h }, asw::color::gray);
        asw::draw::rect_fill({ 650.0F, by, bar_w * sr, bar_h }, asw::color::cyan);

        asw::draw::rect({ 350.0F, by - 20.0F, bar_h, bar_w }, asw::color::gray);
        asw::draw::rect_fill({ 350.0F, by - 20.0F, bar_h, bar_w * su }, asw::color::cyan);

        asw::draw::rect({ 380.0F, by - 20.0F, bar_h, bar_w }, asw::color::gray);
        asw::draw::rect_fill({ 380.0F, by - 20.0F, bar_h, bar_w * sd }, asw::color::cyan);

        asw::display::present();
    }

    asw::core::shutdown();

    return 0;
}
