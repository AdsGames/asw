/// @file main.cpp
/// @brief UI widgets example
///
/// Demonstrates:
///   - asw::ui::Root driving input, layout and drawing
///   - asw::ui::Panel, VBox, Label, Button and InputBox
///   - Button on_click and InputBox on_change callbacks
///   - Keyboard navigation: Tab / arrows move focus, Enter activates
///   - Editing asw::ui::Theme at runtime
///   - asw::assets::get_save_path() to keep the name between runs
///   - asw::dialog::request_file() / take_file() for a native file chooser
///   - asw::dialog::confirm() and warn() message boxes
///
/// Controls:
///   Mouse - click buttons, click the box to type
///   Tab / Shift+Tab / Arrows - move focus
///   Enter / Space - press the focused button
///   Escape - quit (asks first)
///
/// Scripted run:
///   ASW_EXAMPLE_AUTORUN=1 SDL_VIDEO_DRIVER=dummy ./example_ui
///   Fills in a name, clicks Greet and Theme, saves autorun.png and quits.

#include <array>
#include <asw/asw.h>
#include <cstdlib>
#include <fstream>
#include <string>

namespace {
constexpr float SCREEN_W = 800.0F;
constexpr float SCREEN_H = 600.0F;
constexpr float ROW_H = 40.0F;

struct Palette {
    asw::Color panel;
    asw::Color button;
    asw::Color hover;
    asw::Color pressed;
    asw::Color accent;
};

const std::array<Palette, 3> PALETTES = { {
    { { 30, 30, 30 }, { 55, 55, 55 }, { 75, 75, 75 }, { 95, 95, 95 }, { 255, 200, 80 } },
    { { 20, 32, 56 }, { 40, 70, 120 }, { 60, 100, 160 }, { 90, 130, 190 }, { 120, 220, 255 } },
    { { 44, 24, 40 }, { 110, 50, 90 }, { 150, 70, 120 }, { 180, 100, 150 }, { 255, 150, 200 } },
} };

std::string save_file()
{
    return asw::assets::get_save_path("adsgames", "asw-ui-example") + "name.txt";
}

std::string load_name(const std::string& path)
{
    std::ifstream file(path);
    std::string name;
    std::getline(file, name);
    return name;
}

void save_name(const std::string& name)
{
    std::ofstream file(save_file());
    file << name;
}

void apply_palette(asw::ui::Root& ui, const Palette& p)
{
    ui.ctx.theme.panel_bg = p.panel;
    ui.ctx.theme.btn_bg = p.button;
    ui.ctx.theme.btn_hover = p.hover;
    ui.ctx.theme.btn_pressed = p.pressed;
    ui.ctx.theme.btn_focus_ring = p.accent;
    ui.root.bg = p.panel.darken(0.4F);
}

asw::ui::Button& add_button(asw::ui::Widget& parent, const asw::Font& font, const std::string& text)
{
    auto& button = parent.add_child<asw::ui::Button>();
    button.font = font;
    button.text = text;
    button.transform.size.y = ROW_H;
    return button;
}
} // namespace

