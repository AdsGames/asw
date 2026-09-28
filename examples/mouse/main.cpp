/// @file main.cpp
/// @brief Mouse input example
///
/// Demonstrates:
///   - Mouse position and movement (get_mouse().position / .change)
///   - get_mouse_button() (held), get_mouse_button_down() and
///     get_mouse_button_up() (this frame only)
///   - Scroll wheel via get_mouse().z
///   - set_cursor() to change the system cursor, set_cursor_visible() to hide it
///
/// Controls:
///   Move mouse   - move the cursor circle, leaves a trail
///   Left click   - red ripple, crosshair cursor while held
///   Right click  - blue ripple, move cursor while held
///   Middle click - hide / show the system cursor
///   Scroll wheel - resize the circle
///   Escape       - quit
///
/// Scripted run:
///   ASW_EXAMPLE_AUTORUN=1 SDL_VIDEO_DRIVER=dummy ./example_mouse

#include <algorithm>
#include <asw/asw.h>
#include <cmath>
#include <cstdlib>
#include <deque>
#include <format>
#include <vector>

namespace {
constexpr float RIPPLE_TIME = 0.6F;
constexpr std::size_t TRAIL_LENGTH = 24;

struct Ripple {
    asw::Vec2<float> position;
    asw::Color color;
    float age { 0.0F };
};

// Button indicator with a label
void draw_button(const asw::Font& font, const asw::Quad<float>& area, const std::string& label,
    asw::Color on, bool held)
{
    asw::draw::rect_fill(area, held ? on : on.darken(0.7F));
    asw::draw::rect(area, asw::color::gray);
    asw::draw::text(font, label, area.get_center() - asw::Vec2<float>(0.0F, 4.0F),
        asw::color::white, asw::TextJustify::Center);
}
} // namespace

