/// @file camera.cpp
/// @author Allan Legemaate (alegemaate@gmail.com)
/// @brief 2D camera implementation
/// @date 2026-09-27
///
/// @copyright Copyright (c) 2026
///

#include "asw/modules/camera.h"

#include <algorithm>
#include <cmath>

#include "asw/modules/random.h"

namespace asw {

Camera::Camera(const Vec2<float>& view_size)
    : view_size(view_size)
{
}

void Camera::set_view_size(const Vec2<float>& view_size)
{
    this->view_size = view_size;
    clamp_to_bounds();
}

void Camera::set_bounds(const Quad<float>& bounds)
{
    this->bounds = bounds;
    has_bounds = true;
    clamp_to_bounds();
}

void Camera::clear_bounds()
{
    has_bounds = false;
}

void Camera::set_anchor(const Vec2<float>& anchor)
{
    this->anchor = anchor;
    has_anchor = true;
}

void Camera::clear_anchor()
{
    has_anchor = false;
}

void Camera::set_follow_speed(float speed)
{
    follow_speed = std::max(0.0F, speed);
}

void Camera::set_shake_decay(float decay)
{
    shake_decay = std::max(0.0F, decay);
}

void Camera::snap_to(const Vec2<float>& target)
{
    const auto point = has_anchor ? anchor : view_size / 2.0F;
    position = target - point;
    shake_strength = 0.0F;
    shake_offset = Vec2<float>(0.0F, 0.0F);
    clamp_to_bounds();
}

void Camera::follow(const Vec2<float>& target, float dt)
{
    if (follow_speed <= 0.0F) {
        const float strength = shake_strength;
        snap_to(target);
        shake_strength = strength;
        return;
    }

    // Frame rate independent ease
    const auto point = has_anchor ? anchor : view_size / 2.0F;
    const float t = 1.0F - std::exp(-follow_speed * dt);
    position += ((target - point) - position) * t;
    clamp_to_bounds();
}

void Camera::update(float dt)
{
    shake_strength = std::max(0.0F, shake_strength - (shake_decay * dt));

    if (shake_strength > 0.0F) {
        shake_offset = Vec2<float>(random::between(-1.0F, 1.0F), random::between(-1.0F, 1.0F))
            * shake_strength;
    } else {
        shake_offset = Vec2<float>(0.0F, 0.0F);
    }
}

void Camera::shake(float strength)
{
    shake_strength = std::max(shake_strength, strength);
}

void Camera::set_position(const Vec2<float>& position)
{
    this->position = position;
    clamp_to_bounds();
}

Vec2<float> Camera::get_position() const
{
    return position;
}

Quad<float> Camera::get_view() const
{
    return { position + shake_offset, view_size };
}

Vec2<float> Camera::world_to_screen(const Vec2<float>& world) const
{
    return world - (position + shake_offset);
}

Quad<float> Camera::world_to_screen(const Quad<float>& world) const
{
    return { world_to_screen(world.position), world.size };
}

Vec2<float> Camera::screen_to_world(const Vec2<float>& screen) const
{
    return screen + (position + shake_offset);
}

void Camera::clamp_to_bounds()
{
    if (!has_bounds) {
        return;
    }

    // Centre on bounds smaller than the view
    if (bounds.size.x <= view_size.x) {
        position.x = bounds.position.x - ((view_size.x - bounds.size.x) / 2.0F);
    } else {
        position.x = std::clamp(
            position.x, bounds.position.x, bounds.position.x + bounds.size.x - view_size.x);
    }

    if (bounds.size.y <= view_size.y) {
        position.y = bounds.position.y - ((view_size.y - bounds.size.y) / 2.0F);
    } else {
        position.y = std::clamp(
            position.y, bounds.position.y, bounds.position.y + bounds.size.y - view_size.y);
    }
}

} // namespace asw
