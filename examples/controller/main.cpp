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
///
/// Controls:
///   Sticks, triggers, buttons - light up the matching part of the pad
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
constexpr float BUTTON_SIZE = 36.0F;
constexpr float FLASH_TIME = 0.3F;

// Stick: range circle, dead zone ring and a dot at the axis position.
// The dot turns white while the stick is clicked in.
void draw_stick(const asw::Font& font, const asw::Vec2<float>& center, float radius,
    const asw::Vec2<float>& stick, bool clicked, const std::string& label)
{
    const float x = stick.x;
    const float y = stick.y;
    asw::draw::circle_fill(center, radius, asw::Color(40, 46, 58));
    asw::draw::circle(center, radius, asw::color::gray);
    asw::draw::circle(center, radius * DEAD_ZONE, asw::Color(90, 96, 110));
    asw::draw::line(center, center + (asw::Vec2<float>(x, y) * radius), asw::color::cyan);
    asw::draw::circle_fill(center + (asw::Vec2<float>(x, y) * radius), 10.0F,
        clicked ? asw::color::white : asw::color::cyan);
    asw::draw::text(font, std::format("{} {:+.2f} {:+.2f}", label, x, y),
        center + asw::Vec2<float>(0.0F, radius + 10.0F), asw::color::white,
        asw::TextJustify::Center);
}

// Trigger: bar that fills from 0 to 1
void draw_trigger(const asw::Font& font, const asw::Quad<float>& area, float value,
    const std::string& label)
{
    const float v = std::clamp(value, 0.0F, 1.0F);
    asw::draw::rect_fill(area, asw::Color(40, 46, 58));
    asw::draw::rect_fill({ area.position, { area.size.x * v, area.size.y } }, asw::color::orange);
    asw::draw::rect(area, asw::color::gray);
    asw::draw::text(font, std::format("{} {:.2f}", label, v),
        area.position - asw::Vec2<float>(0.0F, 12.0F), asw::color::white);
}

// Button: box lit while held, with a flash ring the frame it goes down
void draw_button(const asw::Font& font, const asw::Vec2<float>& pos, const std::string& label,
    asw::Color on, ControllerButton button, float flash)
{
    const bool held = asw::input::get_controller_button(PAD, button);
    const asw::Quad<float> area(pos, asw::Vec2<float>(BUTTON_SIZE, BUTTON_SIZE));
    asw::draw::rect_fill(area, held ? on : on.darken(0.75F));
    asw::draw::rect(area, asw::color::gray);
    if (flash > 0.0F) {
        asw::draw::circle(area.get_center(), BUTTON_SIZE * (1.2F - (flash / FLASH_TIME) * 0.5F),
            asw::color::white.with_alpha(static_cast<uint8_t>(255.0F * (flash / FLASH_TIME))));
    }
    asw::draw::text(font, label, area.get_center() - asw::Vec2<float>(0.0F, 4.0F),
        held ? asw::color::black : asw::color::white, asw::TextJustify::Center);
}
} // namespace

