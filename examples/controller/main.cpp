/// @file main.cpp
/// @brief Controller / gamepad input example
///
/// Demonstrates:
///   - get_controller_count() and get_controller_name(), checked every frame
///     so controllers can be plugged in and out while running
///   - get_controller_stick() for sticks, with a radial dead zone
///   - get_controller_axis() for triggers (0 to 1)
///   - get_controller_button() (held) and get_controller_button_down() (this
///     frame only)
///   - ANY_CONTROLLER to read every connected pad at once
///   - set_controller_dead_zone()
///   - get_last_device() to tell keyboard and controller players apart
///   - rumble_controller() and rumble_controller_triggers(), with
///     controller_has_rumble() and controller_has_trigger_rumble()
///
/// Controls:
///   Sticks, triggers, buttons - light up the matching part of the pad
///   A                         - short rumble
///   Triggers                  - trigger motors rumble as hard as each is pulled
///   Keyboard Escape           - quit
///
/// Scripted run:
///   ASW_EXAMPLE_AUTORUN=1 SDL_VIDEO_DRIVER=dummy ./example_controller
///   Shows the "no controller" screen, saves autorun.png and quits.

#include <algorithm>
#include <asw/asw.h>
#include <cstdlib>
#include <format>
#include <string>

namespace {
using asw::input::ControllerAxis;
using asw::input::ControllerButton;
using asw::input::ControllerStick;

// Every connected pad drives the display, so any of them can be used
constexpr uint32_t PAD = asw::input::ANY_CONTROLLER;
constexpr float DEAD_ZONE = 0.15F;
constexpr float FLASH_TIME = 0.3F;
constexpr uint32_t RUMBLE_MS = 250;

// Long enough to last until the next frame renews it
constexpr uint32_t TRIGGER_RUMBLE_MS = 100;

// Layout, in logical pixels
constexpr float STICK_RADIUS = 42.0F;
constexpr float FACE_RADIUS = 15.0F;
constexpr float FACE_SPACING = 32.0F;
constexpr float DPAD_SIZE = 26.0F;
constexpr float MENU_RADIUS = 11.0F;

const asw::Color BACKGROUND(24, 28, 36);
const asw::Color PANEL(16, 19, 25);
const asw::Color BODY(52, 58, 72);
const asw::Color BODY_SHADOW(12, 14, 18);
const asw::Color WELL(34, 38, 48);

// Pad body built from overlapping circles and a rect, with a drop shadow
void draw_body(const asw::Vec2<float>& offset, asw::Color color)
{
    const auto at = [&](float x, float y) { return asw::Vec2<float>(x, y) + offset; };
    asw::draw::circle_fill(at(280.0F, 330.0F), 110.0F, color);
    asw::draw::circle_fill(at(520.0F, 330.0F), 110.0F, color);
    asw::draw::circle_fill(at(235.0F, 410.0F), 80.0F, color);
    asw::draw::circle_fill(at(565.0F, 410.0F), 80.0F, color);
    asw::draw::rect_fill({ at(280.0F, 220.0F), { 240.0F, 210.0F } }, color);
}

// Stick: well, dead zone ring and a dot at the axis position.
// The dot turns white while the stick is clicked in.
void draw_stick(const asw::Vec2<float>& center, const asw::Vec2<float>& stick, bool clicked)
{
    const asw::Vec2<float> tip = center + (stick * STICK_RADIUS);
    asw::draw::circle_fill(center, STICK_RADIUS + 6.0F, BODY_SHADOW);
    asw::draw::circle_fill(center, STICK_RADIUS, WELL);
    asw::draw::circle(center, STICK_RADIUS, asw::color::gray);
    asw::draw::circle(center, STICK_RADIUS * DEAD_ZONE, asw::Color(90, 96, 110));
    asw::draw::line(center, tip, asw::color::cyan);
    asw::draw::circle_fill(tip, 12.0F, clicked ? asw::color::white : asw::color::cyan);
}

// Trigger: labelled bar that fills from 0 to 1
void draw_trigger(
    const asw::Font& font, const asw::Quad<float>& area, float value, const std::string& label)
{
    const float v = std::clamp(value, 0.0F, 1.0F);
    asw::draw::rect_fill(area, WELL);
    asw::draw::rect_fill({ area.position, { area.size.x * v, area.size.y } }, asw::color::orange);
    asw::draw::rect(area, asw::color::gray);
    asw::draw::text(font, std::format("{} {:.2f}", label, v),
        area.position - asw::Vec2<float>(0.0F, 14.0F), asw::color::white);
}

// Flash ring that shrinks and fades after a button goes down
void draw_flash(const asw::Vec2<float>& center, float size, float flash)
{
    if (flash > 0.0F) {
        const float t = flash / FLASH_TIME;
        asw::draw::circle(center, size * (1.6F - (t * 0.5F)),
            asw::color::white.with_alpha(static_cast<uint8_t>(255.0F * t)));
    }
}

// Rectangular button: lit while held
void draw_button(const asw::Font& font, const asw::Quad<float>& area, const std::string& label,
    asw::Color on, ControllerButton button, float flash = 0.0F)
{
    const bool held = asw::input::get_controller_button(PAD, button);
    asw::draw::rect_fill(area, held ? on : on.darken(0.7F));
    asw::draw::rect(area, held ? asw::color::white : asw::color::gray);
    draw_flash(area.get_center(), area.size.y * 0.5F, flash);
    asw::draw::text(font, label, area.get_center() - asw::Vec2<float>(0.0F, 4.0F),
        held ? asw::color::black : asw::color::white, asw::TextJustify::Center);
}

// Round button: lit while held
void draw_round_button(const asw::Font& font, const asw::Vec2<float>& center, float radius,
    const std::string& label, asw::Color on, ControllerButton button, float flash = 0.0F)
{
    const bool held = asw::input::get_controller_button(PAD, button);
    asw::draw::circle_fill(center, radius, held ? on : on.darken(0.7F));
    asw::draw::circle(center, radius, held ? asw::color::white : asw::color::gray);
    draw_flash(center, radius, flash);
    asw::draw::text(font, label, center - asw::Vec2<float>(0.0F, 4.0F),
        held ? asw::color::black : asw::color::white, asw::TextJustify::Center);
}
} // namespace

