/// @file main.cpp
/// @brief UI widgets example
///
/// Demonstrates:
///   - asw::ui::Root driving input, layout and drawing
///   - asw::ui::Panel, Stack, Label, Button, Choice, Slider, Checkbox and InputBox
///   - Button on_click, and Choice, Slider, Checkbox and InputBox on_change
///   - Theme font, so widgets do not each need one
///   - Keyboard and controller navigation through action bindings
///     (asw::ui::bind_default_navigation) and Root::on_back
///   - Editing asw::ui::Theme at runtime, and a per-button ButtonStyle
///   - asw::assets::get_save_path() to keep the name between runs
///   - asw::dialog::request_file() / take_file() for a native file chooser
///   - asw::dialog::confirm() and warn() message boxes
///     (desktop only, the web build has no native dialogs)
///
/// Controls:
///   Mouse - click buttons, click the box to type
///   Arrows, D-pad, left stick - move focus, or change a choice or slider
///     that is not in a row
///   Tab / Shift+Tab, LB / RB - move focus in order, from anywhere
///   Enter / Space / A - press the focused button, or edit a slider in a row
///   Escape / B - quit (asks first)
///
/// Scripted run:
///   ASW_EXAMPLE_AUTORUN=1 SDL_VIDEO_DRIVER=dummy ./example_ui
///   Fills in a name, clicks Greet, Theme, the slider and the checkbox, moves
///   focus down, left, right and down from the name box, saves autorun.png and
///   quits.

#include <array>
#include <asw/asw.h>
#include <cstdlib>
#include <fstream>
#include <string>
#include <utility>

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
    auto& theme = ui.ctx.theme;
    theme.panel_bg = p.panel;

    theme.button.bg = p.button;
    theme.button.bg_hover = p.hover;
    theme.button.bg_pressed = p.pressed;
    theme.button.border = p.pressed;
    theme.button.border_width = 1.0F;

    theme.checkbox.box = p.button;
    theme.checkbox.box_hover = p.hover;
    theme.checkbox.box_pressed = p.pressed;
    theme.checkbox.mark = p.accent;

    theme.slider.track = p.button;
    theme.slider.fill = p.accent;

    theme.input.border = p.button;
    theme.input.border_hover = p.hover;

    theme.focus_ring.color = p.accent;
    theme.focus_ring.width = 2.0F;

    ui.root.bg = p.panel.darken(0.4F);
}