int main()
{
    asw::core::init(800, 600);
    asw::display::set_title("ASW Example - Controller");

    const bool autorun = std::getenv("ASW_EXAMPLE_AUTORUN") != nullptr;
    const auto font = asw::assets::load_font("assets/font.ttf", 8.0F, asw::FontStyle::Pixel);

    std::string last_pressed = "-";
    float flash = 0.0F;
    int frame = 0;

    while (!asw::core::is_exiting()) {
        asw::core::update();
        const float dt = autorun ? 1.0F / 60.0F : asw::core::get_delta_time();

        if (asw::input::get_key_down(asw::input::Key::Escape)) {
            asw::core::exit();
        }

        asw::display::clear(asw::Color(24, 28, 36));

        const int count = asw::input::get_controller_count();
        asw::draw::text_shadow(font, std::format("Controllers: {}", count),
            asw::Vec2<float>(10.0F, 10.0F), asw::color::white);

        const bool on_pad
            = asw::input::get_last_device() == asw::input::InputDevice::Controller;
        asw::draw::text_shadow(font,
            on_pad ? "Last used: controller" : "Last used: keyboard / mouse",
            asw::Vec2<float>(790.0F, 10.0F), on_pad ? asw::color::lime : asw::color::lightgray,
            asw::color::black, asw::Vec2<float>(2.0F, 2.0F), asw::TextJustify::Right);

        if (count == 0) {
            asw::draw::text_shadow(font, "No controller found - plug one in",
                asw::Vec2<float>(400.0F, 290.0F), asw::color::orange, asw::color::black,
                asw::Vec2<float>(2.0F, 2.0F), asw::TextJustify::Center);
        } else {
            // Dead zones are stored per pad, so this reaches pads plugged in later
            asw::input::set_controller_dead_zone(PAD, DEAD_ZONE);

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

            asw::draw::text_shadow(font, "Pad 0: " + asw::input::get_controller_name(0),
                asw::Vec2<float>(10.0F, 24.0F), asw::color::white);
            asw::draw::text_shadow(font, "Last pressed: " + last_pressed,
                asw::Vec2<float>(10.0F, 38.0F), asw::color::white);

            // Sticks
            draw_stick(font, { 220.0F, 360.0F }, 70.0F,
                asw::input::get_controller_stick(PAD, ControllerStick::Left),
                asw::input::get_controller_button(PAD, ControllerButton::LeftStick), "L");
            draw_stick(font, { 520.0F, 420.0F }, 55.0F,
                asw::input::get_controller_stick(PAD, ControllerStick::Right),
                asw::input::get_controller_button(PAD, ControllerButton::RightStick), "R");

            // Triggers, 0 released to 1 fully pulled
            draw_trigger(font, { 100.0F, 110.0F, 140.0F, 18.0F },
                asw::input::get_controller_axis(PAD, ControllerAxis::LeftTrigger), "LT");
            draw_trigger(font, { 560.0F, 110.0F, 140.0F, 18.0F },
                asw::input::get_controller_axis(PAD, ControllerAxis::RightTrigger), "RT");

            // Shoulders
            draw_button(font, { 100.0F, 150.0F }, "LB", asw::color::purple,
                ControllerButton::LeftShoulder, 0.0F);
            draw_button(font, { 664.0F, 150.0F }, "RB", asw::color::purple,
                ControllerButton::RightShoulder, 0.0F);

            // Face buttons in a diamond
            const asw::Vec2<float> face { 570.0F, 220.0F };
            const auto face_flash = [&](const char* name) {
                return last_pressed == name ? flash : 0.0F;
            };
            draw_button(font, { face.x + BUTTON_SIZE, face.y }, "Y", asw::color::yellow,
                ControllerButton::Y, face_flash("Y"));
            draw_button(font, { face.x, face.y + BUTTON_SIZE }, "X", asw::color::dodgerblue,
                ControllerButton::X, face_flash("X"));
            draw_button(font, { face.x + (BUTTON_SIZE * 2.0F), face.y + BUTTON_SIZE }, "B",
                asw::color::red, ControllerButton::B, face_flash("B"));
            draw_button(font, { face.x + BUTTON_SIZE, face.y + (BUTTON_SIZE * 2.0F) }, "A",
                asw::color::lime, ControllerButton::A, face_flash("A"));

            // D-pad
            const asw::Vec2<float> dpad { 90.0F, 220.0F };
            draw_button(font, { dpad.x + BUTTON_SIZE, dpad.y }, "^", asw::color::silver,
                ControllerButton::DPadUp, 0.0F);
            draw_button(font, { dpad.x, dpad.y + BUTTON_SIZE }, "<", asw::color::silver,
                ControllerButton::DPadLeft, 0.0F);
            draw_button(font, { dpad.x + (BUTTON_SIZE * 2.0F), dpad.y + BUTTON_SIZE }, ">",
                asw::color::silver, ControllerButton::DPadRight, 0.0F);
            draw_button(font, { dpad.x + BUTTON_SIZE, dpad.y + (BUTTON_SIZE * 2.0F) }, "v",
                asw::color::silver, ControllerButton::DPadDown, 0.0F);

            // Back / Start
            draw_button(font, { 340.0F, 230.0F }, "Bk", asw::color::silver, ControllerButton::Back,
                face_flash("Back"));
            draw_button(font, { 424.0F, 230.0F }, "St", asw::color::silver,
                ControllerButton::Start, face_flash("Start"));
        }

        if (autorun && frame == 5) {
            const bool saved = asw::display::screenshot("autorun.png");
            asw::log::info(saved ? "Saved autorun.png" : "Screenshot failed");
            asw::log::info("Controllers: {}", count);
            asw::core::exit();
        }

        asw::display::present();
        frame++;
    }

    asw::core::shutdown();
    return 0;
}