int main()
{
    asw::core::init(800, 600);
    asw::display::set_title("ASW Example - Mouse");

    const bool autorun = std::getenv("ASW_EXAMPLE_AUTORUN") != nullptr;
    const auto font = asw::assets::load_font("assets/font.ttf", 8.0F, asw::FontStyle::Pixel);

    float radius = 30.0F;
    float wheel_total = 0.0F;
    bool cursor_visible = true;
    std::vector<Ripple> ripples;
    std::deque<asw::Vec2<float>> trail;
    int frame = 0;

    while (!asw::core::is_exiting()) {
        // Scripted input: sweep across, left click, right click
        if (autorun) {
            const float t = static_cast<float>(frame) / 40.0F;
            asw::input::simulate_mouse_move(
                asw::Vec2<float>(200.0F + (400.0F * t), 300.0F + (std::sin(t * 6.0F) * 80.0F)));
            if (frame == 15) {
                asw::input::simulate_mouse_button_down(asw::input::MouseButton::Left);
            }
            if (frame == 18) {
                asw::input::simulate_mouse_button_up(asw::input::MouseButton::Left);
            }
            if (frame == 30) {
                asw::input::simulate_mouse_button_down(asw::input::MouseButton::Right);
            }
        }

        asw::core::update();
        const float dt = autorun ? 1.0F / 60.0F : asw::core::get_delta_time();
        const auto& mouse = asw::input::get_mouse();

        if (asw::input::get_key_down(asw::input::Key::Escape)) {
            asw::core::exit();
        }

        // --- Button events, once per click ---
        if (asw::input::get_mouse_button_down(asw::input::MouseButton::Left)) {
            ripples.push_back({ mouse.position, asw::color::red });
        }
        if (asw::input::get_mouse_button_down(asw::input::MouseButton::Right)) {
            ripples.push_back({ mouse.position, asw::color::dodgerblue });
        }
        if (asw::input::get_mouse_button_down(asw::input::MouseButton::Middle)) {
            cursor_visible = !cursor_visible;
            asw::input::set_cursor_visible(cursor_visible);
        }

        // --- Scroll wheel ---
        if (mouse.z != 0.0F) {
            radius = std::clamp(radius + (mouse.z * 5.0F), 5.0F, 150.0F);
            wheel_total += mouse.z;
        }

        // --- Cursor style while a button is held ---
        if (asw::input::get_mouse_button(asw::input::MouseButton::Left)) {
            asw::input::set_cursor(asw::input::CursorId::Crosshair);
        } else if (asw::input::get_mouse_button(asw::input::MouseButton::Right)) {
            asw::input::set_cursor(asw::input::CursorId::Move);
        } else {
            asw::input::set_cursor(asw::input::CursorId::Default);
        }

        // Ripples and trail
        for (auto& r : ripples) {
            r.age += dt;
        }
        std::erase_if(ripples, [](const Ripple& r) { return r.age >= RIPPLE_TIME; });

        trail.push_back(mouse.position);
        if (trail.size() > TRAIL_LENGTH) {
            trail.pop_front();
        }

        // --- Draw ---
        asw::display::clear(asw::Color(24, 28, 36));

        const auto& mp = mouse.position;
        const auto win = asw::display::get_logical_size();
        asw::draw::line({ 0.0F, mp.y }, { static_cast<float>(win.x), mp.y }, asw::Color(50, 56, 70));
        asw::draw::line({ mp.x, 0.0F }, { mp.x, static_cast<float>(win.y) }, asw::Color(50, 56, 70));

        for (std::size_t i = 1; i < trail.size(); ++i) {
            const auto alpha = static_cast<uint8_t>(
                200.0F * static_cast<float>(i) / static_cast<float>(trail.size()));
            asw::draw::line(trail[i - 1], trail[i], asw::color::yellow.with_alpha(alpha));
        }

        for (const auto& r : ripples) {
            const float t = r.age / RIPPLE_TIME;
            asw::draw::circle(r.position, radius + (t * 80.0F),
                r.color.with_alpha(static_cast<uint8_t>(255.0F * (1.0F - t))));
        }

        const bool held = asw::input::get_mouse_button(asw::input::MouseButton::Left)
            || asw::input::get_mouse_button(asw::input::MouseButton::Right);
        asw::draw::circle_fill(mp, radius, (held ? asw::color::orange : asw::color::white).with_alpha(160));
        asw::draw::circle(mp, radius, asw::color::white);

        // Buttons
        draw_button(font, { 10.0F, 520.0F, 60.0F, 30.0F }, "L", asw::color::red,
            asw::input::get_mouse_button(asw::input::MouseButton::Left));
        draw_button(font, { 75.0F, 520.0F, 60.0F, 30.0F }, "M", asw::color::lime,
            asw::input::get_mouse_button(asw::input::MouseButton::Middle));
        draw_button(font, { 140.0F, 520.0F, 60.0F, 30.0F }, "R", asw::color::dodgerblue,
            asw::input::get_mouse_button(asw::input::MouseButton::Right));

        // Info
        const auto hud = [&](int line, const std::string& text) {
            asw::draw::text_shadow(font, text,
                asw::Vec2<float>(10.0F, 10.0F + (static_cast<float>(line) * 14.0F)),
                asw::color::white);
        };
        hud(0, std::format("Position: {:.0f}, {:.0f}", mp.x, mp.y));
        hud(1, std::format("Change:   {:.1f}, {:.1f}", mouse.change.x, mouse.change.y));
        hud(2, std::format("Wheel:    {:.1f}  radius {:.0f}", wheel_total, radius));
        hud(3, std::string("Cursor:   ") + (cursor_visible ? "shown" : "hidden (middle click)"));

        if (autorun && frame == 40) {
            const bool saved = asw::display::screenshot("autorun.png");
            asw::log::info(saved ? "Saved autorun.png" : "Screenshot failed");
            asw::log::info("Mouse: {:.0f}, {:.0f}  ripples: {}", mp.x, mp.y, ripples.size());
            asw::core::exit();
        }

        asw::display::present();
        frame++;
    }

    asw::input::set_cursor_visible(true);
    asw::core::shutdown();
    return 0;
}