asw::ui::Button& add_button(asw::ui::Widget& parent, const std::string& text)
{
    // No font set, buttons use the theme font
    auto& button = parent.add_child<asw::ui::Button>();
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

    // The root follows the screen size and is see through
    asw::ui::Root ui;
    ui.ctx.theme.font = font;

    // Centred card holding a vertical stack of widgets
    auto& card = ui.root.add_child<asw::ui::Panel>();
    card.transform = asw::Quad<float>((SCREEN_W - 400.0F) / 2.0F, 40.0F, 400.0F, 500.0F);

    auto& stack = card.add_child<asw::ui::Stack>();
    stack.transform = card.transform;
    stack.padding = 24.0F;
    stack.gap = 12.0F;

    auto& title = stack.add_child<asw::ui::Label>();
    title.font = title_font;
    title.text = "Settings";
    title.color = asw::color::white;
    title.transform.size.y = 48.0F;

    auto& prompt = stack.add_child<asw::ui::Label>();
    prompt.text = "Your name";
    prompt.color = asw::color::lightgray;
    prompt.transform.size.y = 20.0F;

    auto& name = stack.add_child<asw::ui::InputBox>();
    name.placeholder = "Type here...";
    name.value = load_name(save_file());
    name.transform.size.y = ROW_H;

    auto& greeting = stack.add_child<asw::ui::Label>();
    greeting.color = asw::color::gold;
    greeting.transform.size.y = 28.0F;

    // A row: a button and a choice side by side
    auto& row = stack.add_child<asw::ui::Stack>();
    row.direction = asw::ui::Direction::Horizontal;
    row.transform.size.y = ROW_H;

    auto& greet = add_button(row, "Greet");
    greet.transform.size.x = 150.0F;

    auto& theme = row.add_child<asw::ui::Choice>();
    theme.options = { "Theme: Dark", "Theme: Ocean", "Theme: Rose" };
    theme.transform.size.x = 194.0F;

#ifndef __EMSCRIPTEN__
    // Browsers have no native file chooser or message boxes, and quitting
    // would only freeze the page, so the web build leaves these out
    auto& load = add_button(stack, "Load name from file...");
#endif

    // Another row: a label and a slider
    auto& volume_row = stack.add_child<asw::ui::Stack>();
    volume_row.direction = asw::ui::Direction::Horizontal;
    volume_row.transform.size.y = 20.0F;

    auto& volume_label = volume_row.add_child<asw::ui::Label>();
    volume_label.text = "Volume";
    volume_label.transform.size.x = 100.0F;

    auto& volume = volume_row.add_child<asw::ui::Slider>();
    volume.value = asw::sound::get_master_volume();
    volume.transform.size.x = 244.0F;

    auto& remember = stack.add_child<asw::ui::Checkbox>();
    remember.text = "Remember my name";
    remember.checked = true;
    remember.transform.size.y = 24.0F;
#ifndef __EMSCRIPTEN__
    auto& quit = add_button(stack, "Quit");
#endif

    // Card background follows the theme
    apply_palette(ui, PALETTES[0]);
    card.bg = ui.ctx.theme.panel_bg;

    name.on_change = [&greeting](const std::string& value) {
        greeting.text = value.empty() ? "" : "Typing: " + value;
    };

    greet.on_click = [&name, &greeting, &remember]() {
        if (name.value.empty()) {
            greeting.text = "Please enter a name";
            return;
        }
        if (!remember.checked) {
            greeting.text = "Hello, " + name.value + "!";
            return;
        }
        save_name(name.value);
        greeting.text = "Hello, " + name.value + "! (saved)";
    };

#ifndef __EMSCRIPTEN__
    // The chooser does not block, the file arrives in the loop through take_file()
    load.on_click = []() {
        asw::dialog::request_file(asw::dialog::FileMode::Open, "",
            { { "Text files", "txt" }, { "All files", "*" } });
    };
#endif

    theme.on_change = [&ui, &card](std::size_t index) {
        apply_palette(ui, PALETTES[index]);
        card.bg = ui.ctx.theme.panel_bg;
    };

    volume.on_change = [](float value) { asw::sound::set_master_volume(value); };

    remember.on_change = [&greeting](bool checked) {
        greeting.text = checked ? "Greet will save your name" : "Greet will not save your name";
    };

    // Arrows, Tab, Return and Escape plus the same on any controller
    ui.ctx.navigation = asw::ui::bind_default_navigation();

#ifndef __EMSCRIPTEN__
    // A button with its own style, left aligned and red
    asw::ui::ButtonStyle danger;
    danger.bg = { 120, 40, 40 };
    danger.bg_hover = { 160, 60, 60 };
    danger.bg_pressed = { 190, 80, 80 };
    danger.border = { 230, 120, 120 };
    danger.border_width = 2.0F;
    danger.text_align = asw::TextJustify::Left;
    quit.style = danger;
    quit.padding = 12.0F;

    // Message boxes block until answered
    const auto ask_quit = []() {
        if (asw::dialog::confirm("Quit", "Close the UI example?")) {
            asw::core::exit();
        }
    };
    quit.on_click = ask_quit;

    // Back (Escape or B) when no widget uses it
    ui.on_back = ask_quit;
#endif

    if (!name.value.empty()) {
        greeting.text = "Welcome back, " + name.value;
    }

    int frame = 0;

    // Scripted navigation steps: frame and key
    const std::array<std::pair<int, asw::input::Key>, 4> NAV_STEPS { {
        { 36, asw::input::Key::Down },
        { 38, asw::input::Key::Left },
        { 40, asw::input::Key::Right },
        { 42, asw::input::Key::Down },
    } };
    std::string nav_path;
    const auto focus_name = [&]() -> std::string {
        const asw::ui::Widget* f = ui.ctx.focus.focused();
        if (f == &name) {
            return "name";
        }
        if (f == &greet) {
            return "greet";
        }
        if (f == &theme) {
            return "theme";
        }
        if (f == &volume) {
            return "volume";
        }
        if (f == &remember) {
            return "remember";
        }
#ifndef __EMSCRIPTEN__
        if (f == &load) {
            return "load";
        }
        if (f == &quit) {
            return "quit";
        }
#endif
        return "none";
    };

    asw::core::run([&]() {
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
            if (frame == 25) {
                // A quarter of the way along the slider
                asw::input::simulate_mouse_move({ volume.transform.position.x
                        + (volume.transform.size.x * 0.25F),
                    volume.transform.get_center().y });
                asw::input::simulate_mouse_button_down(asw::input::MouseButton::Left);
            }
            if (frame == 30) {
                click(remember);
            }
            if (frame == 11 || frame == 21 || frame == 26 || frame == 31) {
                asw::input::simulate_mouse_button_up(asw::input::MouseButton::Left);
            }

            // Navigation: from the name box down into the row, across it,
            // and down again. The choice leaves left and right to the row.
            if (frame == 35) {
                ui.focus(name, true);
            }
            for (const auto& [at, key] : NAV_STEPS) {
                if (frame == at) {
                    asw::input::simulate_key_down(key);
                }
                if (frame == at + 1) {
                    asw::input::simulate_key_up(key);
                }
            }
        }

        asw::core::update();

        ui.update();

        if (autorun && (frame == 35 || frame == 37 || frame == 39 || frame == 41 || frame == 43)) {
            nav_path += (nav_path.empty() ? "" : " > ") + focus_name();
        }

#ifndef __EMSCRIPTEN__
        if (const auto path = asw::dialog::take_file()) {
            const auto loaded = load_name(*path);
            if (loaded.empty()) {
                asw::dialog::warn("Load name", "The first line of " + *path + " is empty.");
            } else {
                name.value = loaded;
                greeting.text = "Loaded " + loaded;
            }
        }
#endif

        asw::display::clear();
        ui.draw();

        asw::draw::text(font, "Arrows or Tab (LB / RB) move, Enter (A) presses",
            { SCREEN_W / 2.0F, 540.0F },
            asw::color::gray, asw::TextJustify::Center);

        if (autorun && frame == 45) {
            const bool saved = asw::display::screenshot("autorun.png");
            asw::log::info(saved ? "Saved autorun.png" : "Screenshot failed");
            asw::log::info("Greeting: " + greeting.text);
            asw::log::info(std::string("Remember name: ") + (remember.checked ? "yes" : "no"));
            asw::log::info("Theme: " + theme.options[theme.index]);
            asw::log::info("Volume: " + std::to_string(volume.value));
            asw::log::info("Navigation: " + nav_path);
            asw::core::exit();
        }

        asw::display::present();
        frame++;
    });

    asw::core::shutdown();
    return 0;
}
