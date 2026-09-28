/// @file main.cpp
/// @brief Scene manager and game object example
///
/// Demonstrates:
///   - asw::scene::SceneManager with two scenes and scene switching
///   - asw::scene::Scene::create_object() and get_object_view()
///   - asw::game::GameObject with a Physics body, z_index, alpha and alive
///   - asw::ParticleEmitter used as a scene object
///   - asw::easing to animate the title screen
///   - asw::random for spawn positions, colours and velocities
///
/// Controls:
///   Title: Enter / Left click - start
///   Game: Left click - pop a ball, or spawn one on empty space
///         Space - spawn ten balls
///         Escape - back to title
///   Escape on the title - quit
///
/// Scripted run:
///   ASW_EXAMPLE_AUTORUN=1 SDL_VIDEO_DRIVER=dummy ./example_scenes
///   Starts the game, spawns balls, pops one, saves autorun.png and quits.

#include <algorithm>
#include <asw/asw.h>
#include <cmath>
#include <cstdlib>
#include <vector>

namespace {
constexpr float SCREEN_W = 800.0F;
constexpr float SCREEN_H = 600.0F;
constexpr float GRAVITY = 900.0F;
constexpr float BALL_LIFETIME = 8.0F;

enum class SceneId { Title, Game };

const std::vector<asw::Color> PALETTE = { asw::color::tomato, asw::color::gold,
    asw::color::mediumseagreen, asw::color::deepskyblue, asw::color::orchid };

bool autorun()
{
    static const bool enabled = std::getenv("ASW_EXAMPLE_AUTORUN") != nullptr;
    return enabled;
}

/// A bouncing ball. Uses the GameObject physics body for movement and fades
/// out before it removes itself by clearing alive.
class Ball : public asw::game::GameObject {
public:
    Ball(const asw::Vec2<float>& center, float radius)
        : radius(radius)
        , color(PALETTE[static_cast<std::size_t>(
              asw::random::random(static_cast<int>(PALETTE.size()) - 1))])
    {
        transform = asw::Quad<float>(center.x - radius, center.y - radius, radius * 2, radius * 2);
        body.velocity = asw::Vec2<float>(
            asw::random::between(-250.0F, 250.0F), asw::random::between(-500.0F, -150.0F));
        body.acceleration = asw::Vec2<float>(0.0F, GRAVITY);

        // Bigger balls sit behind smaller ones
        z_index = -static_cast<int>(radius);
    }

    void update(float dt) override
    {
        // Integrates velocity and position
        asw::game::GameObject::update(dt);

        auto& pos = transform.position;
        const float size = radius * 2.0F;

        if (pos.x < 0.0F || pos.x + size > SCREEN_W) {
            pos.x = std::clamp(pos.x, 0.0F, SCREEN_W - size);
            body.velocity.x = -body.velocity.x;
        }

        if (pos.y + size > SCREEN_H) {
            pos.y = SCREEN_H - size;
            body.velocity.y = -body.velocity.y * 0.8F;
        }

        age += dt;
        alpha = std::clamp((BALL_LIFETIME - age) / 1.0F, 0.0F, 1.0F);

        if (age >= BALL_LIFETIME) {
            alive = false;
        }
    }

    void draw() override
    {
        const auto a = static_cast<uint8_t>(255.0F * alpha);
        asw::draw::circle_fill(transform.get_center(), radius, color.with_alpha(a));
        asw::draw::circle(transform.get_center(), radius, color.lighten(0.5F).with_alpha(a));
    }

    bool hit(const asw::Vec2<float>& point) const
    {
        return transform.get_center().distance(point) <= radius;
    }

    const asw::Color& get_color() const
    {
        return color;
    }

private:
    float radius;
    asw::Color color;
    float age { 0.0F };
};

/// A one shot particle burst that removes itself once every particle is gone.
class Burst : public asw::ParticleEmitter {
public:
    Burst(const asw::Vec2<float>& position, const asw::ParticleConfig& config)
        : asw::ParticleEmitter(config, 64)
    {
        transform.position = position;
        z_index = 100;
        emit(40);
    }

    void update(float dt) override
    {
        asw::ParticleEmitter::update(dt);

        if (get_alive_count() == 0) {
            alive = false;
        }
    }
};

class TitleScene : public asw::scene::Scene<SceneId> {
public:
    using asw::scene::Scene<SceneId>::Scene;

    void init() override
    {
        time = 0.0F;
        frame = 0;
    }

    void update(float dt) override
    {
        Scene::update(dt);
        time += dt;
        frame++;

        if (autorun() && frame == 30) {
            asw::input::simulate_key_down(asw::input::Key::Return);
        }

        if (autorun() && frame == 31) {
            asw::input::simulate_key_up(asw::input::Key::Return);
        }

        if (asw::input::get_key_down(asw::input::Key::Return)
            || asw::input::get_mouse_button_down(asw::input::MouseButton::Left)) {
            manager.set_next_scene(SceneId::Game);
        }

        if (asw::input::get_key_down(asw::input::Key::Escape)) {
            asw::core::exit();
        }
    }

