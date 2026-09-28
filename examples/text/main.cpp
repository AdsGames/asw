/// @file main.cpp
/// @brief Text drawing example
///
/// Demonstrates:
///   - asw::assets::load_font() with FontStyle::Pixel and FontStyle::Smooth
///   - Font caching with load_font(..., key) and get_font(key)
///   - asw::draw::text() with Left, Center and Right justification
///   - asw::draw::text_shadow()
///   - Fading text through the colour's alpha
///   - asw::util::get_text_size() to fit boxes around text
///   - asw::util::get_font_height() for line spacing
///   - asw::core::get_delta_time() for a live frame time readout
///
/// Controls:
///   Space - restart the typewriter
///   Escape - quit
///
/// Scripted run:
///   ASW_EXAMPLE_AUTORUN=1 SDL_VIDEO_DRIVER=dummy ./example_text
///   Lets the typewriter finish, saves autorun.png and quits.

#include <array>
#include <asw/asw.h>
#include <cmath>
#include <cstdlib>
#include <string>

namespace {
constexpr float SCREEN_W = 800.0F;

// Draws text with a padded box around it, sized by get_text_size
void boxed_text(const asw::Font& font, const std::string& text, const asw::Vec2<float>& position,
    asw::Color color, asw::Color box)
{
    constexpr float PAD = 6.0F;
    const auto size = asw::util::get_text_size(font, text);
    const asw::Quad<float> rect(position.x - PAD, position.y - PAD,
        static_cast<float>(size.x) + (PAD * 2.0F), static_cast<float>(size.y) + (PAD * 2.0F));

    asw::draw::rect_fill(rect, box);
    asw::draw::rect(rect, box.lighten(0.4F));
    asw::draw::text(font, text, position, color);
}
} // namespace

