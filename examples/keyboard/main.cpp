/// @file main.cpp
/// @brief Keyboard input example
///
/// Demonstrates:
///   - Polling keys with get_key() (held), get_key_down() (pressed), get_key_up() (released)
///   - Moving a box with WASD / arrow keys
///   - Detecting individual key events and logging them
///   - Reading text_input for typed characters
///
/// Controls:
///   WASD / Arrow keys - move the box
///   Escape            - quit

#include <asw/asw.h>

int main()
{
    asw::core::init(800, 600);
    asw::display::set_title("ASW Example - Keyboard");
    asw::core::print_info();

    asw::Vec2<float> pos { 375.0F, 275.0F };
    constexpr float speed = 3.0F;
    constexpr float box_size = 50.0F;

    while (!asw::core::is_exiting()) {
        asw::core::update();

        // --- Movement (held) ---
        if (asw::input::get_key(asw::input::Key::W) || asw::input::get_key(asw::input::Key::Up)) {
            pos.y -= speed;
        }
        if (asw::input::get_key(asw::input::Key::S) || asw::input::get_key(asw::input::Key::Down)) {
            pos.y += speed;
        }
        if (asw::input::get_key(asw::input::Key::A) || asw::input::get_key(asw::input::Key::Left)) {
            pos.x -= speed;
        }
        if (asw::input::get_key(asw::input::Key::D)
            || asw::input::get_key(asw::input::Key::Right)) {
            pos.x += speed;
        }

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

        // --- Key-press events (single frame) ---
        if (asw::input::get_key_down(asw::input::Key::Space)) {
            asw::log::info("Space pressed");
        }
        if (asw::input::get_key_up(asw::input::Key::Space)) {
            asw::log::info("Space released");
        }
        if (asw::input::get_key_down(asw::input::Key::Escape)) {
            asw::core::exit();
            break;
        }

        // --- Text input ---
        if (!asw::input::text_input.empty()) {
            asw::log::info("Text input: {}", asw::input::text_input);
        }

        // --- Draw ---
        asw::display::clear(asw::color::darkslategray);

        // Box - turns cyan when any key is held
        const bool any_held = asw::input::keyboard.any_pressed;
        asw::draw::rect_fill(
            { pos, { box_size, box_size } }, any_held ? asw::color::cyan : asw::color::white);
        asw::draw::rect({ pos, { box_size, box_size } }, asw::color::gray);

        // Indicator dots for WASD
        const asw::Vec2<float> center { 400.0F, 530.0F };
        constexpr float dot = 12.0F;
        const auto key_color
            = [](bool down) { return down ? asw::color::lime : asw::color::dimgray; };

        asw::draw::circle_fill({ center.x, center.y - 20.0F }, dot,
            key_color(asw::input::get_key(asw::input::Key::W)));
        asw::draw::circle_fill({ center.x, center.y + 20.0F }, dot,
            key_color(asw::input::get_key(asw::input::Key::S)));
        asw::draw::circle_fill({ center.x - 20.0F, center.y }, dot,
            key_color(asw::input::get_key(asw::input::Key::A)));
        asw::draw::circle_fill({ center.x + 20.0F, center.y }, dot,
            key_color(asw::input::get_key(asw::input::Key::D)));

        asw::display::present();
    }

    return 0;
}
