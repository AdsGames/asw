/// @file main.cpp
/// @brief Drawing primitives example
///
/// Demonstrates:
///   - asw::draw::clear_color()
///   - asw::draw::point(), line(), rect(), rect_fill(), circle(), circle_fill()
///   - asw::draw::rect() with a thickness, and rect_fill_rotate()
///   - asw::draw::text() and text_shadow()
///   - asw::color constants and Color helpers (lighten, darken, with_alpha)
///   - Alpha blending of overlapping shapes
///
/// Controls:
///   Space  - pause the animation
///   Escape - quit
///
/// Scripted run:
///   ASW_EXAMPLE_AUTORUN=1 SDL_VIDEO_DRIVER=dummy ./example_primitives

#include <asw/asw.h>
#include <cmath>
#include <cstdlib>
#include <numbers>

namespace {
constexpr float TAU = std::numbers::pi_v<float> * 2.0F;

void label(const asw::Font& font, const asw::Vec2<float>& pos, const std::string& text)
{
    asw::draw::text(font, text, pos, asw::color::lightgray);
}
} // namespace

int main()
{
    asw::core::init(800, 600);
    asw::display::set_title("ASW Example - Primitives");

    const bool autorun = std::getenv("ASW_EXAMPLE_AUTORUN") != nullptr;
    const auto font = asw::assets::load_font("assets/font.ttf", 8.0F, asw::FontStyle::Pixel);

    float angle = 0.0F;
    bool paused = false;
    int frame = 0;

    asw::core::run([&]() {
        asw::core::update();
        const float dt = autorun ? 1.0F / 60.0F : asw::core::get_delta_time();

        if (asw::input::get_key_down(asw::input::Key::Escape)) {
            asw::core::exit();
        }
        if (asw::input::get_key_down(asw::input::Key::Space)) {
            paused = !paused;
        }
        if (!paused) {
            angle += 1.2F * dt; // radians per second
        }

        asw::draw::clear_color(asw::Color(24, 28, 36));

        // --- Points ---
        label(font, { 40.0F, 20.0F }, "point");
        for (int i = 0; i < 40; ++i) {
            const float x = 40.0F + (static_cast<float>(i) * 8.0F);
            const float y = 42.0F + (std::sin(angle * 3.0F + static_cast<float>(i) * 0.4F) * 6.0F);
            asw::draw::point({ x, y }, asw::color::white);
        }

        // --- Lines ---
        label(font, { 40.0F, 64.0F }, "line");
        asw::draw::line({ 40.0F, 80.0F }, { 350.0F, 80.0F }, asw::color::lime);
        asw::draw::line({ 40.0F, 90.0F }, { 350.0F, 90.0F }, asw::color::lime.darken(0.4F));

        const asw::Vec2<float> hub { 480.0F, 90.0F };
        constexpr int SPOKES = 12;
        for (int i = 0; i < SPOKES; ++i) {
            const float a = angle + (static_cast<float>(i) * (TAU / SPOKES));
            asw::draw::line(hub, hub + (asw::Vec2<float>(std::cos(a), std::sin(a)) * 60.0F),
                asw::Color(200, 200, static_cast<uint8_t>(128 + (127 * std::sin(a)))));
        }

        // --- Rectangles ---
        label(font, { 40.0F, 124.0F }, "rect thickness 6 / rect_fill");
        asw::draw::rect({ 40.0F, 140.0F, 120.0F, 70.0F }, asw::color::red, 6.0F);
        asw::draw::rect_fill({ 190.0F, 140.0F, 120.0F, 70.0F }, asw::color::red.darken(0.3F));
        asw::draw::rect({ 190.0F, 140.0F, 120.0F, 70.0F }, asw::color::red);

        // --- Colour helpers ---
        label(font, { 40.0F, 228.0F }, "darken <  base  > lighten");
        for (int i = 0; i < 9; ++i) {
            const float t = static_cast<float>(i - 4) / 4.0F;
            const auto c = t < 0.0F ? asw::color::cornflowerblue.darken(-t * 0.8F)
                                    : asw::color::cornflowerblue.lighten(t * 0.8F);
            asw::draw::rect_fill({ 40.0F + (static_cast<float>(i) * 34.0F), 244.0F, 30.0F, 30.0F }, c);
        }

        // --- Circles ---
        label(font, { 40.0F, 296.0F }, "circle / circle_fill");
        asw::draw::circle({ 100.0F, 370.0F }, 50.0F, asw::color::yellow);
        asw::draw::circle_fill({ 230.0F, 370.0F }, 50.0F, asw::color::yellow.darken(0.3F));
        asw::draw::circle({ 230.0F, 370.0F }, 50.0F, asw::color::yellow);

        // Spins around its centre
        label(font, { 320.0F, 296.0F }, "rect_fill_rotate");
        asw::draw::rect_fill_rotate(
            { 325.0F, 350.0F, 70.0F, 40.0F }, angle, asw::color::mediumseagreen);

        // Concentric rings, fainter towards the edge, with an orbiting moon
        const asw::Vec2<float> rings { 560.0F, 300.0F };
        for (int i = 5; i >= 1; --i) {
            const auto alpha = static_cast<uint8_t>(30 * (6 - i));
            asw::draw::circle_fill(
                rings, static_cast<float>(i) * 22.0F, asw::color::cornflowerblue.with_alpha(alpha));
        }
        asw::draw::circle(rings, 110.0F, asw::color::white);
        const auto moon = rings + (asw::Vec2<float>(std::cos(angle * 2.0F), std::sin(angle * 2.0F)) * 90.0F);
        asw::draw::circle_fill(moon, 14.0F, asw::color::orange);
        asw::draw::circle(moon, 14.0F, asw::color::white);

        // --- Alpha blending ---
        label(font, { 40.0F, 444.0F }, "with_alpha");
        asw::draw::rect_fill({ 40.0F, 460.0F, 300.0F, 100.0F }, asw::color::purple);
        for (int i = 0; i < 6; ++i) {
            const auto a = static_cast<uint8_t>(40 + (i * 40));
            asw::draw::rect_fill(
                { 50.0F + (static_cast<float>(i) * 48.0F), 470.0F, 40.0F, 80.0F },
                asw::color::white.with_alpha(a));
        }

        // Overlapping translucent circles
        const asw::Vec2<float> venn { 560.0F, 510.0F };
        for (int i = 0; i < 3; ++i) {
            const float a = angle + (static_cast<float>(i) * (TAU / 3.0F));
            const asw::Color colors[] = { asw::color::red, asw::color::lime, asw::color::dodgerblue };
            asw::draw::circle_fill(venn + (asw::Vec2<float>(std::cos(a), std::sin(a)) * 28.0F),
                48.0F, colors[i].with_alpha(110));
        }

        // --- Text ---
        asw::draw::text_shadow(font, "text_shadow",
            asw::Vec2<float>(250.0F, 171.0F), asw::color::white, asw::color::black,
            asw::Vec2<float>(2.0F, 2.0F), asw::TextJustify::Center);
        asw::draw::text_shadow(font, paused ? "Paused (Space)" : "Space: pause",
            asw::Vec2<float>(790.0F, 10.0F), asw::color::white, asw::color::black,
            asw::Vec2<float>(2.0F, 2.0F), asw::TextJustify::Right);

        // Border
        const auto win = asw::display::get_logical_size();
        asw::draw::rect(
            { 2.0F, 2.0F, static_cast<float>(win.x) - 4.0F, static_cast<float>(win.y) - 4.0F },
            asw::color::gray);

        if (autorun && frame == 30) {
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
