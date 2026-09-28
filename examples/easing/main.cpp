/// @file main.cpp
/// @brief Easing functions example
///
/// Demonstrates:
///   - Every asw::easing function, plotted as a curve with a dot riding it
///   - asw::easing::ease() to move a marker between two values
///   - asw::util::lerp() to blend colours by the eased value
///
/// Each cell is one easing function. Rows are families: linear and quad,
/// cubic, sine, expo, then elastic, bounce and back. The bar under each
/// curve moves by the eased value, so overshoot and bounce are easy to see.
///
/// Controls:
///   Space - pause
///   Up / Down - faster / slower
///   Escape - quit
///
/// Scripted run:
///   ASW_EXAMPLE_AUTORUN=1 SDL_VIDEO_DRIVER=dummy ./example_easing
///   Runs part way through a cycle, saves autorun.png and quits.

#include <algorithm>
#include <asw/asw.h>
#include <cmath>
#include <cstdlib>
#include <vector>

namespace {
using EaseFn = float (*)(float);

struct Curve {
    EaseFn fn;
    asw::Color color;
};

constexpr float CELL_W = 200.0F;
constexpr float CELL_H = 120.0F;
constexpr float PLOT_W = 150.0F;
constexpr float PLOT_H = 60.0F;
constexpr int COLUMNS = 4;

const std::vector<Curve>& curves()
{
    namespace e = asw::easing;
    static const std::vector<Curve> list = {
        { e::linear, asw::color::white },
        { e::ease_in_quad, asw::color::tomato },
        { e::ease_out_quad, asw::color::tomato },
        { e::ease_in_out_quad, asw::color::tomato },

        { e::ease_in_cubic, asw::color::orange },
        { e::ease_out_cubic, asw::color::orange },
        { e::ease_in_out_cubic, asw::color::orange },
        { e::ease_in_sine, asw::color::gold },

        { e::ease_out_sine, asw::color::gold },
        { e::ease_in_out_sine, asw::color::gold },
        { e::ease_in_expo, asw::color::mediumseagreen },
        { e::ease_out_expo, asw::color::mediumseagreen },

        { e::ease_in_out_expo, asw::color::mediumseagreen },
        { e::ease_in_elastic, asw::color::deepskyblue },
        { e::ease_out_elastic, asw::color::deepskyblue },
        { e::ease_in_bounce, asw::color::mediumpurple },

        { e::ease_out_bounce, asw::color::mediumpurple },
        { e::ease_in_back, asw::color::hotpink },
        { e::ease_out_back, asw::color::hotpink },
    };
    return list;
}

asw::Color mix(const asw::Color& a, const asw::Color& b, float t)
{
    auto channel = [t](uint8_t from, uint8_t to) {
        return static_cast<uint8_t>(std::clamp(
            asw::util::lerp(static_cast<float>(from), static_cast<float>(to), t), 0.0F, 255.0F));
    };
    return { channel(a.r, b.r), channel(a.g, b.g), channel(a.b, b.b), 255 };
}

void draw_cell(const Curve& curve, const asw::Vec2<float>& origin, float t)
{
    const asw::Quad<float> plot(
        origin.x + (CELL_W - PLOT_W) / 2.0F, origin.y + 20.0F, PLOT_W, PLOT_H);

    // Plot box, 0 at the bottom and 1 at the top
    asw::draw::rect_fill(plot, asw::Color(30, 34, 48));
    asw::draw::rect(plot, asw::Color(70, 76, 96));

    auto to_screen = [&plot](float x, float y) {
        return asw::Vec2<float>(
            plot.position.x + x * plot.size.x, plot.position.y + (1.0F - y) * plot.size.y);
    };

    // Curve, drawn as short segments. Elastic and back leave the box on purpose.
    constexpr int STEPS = 60;
    auto prev = to_screen(0.0F, curve.fn(0.0F));
    for (int i = 1; i <= STEPS; ++i) {
        const float x = static_cast<float>(i) / STEPS;
        const auto next = to_screen(x, curve.fn(x));
        asw::draw::line(prev, next, curve.color.darken(0.2F));
        prev = next;
    }

    // Dot riding the curve
    const float value = curve.fn(t);
    asw::draw::circle_fill(to_screen(t, value), 4.0F, asw::color::white);

    // Marker eased along a track under the plot
    const float track_y = plot.position.y + plot.size.y + 18.0F;
    const float left = plot.position.x;
    const float right = plot.position.x + plot.size.x;
    asw::draw::line({ left, track_y }, { right, track_y }, asw::Color(70, 76, 96));

    const float x = asw::easing::ease(left, right, t, curve.fn);
    asw::draw::rect_fill({ x - 6.0F, track_y - 6.0F, 12.0F, 12.0F },
        mix(curve.color.darken(0.6F), curve.color.lighten(0.3F), value));
}
} // namespace

int main()
{
    asw::core::init(800, 600);
    asw::display::set_title("ASW Example - Easing");

    const bool autorun = std::getenv("ASW_EXAMPLE_AUTORUN") != nullptr;

    constexpr float HOLD = 0.5F;
    float duration = 1.5F;
    float time = 0.0F;
    bool paused = false;
    int frame = 0;

    while (!asw::core::is_exiting()) {
        asw::core::update();

        // Real frame time, but a fixed step for scripted runs so they repeat
        const float dt = autorun ? 1.0F / 60.0F : asw::core::get_delta_time();

        if (asw::input::get_key_down(asw::input::Key::Escape)) {
            asw::core::exit();
        }
        if (asw::input::get_key_down(asw::input::Key::Space)) {
            paused = !paused;
        }
        if (asw::input::get_key_down(asw::input::Key::Up)) {
            duration = std::max(0.25F, duration - 0.25F);
        }
        if (asw::input::get_key_down(asw::input::Key::Down)) {
            duration = std::min(5.0F, duration + 0.25F);
        }

        if (!paused) {
            time += dt;
        }

        // Play forward, hold, play backward, hold
        const float cycle = (duration + HOLD) * 2.0F;
        const float phase = std::fmod(time, cycle);
        float t = 0.0F;
        if (phase < duration) {
            t = phase / duration;
        } else if (phase < duration + HOLD) {
            t = 1.0F;
        } else if (phase < duration * 2.0F + HOLD) {
            t = 1.0F - (phase - duration - HOLD) / duration;
        }

        asw::display::clear(asw::Color(18, 20, 30));

        const auto& list = curves();
        for (std::size_t i = 0; i < list.size(); ++i) {
            const auto col = static_cast<float>(i % COLUMNS);
            const auto row = static_cast<float>(i / COLUMNS);
            draw_cell(list[i], { col * CELL_W, row * CELL_H }, t);
        }

        // Progress bar in the empty last cell
        const asw::Quad<float> bar(
            3.0F * CELL_W + 25.0F, 4.0F * CELL_H + 50.0F, (CELL_W - 50.0F) * t, 16.0F);
        asw::draw::rect_fill(bar, paused ? asw::color::gray : asw::color::white);
        asw::draw::rect({ 3.0F * CELL_W + 25.0F, 4.0F * CELL_H + 50.0F, CELL_W - 50.0F, 16.0F },
            asw::color::white);

        if (autorun && frame == 50) {
            const bool saved = asw::display::screenshot("autorun.png");
            asw::log::info(saved ? "Saved autorun.png" : "Screenshot failed");
            asw::core::exit();
        }

        asw::display::present();
        frame++;
    }

    asw::core::shutdown();
    return 0;
}
