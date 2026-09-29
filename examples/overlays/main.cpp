/// @file main.cpp
/// @brief UI overlays example
///
/// Demonstrates:
///   - Widget::anchor to pin buttons to corners of the screen
///   - Root::toast for short messages at the top of the screen
///   - Root::open_modal with add_text, add_button and on_close
///   - asw::util::open_url to open a web page
///   - asw::assets::write_save() and read_save() to keep a count between runs
///
/// Controls:
///   Mouse - click buttons
///   Arrows, D-pad - move focus, Enter (A) presses
///   Escape / B - close the modal
///
/// Scripted run:
///   ASW_EXAMPLE_AUTORUN=1 SDL_VIDEO_DRIVER=dummy ./example_overlays
///   Shows toasts, opens and closes the modal with back, clicks outside an
///   open modal, saves and loads, saves autorun.png and quits.

#include <asw/asw.h>
#include <cstdlib>
#include <string>

namespace {
constexpr float SCREEN_W = 800.0F;
constexpr float SCREEN_H = 600.0F;

constexpr const char* ORG = "adsgames";
constexpr const char* APP = "asw-examples";
} // namespace

int main()
{
    asw::core::init(static_cast<int>(SCREEN_W), static_cast<int>(SCREEN_H));
    asw::display::set_title("ASW Example - Overlays");

    const bool autorun = std::getenv("ASW_EXAMPLE_AUTORUN") != nullptr;

    const auto font = asw::assets::load_font("assets/font.ttf", 16.0F, asw::FontStyle::Pixel);
    const auto big_font = asw::assets::load_font("assets/font.ttf", 32.0F, asw::FontStyle::Pixel);

    asw::ui::Root ui;
    ui.ctx.theme.font = font;
    ui.ctx.theme.modal.border = { 255, 200, 80 };
    ui.ctx.theme.modal.border_width = 2.0F;
    ui.ctx.theme.toast.anchor = asw::ui::Anchor::TopRight;
    ui.ctx.navigation = asw::ui::bind_default_navigation();

    // Opened at start, kept between runs
    int opens = std::atoi(asw::assets::read_save(ORG, APP, "opens.txt").c_str()) + 1;
    asw::assets::write_save(ORG, APP, "opens.txt", std::to_string(opens));

    // Pinned to corners, they stay there whatever their size
    auto& toast_button = ui.root.add_child<asw::ui::Button>();
    toast_button.padding = 10.0F;
    toast_button.set_text("Show toast", true);
    toast_button.anchor = asw::ui::Anchor::TopLeft;
    toast_button.anchor_margin = { 20.0F, 20.0F };

    auto& account = ui.root.add_child<asw::ui::Button>();
    account.padding = 10.0F;
    account.set_text("Link account", true);
    account.anchor = asw::ui::Anchor::BottomRight;
    account.anchor_margin = { 20.0F, 20.0F };

    auto& runs = ui.root.add_child<asw::ui::Label>();
    runs.set_text("Opened " + std::to_string(opens) + " times", true);
    runs.anchor = asw::ui::Anchor::BottomLeft;
    runs.anchor_margin = { 20.0F, 26.0F };

    int toasts = 0;
    toast_button.on_click = [&]() {
        toasts += 1;
        ui.toast("Achievement unlocked: Toast " + std::to_string(toasts));
    };

    // An account link, like a game asking the player to log in on a website
    std::string result = "none";
    asw::ui::Modal* modal = nullptr;
    const auto open_link = [&]() {
        auto& m = ui.open_modal();
        modal = &m;
        m.add_text("Link your account");
        m.add_text("Go to example.com/link and enter");
        m.add_text("BCDF-GHJK", big_font).color = asw::Color { 255, 200, 80 };
        m.add_button("Open the page", [&]() {
            if (!autorun) {
                asw::util::open_url("https://example.com/link?code=BCDF-GHJK");
            }
            result = "opened";
            m.close();
        });
        m.add_button("Cancel", [&m]() { m.close(); });
        m.on_close = [&]() {
            if (result == "none") {
                result = "cancelled";
            }
            ui.toast(result == "opened" ? "Waiting for the website" : "Link cancelled");
            modal = nullptr;
        };
    };
    account.on_click = open_link;

    int frame = 0;
    std::string log;
    const auto note = [&log](const std::string& line) { log += (log.empty() ? "" : ", ") + line; };

    asw::core::run([&]() {
        if (autorun) {
            auto click = [](const asw::ui::Widget& w) {
                asw::input::simulate_mouse_move(w.transform.get_center());
                asw::input::simulate_mouse_button_down(asw::input::MouseButton::Left);
            };
            auto release
                = []() { asw::input::simulate_mouse_button_up(asw::input::MouseButton::Left); };

            // Four toasts, three show at once
            for (const int at : { 5, 7, 9, 11 }) {
                if (frame == at) {
                    click(toast_button);
                }
                if (frame == at + 1) {
                    release();
                }
            }

            // Open the modal, back closes it
            if (frame == 15) {
                click(account);
            }
            if (frame == 16) {
                release();
            }
            if (frame == 20) {
                asw::input::simulate_key_down(asw::input::Key::Escape);
            }
            if (frame == 21) {
                asw::input::simulate_key_up(asw::input::Key::Escape);
            }

            // Open it again, a click outside does nothing
            if (frame == 25) {
                click(account);
            }
            if (frame == 26) {
                release();
            }
            if (frame == 30) {
                click(toast_button);
            }
            if (frame == 31) {
                release();
            }
        }

        asw::core::update();
        ui.update();

        if (autorun) {
            if (frame == 13) {
                note("toasts " + std::to_string(ui.toast_count()));
            }
            if (frame == 18) {
                const auto* f = ui.ctx.focus.focused();
                note(std::string("modal open ") + (ui.has_modal() ? "yes" : "no"));
                note(std::string("modal focus ")
                    + (modal != nullptr && f != nullptr && f->parent == modal ? "first button"
                                                                              : "none"));
            }
            if (frame == 23) {
                note(std::string("after back ") + (ui.has_modal() ? "open" : "closed") + " ("
                    + result + ")");
                note(std::string("focus back on account ")
                    + (ui.ctx.focus.focused() == &account ? "yes" : "no"));
            }
            if (frame == 33) {
                note("outside click ignored " + std::string(toasts == 4 ? "yes" : "no"));
            }
        }

        asw::display::clear();

        // The game draws first, the UI and its modals go on top
        asw::draw::text(font, "Anchored buttons, toasts and a modal",
            { SCREEN_W / 2.0F, SCREEN_H / 2.0F }, asw::color::gray, asw::TextJustify::Center);

        ui.draw();

        if (autorun && frame == 35) {
            const bool saved = asw::display::screenshot("autorun.png");
            asw::log::info(saved ? "Saved autorun.png" : "Screenshot failed");
            note("save " + asw::assets::read_save(ORG, APP, "opens.txt"));
            asw::log::info("Overlays: " + log);
            asw::core::exit();
        }

        asw::display::present();
        frame++;
    });

    asw::core::shutdown();
    return 0;
}