int main()
{
    asw::core::init(800, 600);
    asw::display::set_title("ASW Example - Text");

    const bool autorun = std::getenv("ASW_EXAMPLE_AUTORUN") != nullptr;

    // Pixel fonts look sharp at whole multiples of their design size
    asw::assets::load_font("assets/font.ttf", 8.0F, "small", asw::FontStyle::Pixel);
    asw::assets::load_font("assets/font.ttf", 16.0F, "medium", asw::FontStyle::Pixel);
    asw::assets::load_font("assets/font.ttf", 32.0F, "large", asw::FontStyle::Pixel);

    // The same file rendered smoothly, for comparison
    asw::assets::load_font("assets/font.ttf", 16.0F, "smooth", asw::FontStyle::Smooth);

    const auto small = asw::assets::get_font("small");
    const auto medium = asw::assets::get_font("medium");
    const auto large = asw::assets::get_font("large");
    const auto smooth = asw::assets::get_font("smooth");

    const std::string story
        = "The quick brown fox jumps over the lazy dog. Press space to type it again.";
    constexpr float CHARS_PER_SECOND = 24.0F;

    float time = 0.0F;
    float typed_time = 0.0F;
    float frame_ms = 0.0F;
    while (!asw::core::is_exiting()) {
        asw::core::update();
        const float dt = asw::core::get_delta_time();
        time += dt;
        typed_time += dt;

        // Smooth the readout so it does not flicker
        frame_ms += ((dt * 1000.0F) - frame_ms) * 0.05F;

        if (asw::input::get_key_down(asw::input::Key::Escape)) {
            asw::core::exit();
        }
        if (asw::input::get_key_down(asw::input::Key::Space)) {
            typed_time = 0.0F;
        }

        asw::display::clear(asw::Color(24, 26, 38));

        // --- Title with a drop shadow ---
        asw::draw::text_shadow(large, "ASW Text", { SCREEN_W / 2.0F, 24.0F }, asw::color::gold,
            asw::color::darkred, { 4.0F, 4.0F }, asw::TextJustify::Center);

        // --- Sizes ---
        asw::draw::text(small, "Pixel 8px", { 40.0F, 100.0F }, asw::color::white);
        asw::draw::text(medium, "Pixel 16px", { 40.0F, 120.0F }, asw::color::white);
        asw::draw::text(smooth, "Smooth 16px", { 40.0F, 148.0F }, asw::color::lightgray);

        // --- Justification against a guide line ---
        const float guide = 560.0F;
        asw::draw::line({ guide, 92.0F }, { guide, 176.0F }, asw::color::tomato);
        asw::draw::text(medium, "Left", { guide, 100.0F }, asw::color::white);
        asw::draw::text(
            medium, "Center", { guide, 124.0F }, asw::color::white, asw::TextJustify::Center);
        asw::draw::text(
            medium, "Right", { guide, 148.0F }, asw::color::white, asw::TextJustify::Right);

        // --- Boxes sized to their text ---
        const std::array<const char*, 3> tags = { "HP 100", "LEVEL 7", "GOLD 12,480" };
        const std::array<asw::Color, 3> tag_colors
            = { asw::color::firebrick, asw::color::royalblue, asw::color::darkgoldenrod };
        float x = 46.0F;
        for (std::size_t i = 0; i < tags.size(); ++i) {
            boxed_text(medium, tags[i], { x, 220.0F }, asw::color::white, tag_colors[i]);
            x += static_cast<float>(asw::util::get_text_size(medium, tags[i]).x) + 32.0F;
        }

        // --- Fading and colour cycling ---
        const auto pulse = static_cast<uint8_t>(127.5F + (127.5F * std::sin(time * 3.0F)));
        asw::draw::text(medium, "Fading in and out", { 40.0F, 290.0F },
            asw::color::aquamarine.with_alpha(pulse));

        const std::string wave = "Wavy letters";
        float wx = 440.0F;
        for (std::size_t i = 0; i < wave.size(); ++i) {
            const std::string letter(1, wave[i]);
            const float wy
                = 290.0F + (std::sin((time * 6.0F) + static_cast<float>(i) * 0.5F) * 6.0F);
            asw::draw::text(medium, letter, { wx, wy }, asw::color::hotpink);
            wx += static_cast<float>(asw::util::get_text_size(medium, letter).x);
        }

        // --- Typewriter in a dialog box ---
        const asw::Quad<float> dialog(30.0F, 380.0F, SCREEN_W - 60.0F, 110.0F);
        asw::draw::rect_fill(dialog, asw::Color(12, 14, 24, 230));
        asw::draw::rect(dialog, asw::color::white);

        const auto shown = std::min(
            story.size(), static_cast<std::size_t>(typed_time * CHARS_PER_SECOND));
        const std::string visible = story.substr(0, shown);

        // Break into lines that fit the box
        const float max_width = dialog.size.x - 40.0F;
        const float line_height = static_cast<float>(asw::util::get_font_height(medium)) * 1.5F;
        std::string line;
        float ly = dialog.position.y + 20.0F;
        std::size_t start = 0;
        while (start < visible.size()) {
            const auto end = visible.find(' ', start);
            const auto word
                = visible.substr(start, end == std::string::npos ? end : end - start + 1);
            const auto width = asw::util::get_text_size(medium, line + word).x;
            if (!line.empty() && static_cast<float>(width) > max_width) {
                asw::draw::text(medium, line, { dialog.position.x + 20.0F, ly }, asw::color::white);
                line.clear();
                ly += line_height;
            }
            line += word;
            start = end == std::string::npos ? visible.size() : end + 1;
        }
        asw::draw::text(medium, line, { dialog.position.x + 20.0F, ly }, asw::color::white);

        // Blinking prompt once the text is done
        if (shown == story.size() && std::fmod(time, 1.0F) < 0.5F) {
            asw::draw::text(medium, "v",
                { dialog.position.x + dialog.size.x - 20.0F, dialog.position.y + 80.0F },
                asw::color::white, asw::TextJustify::Right);
        }

        // --- Frame time, right aligned in the corner ---
        const auto ms = std::to_string(static_cast<int>(frame_ms * 10.0F) / 10) + "."
            + std::to_string(static_cast<int>(frame_ms * 10.0F) % 10) + " ms";
        asw::draw::text(small, ms, { SCREEN_W - 8.0F, 580.0F }, asw::color::gray,
            asw::TextJustify::Right);

        if (autorun && typed_time > 3.5F) {
            const bool saved = asw::display::screenshot("autorun.png");
            asw::log::info(saved ? "Saved autorun.png" : "Screenshot failed");
            asw::core::exit();
        }

        asw::display::present();
    }

    asw::core::shutdown();
    return 0;
}
