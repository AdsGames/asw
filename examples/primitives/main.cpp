/// @file main.cpp
/// @brief Drawing primitives example
///
/// Demonstrates:
///   - asw::draw::point()
///   - asw::draw::line()
///   - asw::draw::rect() and rect_fill()
///   - asw::draw::circle() and circle_fill()
///   - asw::draw::clear_color()
///   - asw::color constants and Color utilities (lighten, darken, with_alpha)
///   - asw::display::clear(), present()
///
/// Controls:
///   Escape - quit

#include <asw/asw.h>
#include <cmath>

int main()
{
    asw::core::init(800, 600);
    asw::display::set_title("ASW Example - Primitives");
    asw::core::print_info();

    float angle = 0.0F;

    while (!asw::core::is_exiting()) {
        asw::core::update();

        if (asw::input::get_key_down(asw::input::Key::Escape)) {
            asw::core::exit();
        }

        angle += 0.02F;

        asw::display::clear(asw::color::darkslategray);

        // --- Points ---
        for (int i = 0; i < 20; ++i) {
            const float x = 40.0F + (static_cast<float>(i) * 8.0F);
            asw::draw::point({ x, 30.0F }, asw::color::white);
        }

        // --- Lines ---
        asw::draw::line({ 50.0F, 60.0F }, { 350.0F, 60.0F }, asw::color::lime);
        asw::draw::line({ 50.0F, 80.0F }, { 350.0F, 80.0F }, asw::color::lime.darken(0.4F));

        // Animated spoke lines from a centre point
        const asw::Vec2<float> hub { 400.0F, 150.0F };
        constexpr int spokes = 8;
        for (int i = 0; i < spokes; ++i) {
            const float a = angle + (static_cast<float>(i) * (3.14159265F * 2.0F / spokes));
            const float len = 60.0F;
            asw::draw::line(hub, { hub.x + std::cos(a) * len, hub.y + std::sin(a) * len },
                asw::Color(200, 200, static_cast<uint8_t>(128 + (127 * std::sin(a)))));
        }

        // --- Rectangles ---
        asw::draw::rect({ 50.0F, 120.0F, 120.0F, 70.0F }, asw::color::red);
        asw::draw::rect_fill({ 200.0F, 120.0F, 120.0F, 70.0F }, asw::color::red.darken(0.3F));
        asw::draw::rect({ 200.0F, 120.0F, 120.0F, 70.0F }, asw::color::red);

        // Colour gradient row of filled rects
        for (int i = 0; i < 8; ++i) {
            const float t = static_cast<float>(i) / 7.0F;
            const asw::Color c { static_cast<uint8_t>(255 * t), static_cast<uint8_t>(100),
                static_cast<uint8_t>(255 * (1.0F - t)) };
            asw::draw::rect_fill(
                { 50.0F + (static_cast<float>(i) * 40.0F), 220.0F, 35.0F, 35.0F }, c);
        }

        // --- Circles ---
        asw::draw::circle({ 100.0F, 340.0F }, 50.0F, asw::color::yellow);
        asw::draw::circle_fill({ 220.0F, 340.0F }, 50.0F, asw::color::yellow.darken(0.3F));
        asw::draw::circle({ 220.0F, 340.0F }, 50.0F, asw::color::yellow);

        // Concentric rings
        const asw::Vec2<float> rings { 400.0F, 400.0F };
        for (int i = 5; i >= 1; --i) {
            const float r = static_cast<float>(i) * 25.0F;
            const auto alpha = static_cast<uint8_t>(255 * (static_cast<float>(i) / 5.0F));
            asw::draw::circle_fill(rings, r, asw::color::cornflowerblue.with_alpha(alpha));
        }
        asw::draw::circle(rings, 125.0F, asw::color::white);

        // Animated orbit circle
        const float ox = rings.x + (std::cos(angle * 2.0F) * 100.0F);
        const float oy = rings.y + (std::sin(angle * 2.0F) * 100.0F);
        asw::draw::circle_fill({ ox, oy }, 15.0F, asw::color::orange);
        asw::draw::circle({ ox, oy }, 15.0F, asw::color::white);

        // --- Alpha / transparency demo ---
        asw::draw::rect_fill({ 550.0F, 100.0F, 200.0F, 200.0F }, asw::color::purple);
        for (int i = 0; i < 5; ++i) {
            const auto a = static_cast<uint8_t>(50 + i * 40);
            asw::draw::rect_fill(
                { 560.0F + (static_cast<float>(i) * 30.0F), 110.0F, 25.0F, 180.0F },
                asw::color::white.with_alpha(a));
        }

        // Border
        const auto win = asw::display::get_logical_size();
        asw::draw::rect(
            { 2.0F, 2.0F, static_cast<float>(win.x) - 4.0F, static_cast<float>(win.y) - 4.0F },
            asw::color::gray);

        asw::display::present();
    }

    asw::core::cleanup();

    return 0;
}
