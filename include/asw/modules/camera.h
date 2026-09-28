/// @file camera.h
/// @author Allan Legemaate (alegemaate@gmail.com)
/// @brief 2D camera for scrolling worlds
/// @date 2026-09-27
///
/// @copyright Copyright (c) 2026
///

#ifndef ASW_CAMERA_H
#define ASW_CAMERA_H

#include "./geometry.h"

namespace asw {

/// @brief 2D camera that follows a target, stays inside the world and shakes.
///
/// @details Drawing in asw is in screen space. Keep game objects in world
/// space and convert with world_to_screen when drawing.
///
class Camera {
public:
    /// @brief Create a camera with no view size. Call set_view_size before use.
    ///
    Camera() = default;

    /// @brief Create a camera.
    ///
    /// @param view_size Size of the area the camera shows, usually the logical
    /// screen size.
    ///
    explicit Camera(const Vec2<float>& view_size);

    /// @brief Set the size of the area the camera shows.
    ///
    /// @param view_size The view size in pixels.
    ///
    void set_view_size(const Vec2<float>& view_size);

    /// @brief Keep the view inside an area of the world. If the area is
    /// smaller than the view, the view is centred on it.
    ///
    /// @param bounds The world area.
    ///
    void set_bounds(const Quad<float>& bounds);

    /// @brief Let the view move anywhere.
    ///
    void clear_bounds();

    /// @brief Set the point in the view where the followed target is kept.
    /// Defaults to the middle of the view.
    ///
    /// @param anchor The point, relative to the top left of the view.
    ///
    void set_anchor(const Vec2<float>& anchor);

    /// @brief Keep the followed target in the middle of the view again.
    ///
    void clear_anchor();

    /// @brief Set how quickly follow catches up with its target.
    ///
    /// @param speed Higher is faster. 0 jumps straight to the target.
    ///
    void set_follow_speed(float speed);

    /// @brief Set how quickly screen shake fades.
    ///
    /// @param decay Shake strength lost per second, in pixels.
    ///
    void set_shake_decay(float decay);

    /// @brief Move straight to a target and stop any shake.
    ///
    /// @param target The world position to keep at the anchor.
    ///
    void snap_to(const Vec2<float>& target);

    /// @brief Ease towards a target. Call once per update.
    ///
    /// @param target The world position to keep at the anchor.
    /// @param dt The time in seconds since the last update.
    ///
    void follow(const Vec2<float>& target, float dt);

    /// @brief Advance screen shake. Call once per update.
    ///
    /// @param dt The time in seconds since the last update.
    ///
    void update(float dt);

    /// @brief Start a screen shake. A weaker shake does not cut short a
    /// stronger one already running.
    ///
    /// @param strength Largest offset in pixels.
    ///
    void shake(float strength);

    /// @brief Set the top left of the view in world space.
    ///
    /// @param position The world position.
    ///
    void set_position(const Vec2<float>& position);

    /// @brief Get the top left of the view in world space, without shake.
    ///
    /// @return The world position.
    ///
    Vec2<float> get_position() const;

    /// @brief Get the visible world area, including shake.
    ///
    /// @return The view in world space.
    ///
    Quad<float> get_view() const;

    /// @brief Convert a world position to screen space.
    ///
    /// @param world The world position.
    /// @return The screen position.
    ///
    Vec2<float> world_to_screen(const Vec2<float>& world) const;

    /// @brief Convert a world area to screen space.
    ///
    /// @param world The world area.
    /// @return The screen area.
    ///
    Quad<float> world_to_screen(const Quad<float>& world) const;

    /// @brief Convert a screen position, such as the mouse, to world space.
    ///
    /// @param screen The screen position.
    /// @return The world position.
    ///
    Vec2<float> screen_to_world(const Vec2<float>& screen) const;

private:
    void clamp_to_bounds();

    Vec2<float> position;
    Vec2<float> view_size;
    Vec2<float> anchor;
    Vec2<float> shake_offset;
    Quad<float> bounds;
    bool has_bounds { false };
    bool has_anchor { false };
    float follow_speed { 8.0F };
    float shake_strength { 0.0F };
    float shake_decay { 60.0F };
};

} // namespace asw

#endif // ASW_CAMERA_H
