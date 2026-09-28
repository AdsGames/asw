/// @file main.cpp
/// @brief Keyboard input example
///
/// Demonstrates:
///   - get_key() (held), get_key_down() (pressed this frame) and get_key_up()
///     (released this frame)
///   - Frame rate independent movement with asw::core::get_delta_time()
///   - get_keyboard() for the last key pressed
///   - get_text_input() for typed characters (after SDL_StartTextInput)
///
/// Controls:
///   WASD / Arrow keys - move the box
///   Space             - watch the pressed / released flashes
///   Type              - text shows at the bottom, Backspace deletes
///   Escape            - quit
///
/// Scripted run:
///   ASW_EXAMPLE_AUTORUN=1 SDL_VIDEO_DRIVER=dummy ./example_keyboard

#include <algorithm>
#include <asw/asw.h>
#include <cstdlib>
#include <string>

namespace {
constexpr float SPEED = 240.0F; // pixels per second
constexpr float BOX_SIZE = 50.0F;
constexpr float FLASH_TIME = 0.4F;

const asw::Color KEY_UP_COLOR(60, 66, 80);
const asw::Color KEY_DOWN_COLOR = asw::color::lime;

// Draw a key cap that lights up while held
void draw_key(const asw::Font& font, const asw::Vec2<float>& pos, const std::string& label,
    asw::input::Key key)
{
    const bool held = asw::input::get_key(key);
    const asw::Quad<float> cap(pos, asw::Vec2<float>(36.0F, 36.0F));
    asw::draw::rect_fill(cap, held ? KEY_DOWN_COLOR : KEY_UP_COLOR);
    asw::draw::rect(cap, asw::color::gray);
    asw::draw::text(font, label, cap.get_center() - asw::Vec2<float>(0.0F, 4.0F),
        held ? asw::color::black : asw::color::white, asw::TextJustify::Center);
}
} // namespace