int main()
{
    asw::core::init(static_cast<int>(SCREEN_W), static_cast<int>(SCREEN_H));
    asw::display::set_title("ASW Example - UI");

    const bool autorun = std::getenv("ASW_EXAMPLE_AUTORUN") != nullptr;

    const auto font = asw::assets::load_font("assets/font.ttf", 16.0F, asw::FontStyle::Pixel);
    const auto title_font = asw::assets::load_font("assets/font.ttf", 32.0F, asw::FontStyle::Pixel);

    asw::ui::Root ui;
    ui.set_size(SCREEN_W, SCREEN_H);

    // Centred card holding a vertical stack of widgets
    auto& card = ui.root.add_child<asw::ui::Panel>();
    card.transform = asw::Quad<float>((SCREEN_W - 400.0F) / 2.0F, 60.0F, 400.0F, 460.0F);

    auto& stack = card.add_child<asw::ui::VBox>();
    stack.transform = card.transform;
    stack.padding = 24.0F;
    stack.gap = 12.0F;

    auto& title = stack.add_child<asw::ui::Label>();
    title.font = title_font;
    title.text = "Settings";
    title.color = asw::color::white;
    title.transform.size.y = 48.0F;

    auto& prompt = stack.add_child<asw::ui::Label>();
    prompt.font = font;
    prompt.text = "Your name";
    prompt.color = asw::color::lightgray;
    prompt.transform.size.y = 20.0F;

    auto& name = stack.add_child<asw::ui::InputBox>();
    name.font = font;
    name.placeholder = "Type here...";
    name.value = load_name(save_file());
    name.transform.size.y = ROW_H;

    auto& greeting = stack.add_child<asw::ui::Label>();
    greeting.font = font;
    greeting.color = asw::color::gold;
    greeting.transform.size.y = 28.0F;

    auto& greet = add_button(stack, font, "Greet");
    auto& load = add_button(stack, font, "Load name from file...");
    auto& theme = add_button(stack, font, "Theme");
    auto& quit = add_button(stack, font, "Quit");

    // Card background follows the theme
    std::size_t palette = 0;
    apply_palette(ui, PALETTES[palette]);
    card.bg = ui.ctx.theme.panel_bg;

    name.on_change = [&greeting](const std::string& value) {
        greeting.text = value.empty() ? "" : "Typing: " + value;
    };

    greet.on_click = [&name, &greeting]() {
        if (name.value.empty()) {
            greeting.text = "Please enter a name";
            return;
        }
        save_name(name.value);
        greeting.text = "Hello, " + name.value + "! (saved)";
    };

    // The chooser does not block, the file arrives in the loop through take_file()
    load.on_click = []() {
        asw::dialog::request_file(asw::dialog::FileMode::Open, "",
            { { "Text files", "txt" }, { "All files", "*" } });
    };

    theme.on_click = [&ui, &card, &palette]() {
        palette = (palette + 1) % PALETTES.size();
        apply_palette(ui, PALETTES[palette]);
        card.bg = ui.ctx.theme.panel_bg;
    };

    // Message boxes block until answered
    const auto ask_quit = []() {
        if (asw::dialog::confirm("Quit", "Close the UI example?")) {
            asw::core::exit();
        }
    };
    quit.on_click = ask_quit;

    if (!name.value.empty()) {
        greeting.text = "Welcome back, " + name.value;
    }

    int frame = 0;

    while (!asw::core::is_exiting()) {
        // Scripted input: fill in a name, click Greet, then Theme
        if (autorun) {
            auto click = [](const asw::ui::Widget& w) {
                asw::input::simulate_mouse_move(w.transform.get_center());
                asw::input::simulate_mouse_button_down(asw::input::MouseButton::Left);
            };

            if (frame == 5) {
                name.value = "Ada";
            }
            if (frame == 10) {
                click(greet);
            }
            if (frame == 20) {
                click(theme);
            }
            if (frame == 11 || frame == 21) {
                asw::input::simulate_mouse_button_up(asw::input::MouseButton::Left);
            }
        }

        asw::core::update();

        if (asw::input::get_key_down(asw::input::Key::Escape)) {
            ask_quit();
        }

        ui.update();

        if (const auto path = asw::dialog::take_file()) {
            const auto loaded = load_name(*path);
            if (loaded.empty()) {
                asw::dialog::warn("Load name", "The first line of " + *path + " is empty.");
            } else {
                name.value = loaded;
                greeting.text = "Loaded " + loaded;
            }
        }

        asw::display::clear();
        ui.draw();

        asw::draw::text(font, "Tab to move focus, Enter to press", { SCREEN_W / 2.0F, 540.0F },
            asw::color::gray, asw::TextJustify::Center);

        if (autorun && frame == 40) {
            const bool saved = asw::display::screenshot("autorun.png");
            asw::log::info(saved ? "Saved autorun.png" : "Screenshot failed");
            asw::log::info("Greeting: " + greeting.text);
            asw::core::exit();
        }

        asw::display::present();
        frame++;
    }

    asw::core::shutdown();
    return 0;
}