int main()
{
    asw::core::init(800, 600);
    asw::display::set_title("ASW Example - Controller");

    // ANY_CONTROLLER also sets the dead zone of pads plugged in later
    asw::input::set_controller_dead_zone(PAD, DEAD_ZONE);

    const bool autorun = std::getenv("ASW_EXAMPLE_AUTORUN") != nullptr;
    const auto font = asw::assets::load_font("assets/font.ttf", 8.0F, asw::FontStyle::Pixel);

    std::string last_pressed = "-";
    float flash = 0.0F;
    int frame = 0;

    asw::core::run([&]() {
        asw::core::update();
        const float dt = autorun ? 1.0F / 60.0F : asw::core::get_delta_time();

        if (asw::input::get_key_down(asw::input::Key::Escape)) {
            asw::core::exit();
        }

        asw::display::clear(BACKGROUND);

        // Info bar
        asw::draw::rect_fill({ 0.0F, 0.0F, 800.0F, 58.0F }, PANEL);
        asw::draw::line({ 0.0F, 58.0F }, { 800.0F, 58.0F }, asw::Color(60, 66, 80));

        const int count = asw::input::get_controller_count();
        asw::draw::text(
            font, std::format("Controllers: {}", count), { 12.0F, 10.0F }, asw::color::white);

        const bool on_pad = asw::input::get_last_device() == asw::input::InputDevice::Controller;
        asw::draw::text(font, on_pad ? "Last used: controller" : "Last used: keyboard / mouse",
            { 788.0F, 10.0F }, on_pad ? asw::color::lime : asw::color::lightgray,
            asw::TextJustify::Right);

        if (count == 0) {
            draw_body({ 6.0F, 8.0F }, BODY_SHADOW);
            draw_body({ 0.0F, 0.0F }, WELL);
            asw::draw::text_shadow(font, "No controller found - plug one in", { 400.0F, 324.0F },
                asw::color::orange, asw::color::black, { 2.0F, 2.0F }, asw::TextJustify::Center);
        } else {
            // Remember the last face button pressed this frame
            const std::pair<ControllerButton, const char*> named[] = {
                { ControllerButton::A, "A" },
                { ControllerButton::B, "B" },
                { ControllerButton::X, "X" },
                { ControllerButton::Y, "Y" },
                { ControllerButton::Start, "Start" },
                { ControllerButton::Back, "Back" },
            };
            for (const auto& [button, name] : named) {
                if (asw::input::get_controller_button_down(PAD, button)) {
                    last_pressed = name;
                    flash = FLASH_TIME;
                }
            }
            flash = std::max(0.0F, flash - dt);
            const auto face_flash
                = [&](const char* name) { return last_pressed == name ? flash : 0.0F; };

            // Rumble: A gives a short burst, and each trigger's motor follows
            // how far that trigger is pulled
            const bool has_rumble = asw::input::controller_has_rumble(PAD);
            const bool has_trigger_rumble = asw::input::controller_has_trigger_rumble(PAD);
            if (has_rumble && asw::input::get_controller_button_down(PAD, ControllerButton::A)) {
                asw::input::rumble_controller(PAD, 0.8F, 0.4F, RUMBLE_MS);
            }
            if (has_trigger_rumble) {
                asw::input::rumble_controller_triggers(PAD,
                    asw::input::get_controller_axis(PAD, ControllerAxis::LeftTrigger),
                    asw::input::get_controller_axis(PAD, ControllerAxis::RightTrigger),
                    TRIGGER_RUMBLE_MS);
            }

            asw::draw::text(font, "Pad 0: " + asw::input::get_controller_name(0), { 12.0F, 24.0F },
                asw::color::white);
            asw::draw::text(
                font, "Last pressed: " + last_pressed, { 12.0F, 38.0F }, asw::color::white);
            asw::draw::text(font,
                std::format("Rumble (A): {}  Trigger rumble: {}", has_rumble ? "yes" : "no",
                    has_trigger_rumble ? "yes" : "no"),
                { 788.0F, 38.0F }, asw::color::lightgray, asw::TextJustify::Right);

            // Triggers, 0 released to 1 fully pulled, above the shoulders
            draw_trigger(font, { 170.0F, 140.0F, 150.0F, 16.0F },
                asw::input::get_controller_axis(PAD, ControllerAxis::LeftTrigger), "LT");
            draw_trigger(font, { 480.0F, 140.0F, 150.0F, 16.0F },
                asw::input::get_controller_axis(PAD, ControllerAxis::RightTrigger), "RT");
            draw_button(font, { 170.0F, 176.0F, 150.0F, 24.0F }, "LB", asw::color::purple,
                ControllerButton::LeftShoulder);
            draw_button(font, { 480.0F, 176.0F, 150.0F, 24.0F }, "RB", asw::color::purple,
                ControllerButton::RightShoulder);

            draw_body({ 6.0F, 8.0F }, BODY_SHADOW);
            draw_body({ 0.0F, 0.0F }, BODY);

            // Left stick up top, D-pad below it, like most modern pads
            const asw::Vec2<float> left_stick { 260.0F, 300.0F };
            const asw::Vec2<float> right_stick { 470.0F, 385.0F };
            draw_stick(left_stick, asw::input::get_controller_stick(PAD, ControllerStick::Left),
                asw::input::get_controller_button(PAD, ControllerButton::LeftStick));
            draw_stick(right_stick, asw::input::get_controller_stick(PAD, ControllerStick::Right),
                asw::input::get_controller_button(PAD, ControllerButton::RightStick));

            // D-pad as a cross of four arms around a center square
            const asw::Vec2<float> dpad { 330.0F, 385.0F };
            const float h = DPAD_SIZE * 0.5F;
            const asw::Vec2<float> arm { DPAD_SIZE, DPAD_SIZE };
            asw::draw::rect_fill({ dpad - asw::Vec2<float>(h, h), arm }, WELL);
            draw_button(font, { dpad + asw::Vec2<float>(-h, -h - DPAD_SIZE), arm }, "^",
                asw::color::silver, ControllerButton::DPadUp);
            draw_button(font, { dpad + asw::Vec2<float>(-h - DPAD_SIZE, -h), arm }, "<",
                asw::color::silver, ControllerButton::DPadLeft);
            draw_button(font, { dpad + asw::Vec2<float>(h, -h), arm }, ">", asw::color::silver,
                ControllerButton::DPadRight);
            draw_button(font, { dpad + asw::Vec2<float>(-h, h), arm }, "v", asw::color::silver,
                ControllerButton::DPadDown);

            // Face buttons in a diamond
            const asw::Vec2<float> face { 545.0F, 295.0F };
            draw_round_button(font, face + asw::Vec2<float>(0.0F, -FACE_SPACING), FACE_RADIUS, "Y",
                asw::color::yellow, ControllerButton::Y, face_flash("Y"));
            draw_round_button(font, face + asw::Vec2<float>(-FACE_SPACING, 0.0F), FACE_RADIUS, "X",
                asw::color::dodgerblue, ControllerButton::X, face_flash("X"));
            draw_round_button(font, face + asw::Vec2<float>(FACE_SPACING, 0.0F), FACE_RADIUS, "B",
                asw::color::red, ControllerButton::B, face_flash("B"));
            draw_round_button(font, face + asw::Vec2<float>(0.0F, FACE_SPACING), FACE_RADIUS, "A",
                asw::color::lime, ControllerButton::A, face_flash("A"));

            // Back / Start between the sticks
            draw_round_button(font, { 360.0F, 285.0F }, MENU_RADIUS, "Bk", asw::color::silver,
                ControllerButton::Back, face_flash("Back"));
            draw_round_button(font, { 440.0F, 285.0F }, MENU_RADIUS, "St", asw::color::silver,
                ControllerButton::Start, face_flash("Start"));

            // Stick readouts below the pad, clear of the drawing
            const auto l = asw::input::get_controller_stick(PAD, ControllerStick::Left);
            const auto r = asw::input::get_controller_stick(PAD, ControllerStick::Right);
            asw::draw::text(font,
                std::format(
                    "L stick {:+.2f} {:+.2f}      R stick {:+.2f} {:+.2f}", l.x, l.y, r.x, r.y),
                { 400.0F, 530.0F }, asw::color::lightgray, asw::TextJustify::Center);
        }

        if (autorun && frame == 5) {
            const bool saved = asw::display::screenshot("autorun.png");
            asw::log::info(saved ? "Saved autorun.png" : "Screenshot failed");
            asw::log::info("Controllers: {}", count);
            asw::core::exit();
        }

        asw::display::present();
        frame++;
    });

    asw::core::shutdown();
    return 0;
}