int main()
{
    asw::core::init(800, 600);
    asw::display::set_title("ASW Example - Keyboard");

    const bool autorun = std::getenv("ASW_EXAMPLE_AUTORUN") != nullptr;
    const auto font = asw::assets::load_font("assets/font.ttf", 8.0F, asw::FontStyle::Pixel);

    // Text input events are only sent while text input is on
    SDL_StartTextInput(asw::display::get_window());

    asw::Vec2<float> pos { 375.0F, 200.0F };
    std::string typed;
    std::string last_key = "-";
    float pressed_flash = 0.0F;
    float released_flash = 0.0F;
    int frame = 0;

    asw::core::run([&]() {
        // Scripted input: move right, tap space
        if (autorun) {
            if (frame == 2) {
                asw::input::simulate_key_down(asw::input::Key::D);
            }
            if (frame == 30) {
                asw::input::simulate_key_up(asw::input::Key::D);
                asw::input::simulate_key_down(asw::input::Key::Space);
            }
            if (frame == 36) {
                asw::input::simulate_key_up(asw::input::Key::Space);
            }
        }

        asw::core::update();
        const float dt = autorun ? 1.0F / 60.0F : asw::core::get_delta_time();

        if (asw::input::get_key_down(asw::input::Key::Escape)) {
            asw::core::exit();
        }

        // --- Held keys: movement ---
        asw::Vec2<float> move(0.0F, 0.0F);
        if (asw::input::get_key(asw::input::Key::W) || asw::input::get_key(asw::input::Key::Up)) {
            move.y -= 1.0F;
        }
        if (asw::input::get_key(asw::input::Key::S)
            || asw::input::get_key(asw::input::Key::Down)) {
            move.y += 1.0F;
        }
        if (asw::input::get_key(asw::input::Key::A)
            || asw::input::get_key(asw::input::Key::Left)) {
            move.x -= 1.0F;
        }
        if (asw::input::get_key(asw::input::Key::D)
            || asw::input::get_key(asw::input::Key::Right)) {
            move.x += 1.0F;
        }
        pos += move.normalized() * (SPEED * dt);

        const auto win = asw::display::get_logical_size();
        pos.x = std::clamp(pos.x, 0.0F, static_cast<float>(win.x) - BOX_SIZE);
        pos.y = std::clamp(pos.y, 0.0F, static_cast<float>(win.y) - BOX_SIZE);

        // --- Single frame events ---
        if (asw::input::get_key_down(asw::input::Key::Space)) {
            pressed_flash = FLASH_TIME;
        }
        if (asw::input::get_key_up(asw::input::Key::Space)) {
            released_flash = FLASH_TIME;
        }
        pressed_flash = std::max(0.0F, pressed_flash - dt);
        released_flash = std::max(0.0F, released_flash - dt);

        // last_pressed only holds a key on the frame it goes down, so keep it
        const auto& keyboard = asw::input::get_keyboard();
        if (keyboard.last_pressed >= 0) {
            last_key = SDL_GetScancodeName(static_cast<SDL_Scancode>(keyboard.last_pressed));
        }

        // --- Text input ---
        typed += asw::input::get_text_input();
        if (asw::input::get_key_down(asw::input::Key::Backspace) && !typed.empty()) {
            typed.pop_back();
        }
        if (typed.size() > 60) {
            typed.erase(0, typed.size() - 60);
        }

        // --- Draw ---
        asw::display::clear(asw::Color(24, 28, 36));

        const asw::Quad<float> box(pos, asw::Vec2<float>(BOX_SIZE, BOX_SIZE));
        asw::draw::rect_fill(box, keyboard.any_pressed ? asw::color::cyan : asw::color::white);
        asw::draw::rect(box, asw::color::gray);

        // WASD caps
        const asw::Vec2<float> caps(560.0F, 380.0F);
        draw_key(font, caps + asw::Vec2<float>(40.0F, 0.0F), "W", asw::input::Key::W);
        draw_key(font, caps + asw::Vec2<float>(0.0F, 40.0F), "A", asw::input::Key::A);
        draw_key(font, caps + asw::Vec2<float>(40.0F, 40.0F), "S", asw::input::Key::S);
        draw_key(font, caps + asw::Vec2<float>(80.0F, 40.0F), "D", asw::input::Key::D);

        // Space bar with pressed / released flashes
        const asw::Quad<float> space(60.0F, 420.0F, 300.0F, 36.0F);
        const bool space_held = asw::input::get_key(asw::input::Key::Space);
        asw::draw::rect_fill(space, space_held ? KEY_DOWN_COLOR : KEY_UP_COLOR);
        asw::draw::rect(space, asw::color::gray);
        asw::draw::text(font, "get_key(Space)", space.get_center() - asw::Vec2<float>(0.0F, 4.0F),
            space_held ? asw::color::black : asw::color::white, asw::TextJustify::Center);

        const auto flash_color = [](float t) {
            return asw::Color(255, 220, 80, static_cast<uint8_t>(255.0F * (t / FLASH_TIME)));
        };
        asw::draw::text(font, "get_key_down", asw::Vec2<float>(60.0F, 400.0F),
            pressed_flash > 0.0F ? flash_color(pressed_flash) : asw::Color(90, 96, 110));
        asw::draw::text(font, "get_key_up", asw::Vec2<float>(360.0F, 400.0F),
            released_flash > 0.0F ? flash_color(released_flash) : asw::Color(90, 96, 110),
            asw::TextJustify::Right);

        // Info
        asw::draw::text_shadow(font, "Move: WASD / arrows  Quit: Escape",
            asw::Vec2<float>(10.0F, 10.0F), asw::color::white);
        asw::draw::text_shadow(
            font, "Last key: " + last_key, asw::Vec2<float>(10.0F, 24.0F), asw::color::white);

        asw::draw::text_shadow(font, "Type something:", asw::Vec2<float>(10.0F, 520.0F),
            asw::color::lightgray);
        asw::draw::rect(asw::Quad<float>(10.0F, 536.0F, 780.0F, 24.0F), asw::color::gray);
        asw::draw::text(font, typed + "_", asw::Vec2<float>(16.0F, 544.0F), asw::color::white);

        if (autorun && frame == 34) {
            const bool saved = asw::display::screenshot("autorun.png");
            asw::log::info(saved ? "Saved autorun.png" : "Screenshot failed");
            asw::log::info("Box x: {:.0f}", pos.x);
            asw::core::exit();
        }

        asw::display::present();
        frame++;
    });

    SDL_StopTextInput(asw::display::get_window());
    asw::core::shutdown();
    return 0;
}
