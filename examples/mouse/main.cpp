/// @file main.cpp
/// @brief Mouse input example
///
/// Demonstrates:
///   - Reading mouse position and delta via asw::input::mouse
///   - Polling buttons with get_mouse_button_down() / get_mouse_button_up() / get_mouse_button()
///   - Scroll wheel via mouse.z
///   - Changing cursor style with set_cursor()
///
/// Controls:
///   Move mouse         - move the cursor circle
///   Left click         - change circle color to red
///   Right click        - change circle color to blue
///   Scroll wheel       - resize the circle
///   Escape             - quit

#include <algorithm>
#include <asw/asw.h>

int main()
{
    asw::core::init(800, 600);
    asw::display::set_title("ASW Example - Mouse");
    asw::core::print_info();

    asw::Color circle_color = asw::color::white;
    float radius = 30.0F;

    while (!asw::core::is_exiting()) {
        asw::core::update();

        // --- Button events ---
        if (asw::input::get_mouse_button_down(asw::input::MouseButton::Left)) {
            circle_color = asw::color::red;
            asw::log::info("Left button pressed at ({:.0f}, {:.0f})", asw::input::mouse.position.x,
                asw::input::mouse.position.y);
        }
        if (asw::input::get_mouse_button_down(asw::input::MouseButton::Right)) {
            circle_color = asw::color::blue;
            asw::log::info("Right button pressed at ({:.0f}, {:.0f})", asw::input::mouse.position.x,
                asw::input::mouse.position.y);
        }
        if (asw::input::get_mouse_button_up(asw::input::MouseButton::Left)
            || asw::input::get_mouse_button_up(asw::input::MouseButton::Right)) {
            circle_color = asw::color::white;
        }

        // --- Scroll wheel ---
        if (asw::input::mouse.z != 0.0F) {
            radius = std::clamp(radius + asw::input::mouse.z * 5.0F, 5.0F, 150.0F);
            asw::log::info("Scroll: {:.1f}  radius: {:.0f}", asw::input::mouse.z, radius);
        }

        // --- Cursor style based on button held ---
        if (asw::input::get_mouse_button(asw::input::MouseButton::Left)) {
            asw::input::set_cursor(asw::input::CursorId::Crosshair);
        } else if (asw::input::get_mouse_button(asw::input::MouseButton::Right)) {
            asw::input::set_cursor(asw::input::CursorId::Move);
        } else {
            asw::input::set_cursor(asw::input::CursorId::Default);
        }

        if (asw::input::get_key_down(asw::input::Key::Escape)) {
            asw::core::exit();
            break;
        }

        // --- Draw ---
        asw::display::clear(asw::color::darkslategray);

        const auto& mp = asw::input::mouse.position;

        // Crosshair lines
        const auto win = asw::display::get_logical_size();
        asw::draw::line({ 0.0F, mp.y }, { static_cast<float>(win.x), mp.y }, asw::color::dimgray);
        asw::draw::line({ mp.x, 0.0F }, { mp.x, static_cast<float>(win.y) }, asw::color::dimgray);

        // Main cursor circle
        asw::draw::circle_fill(mp, radius, circle_color.with_alpha(180));
        asw::draw::circle(mp, radius, asw::color::white);

        // Delta indicator (shows mouse movement direction)
        const auto& delta = asw::input::mouse.change;
        if (delta.x != 0.0F || delta.y != 0.0F) {
            asw::draw::line(
                mp, { mp.x + delta.x * 10.0F, mp.y + delta.y * 10.0F }, asw::color::yellow);
        }

        // Button state indicators
        constexpr float btn_size = 30.0F;
        const asw::Vec2<float> btn_base { 20.0F, 20.0F };
        asw::draw::rect_fill({ btn_base, { btn_size, btn_size } },
            asw::input::get_mouse_button(asw::input::MouseButton::Left) ? asw::color::red
                                                                        : asw::color::darkred);
        asw::draw::rect_fill(
            { { btn_base.x + btn_size + 5.0F, btn_base.y }, { btn_size, btn_size } },
            asw::input::get_mouse_button(asw::input::MouseButton::Middle) ? asw::color::lime
                                                                          : asw::color::darkgreen);
        asw::draw::rect_fill(
            { { btn_base.x + (btn_size + 5.0F) * 2.0F, btn_base.y }, { btn_size, btn_size } },
            asw::input::get_mouse_button(asw::input::MouseButton::Right) ? asw::color::blue
                                                                         : asw::color::darkblue);

        asw::display::present();
    }

    return 0;
}
