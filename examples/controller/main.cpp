/// @file main.cpp
/// @brief Controller / gamepad input example
///
/// Demonstrates:
///   - Detecting connected controllers via get_controller_count()
///   - Reading axes with get_controller_axis()
///   - Polling buttons with get_controller_button_down() / get_controller_button_up() /
///   get_controller_button()
///   - Setting the dead zone with set_controller_dead_zone()
///   - Visualising left/right sticks as dots inside circles
///   - Displaying button states as coloured indicators
///
/// Controls:
///   Left stick         - move left stick indicator
///   Right stick        - move right stick indicator
///   Triggers           - fill trigger bars
///   Any button         - highlight corresponding indicator
///   Keyboard Escape    - quit

#include <asw/asw.h>

namespace {

/// Draw a stick visualiser: a circle with a dot at the axis position.
void draw_stick(asw::Vec2<float> center, float radius, float axis_x, float axis_y)
{
    asw::draw::circle(center, radius, asw::color::gray);
    asw::draw::circle_fill(
        { center.x + axis_x * radius, center.y + axis_y * radius }, 10.0F, asw::color::cyan);
}

/// Draw a horizontal trigger bar filled proportionally.
void draw_trigger(asw::Vec2<float> pos, asw::Vec2<float> size, float value)
{
    asw::draw::rect({ pos, size }, asw::color::gray);
    asw::draw::rect_fill({ pos, { size.x * value, size.y } }, asw::color::orange);
}

/// Draw a button indicator (coloured box).
void draw_button(asw::Vec2<float> pos, float size, asw::Color on_color, bool pressed)
{
    asw::draw::rect_fill({ pos, { size, size } }, pressed ? on_color : on_color.darken(0.7F));
    asw::draw::rect({ pos, { size, size } }, asw::color::gray);
}

} // namespace

int main()
{
    asw::core::init(800, 600);
    asw::display::set_title("ASW Example - Controller");

    // Use a comfortable dead zone
    asw::input::set_controller_dead_zone(0, 0.15F);

    while (!asw::core::exit) {
        asw::core::update();

        if (asw::input::get_key_down(asw::input::Key::Escape)) {
            asw::core::exit = true;
        }

        // Log button events for controller 0
        if (asw::input::get_controller_button_down(0, asw::input::ControllerButton::A)) {
            asw::log::info("A pressed");
        }
        if (asw::input::get_controller_button_down(0, asw::input::ControllerButton::B)) {
            asw::log::info("B pressed");
        }
        if (asw::input::get_controller_button_down(0, asw::input::ControllerButton::X)) {
            asw::log::info("X pressed");
        }
        if (asw::input::get_controller_button_down(0, asw::input::ControllerButton::Y)) {
            asw::log::info("Y pressed");
        }
        if (asw::input::get_controller_button_down(0, asw::input::ControllerButton::Start)) {
            asw::log::info("Start pressed");
        }

        // --- Draw ---
        asw::display::clear(asw::color::darkslategray);

        const int count = asw::input::get_controller_count();
        if (count == 0) {
            // No controller – show a simple message indicator
            asw::draw::rect_fill({ 250.0F, 270.0F, 300.0F, 60.0F }, asw::color::darkred);
            asw::draw::rect({ 250.0F, 270.0F, 300.0F, 60.0F }, asw::color::red);
        } else {
            // Read axes for controller 0
            const float lx = asw::input::get_controller_axis(0, asw::input::ControllerAxis::LeftX);
            const float ly = asw::input::get_controller_axis(0, asw::input::ControllerAxis::LeftY);
            const float rx = asw::input::get_controller_axis(0, asw::input::ControllerAxis::RightX);
            const float ry = asw::input::get_controller_axis(0, asw::input::ControllerAxis::RightY);
            const float lt
                = asw::input::get_controller_axis(0, asw::input::ControllerAxis::LeftTrigger);
            const float rt
                = asw::input::get_controller_axis(0, asw::input::ControllerAxis::RightTrigger);

            // Sticks
            draw_stick({ 200.0F, 350.0F }, 80.0F, lx, ly);
            draw_stick({ 500.0F, 420.0F }, 60.0F, rx, ry);

            // Triggers
            draw_trigger({ 100.0F, 480.0F }, { 120.0F, 20.0F }, (lt + 1.0F) / 2.0F);
            draw_trigger({ 580.0F, 480.0F }, { 120.0F, 20.0F }, (rt + 1.0F) / 2.0F);

            // Face buttons (A/B/X/Y) - diamond layout
            constexpr float bs = 36.0F;
            const asw::Vec2<float> face { 570.0F, 260.0F };
            draw_button({ face.x + bs, face.y }, bs, asw::color::yellow,
                asw::input::get_controller_button(0, asw::input::ControllerButton::Y));
            draw_button({ face.x, face.y + bs }, bs, asw::color::blue,
                asw::input::get_controller_button(0, asw::input::ControllerButton::X));
            draw_button({ face.x + bs * 2, face.y + bs }, bs, asw::color::red,
                asw::input::get_controller_button(0, asw::input::ControllerButton::B));
            draw_button({ face.x + bs, face.y + bs * 2 }, bs, asw::color::lime,
                asw::input::get_controller_button(0, asw::input::ControllerButton::A));

            // Shoulder buttons
            draw_button({ 100.0F, 200.0F }, bs, asw::color::purple,
                asw::input::get_controller_button(0, asw::input::ControllerButton::LeftShoulder));
            draw_button({ 664.0F, 200.0F }, bs, asw::color::purple,
                asw::input::get_controller_button(0, asw::input::ControllerButton::RightShoulder));

            // D-Pad
            const asw::Vec2<float> dpad { 310.0F, 420.0F };
            draw_button({ dpad.x + bs, dpad.y }, bs, asw::color::silver,
                asw::input::get_controller_button(0, asw::input::ControllerButton::DPadUp));
            draw_button({ dpad.x, dpad.y + bs }, bs, asw::color::silver,
                asw::input::get_controller_button(0, asw::input::ControllerButton::DPadLeft));
            draw_button({ dpad.x + bs * 2, dpad.y + bs }, bs, asw::color::silver,
                asw::input::get_controller_button(0, asw::input::ControllerButton::DPadRight));
            draw_button({ dpad.x + bs, dpad.y + bs * 2 }, bs, asw::color::silver,
                asw::input::get_controller_button(0, asw::input::ControllerButton::DPadDown));

            // Start / Back
            draw_button({ 370.0F, 260.0F }, bs, asw::color::silver,
                asw::input::get_controller_button(0, asw::input::ControllerButton::Start));
            draw_button({ 310.0F, 260.0F }, bs, asw::color::silver,
                asw::input::get_controller_button(0, asw::input::ControllerButton::Back));
        }

        asw::display::present();
    }

    return 0;
}