    void draw() override
    {
        asw::display::clear(asw::color::midnightblue);

        // Drop the logo in with a bounce, then let it bob
        const float drop
            = asw::easing::ease(-200.0F, 220.0F, time / 1.2F, asw::easing::ease_out_bounce);
        const float bob = time > 1.2F ? std::sin((time - 1.2F) * 2.0F) * 8.0F : 0.0F;

        const float w = 360.0F;
        const float h = 120.0F;
        const asw::Quad<float> logo((SCREEN_W - w) / 2.0F, drop + bob, w, h);
        asw::draw::rect_fill(logo, asw::color::royalblue);
        asw::draw::rect(logo, asw::color::white);

        for (std::size_t i = 0; i < PALETTE.size(); ++i) {
            const float x = logo.position.x + 60.0F + static_cast<float>(i) * 60.0F;
            const float y = logo.get_center().y
                + std::sin(time * 4.0F + static_cast<float>(i)) * 12.0F;
            asw::draw::circle_fill({ x, y }, 20.0F, PALETTE[i]);
        }

        // "Press start" prompt blinks once the logo has landed
        if (time > 1.2F && std::fmod(time, 1.0F) < 0.6F) {
            asw::draw::rect_fill({ SCREEN_W / 2.0F - 80.0F, 440.0F, 160.0F, 12.0F },
                asw::color::white.with_alpha(200));
        }
    }

private:
    float time { 0.0F };
    int frame { 0 };
};

class GameScene : public asw::scene::Scene<SceneId> {
public:
    using asw::scene::Scene<SceneId>::Scene;

    void init() override
    {
        frame = 0;
        popped = 0;

        sparks.lifetime_min = 0.3F;
        sparks.lifetime_max = 0.8F;
        sparks.speed_min = 60.0F;
        sparks.speed_max = 260.0F;
        sparks.gravity = asw::Vec2<float>(0.0F, 400.0F);
        sparks.size_start = 6.0F;
        sparks.size_end = 1.0F;

        spawn(asw::Vec2<float>(SCREEN_W / 2.0F, SCREEN_H / 3.0F), 10);
    }

    void update(float dt) override
    {
        frame++;
        run_script();

        if (asw::input::get_key_down(asw::input::Key::Escape)) {
            manager.set_next_scene(SceneId::Title);
        }

        if (asw::input::get_key_down(asw::input::Key::Space)) {
            spawn(asw::Vec2<float>(asw::random::between(100.0F, SCREEN_W - 100.0F), 100.0F), 10);
        }

        if (asw::input::get_mouse_button_down(asw::input::MouseButton::Left)) {
            click(asw::input::get_mouse().position);
        }

        // Keep the screen busy
        if (frame % 60 == 0) {
            spawn(asw::Vec2<float>(asw::random::between(100.0F, SCREEN_W - 100.0F), 80.0F), 1);
        }

        // Updates every object, removes dead ones and adds new ones
        Scene::update(dt);
    }

    void draw() override
    {
        asw::display::clear(asw::color::darkslategray);

        // Floor
        asw::draw::line(
            { 0.0F, SCREEN_H - 1.0F }, { SCREEN_W, SCREEN_H - 1.0F }, asw::color::white);

        // Draws every object sorted by z_index
        Scene::draw();

        // One pip per popped ball
        for (int i = 0; i < popped && i < 40; ++i) {
            asw::draw::circle_fill(
                { 16.0F + static_cast<float>(i) * 16.0F, 16.0F }, 5.0F, asw::color::gold);
        }

        if (autorun() && frame >= 160) {
            const bool saved = asw::display::screenshot("autorun.png");
            asw::log::info(saved ? "Saved autorun.png" : "Screenshot failed");
            asw::log::info("Objects: " + std::to_string(get_objects().size())
                + ", popped: " + std::to_string(popped)
                + ", fps: " + std::to_string(manager.get_fps()));
            asw::core::exit();
        }
    }

private:
    void spawn(const asw::Vec2<float>& position, int count)
    {
        for (int i = 0; i < count; ++i) {
            const asw::Vec2<float> jitter(
                asw::random::between(-40.0F, 40.0F), asw::random::between(-20.0F, 20.0F));
            create_object<Ball>(position + jitter, asw::random::between(12.0F, 36.0F));
        }
    }

    void click(const asw::Vec2<float>& position)
    {
        // Pop the frontmost ball under the mouse
        std::shared_ptr<Ball> target;
        for (const auto& ball : get_object_view<Ball>()) {
            if (ball->alive && ball->hit(position)
                && (target == nullptr || ball->z_index >= target->z_index)) {
                target = ball;
            }
        }

        if (target == nullptr) {
            spawn(position, 1);
            return;
        }

        target->alive = false;
        popped++;

        auto config = sparks;
        config.color_start = target->get_color().lighten(0.4F);
        config.color_end = target->get_color().with_alpha(0);
        create_object<Burst>(target->get_transform().get_center(), config);
    }

    void run_script()
    {
        if (!autorun()) {
            return;
        }

        if (frame == 20) {
            asw::input::simulate_key_down(asw::input::Key::Space);
        }
        if (frame == 21) {
            asw::input::simulate_key_up(asw::input::Key::Space);
        }

        // Pop whatever ball is first in the list
        if (frame == 120) {
            const auto balls = get_object_view<Ball>();
            if (!balls.empty()) {
                asw::input::simulate_mouse_move(balls.front()->get_transform().get_center());
                asw::input::simulate_mouse_button_down(asw::input::MouseButton::Left);
            }
        }
        if (frame == 121) {
            asw::input::simulate_mouse_button_up(asw::input::MouseButton::Left);
        }
    }

    asw::ParticleConfig sparks;
    int frame { 0 };
    int popped { 0 };
};
} // namespace

int main()
{
    asw::core::init(static_cast<int>(SCREEN_W), static_cast<int>(SCREEN_H));
    asw::display::set_title("ASW Example - Scenes");

    asw::scene::SceneManager<SceneId> manager;
    manager.register_scene<TitleScene>(SceneId::Title, manager);
    manager.register_scene<GameScene>(SceneId::Game, manager);
    manager.set_next_scene(SceneId::Title);

    // 60 updates per second, drawing runs as fast as it can
    manager.set_timestep(std::chrono::microseconds(16667));
    manager.start();

    asw::core::shutdown();
    return 0;
}
