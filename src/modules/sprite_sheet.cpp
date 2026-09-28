/// @file sprite_sheet.cpp
/// @author Allan Legemaate (alegemaate@gmail.com)
/// @brief Sprite sheet and animation implementation
/// @date 2026-09-27
///
/// @copyright Copyright (c) 2026
///

#include "asw/modules/sprite_sheet.h"

#include <algorithm>
#include <cmath>

#include "asw/modules/draw.h"
#include "asw/modules/util.h"

namespace asw {

SpriteSheet::SpriteSheet(const Texture& texture, const Vec2<float>& frame_size, int frame_count)
    : texture(texture)
    , frame_size(frame_size)
{
    if (texture == nullptr || frame_size.x <= 0.0F || frame_size.y <= 0.0F) {
        return;
    }

    const auto texture_size = util::get_texture_size(texture);
    columns = std::max(1, static_cast<int>(std::floor(texture_size.x / frame_size.x)));
    const int rows = std::max(1, static_cast<int>(std::floor(texture_size.y / frame_size.y)));
    const int fits = columns * rows;

    this->frame_count = frame_count > 0 ? std::min(frame_count, fits) : fits;
}

const Texture& SpriteSheet::get_texture() const
{
    return texture;
}

Vec2<float> SpriteSheet::get_frame_size() const
{
    return frame_size;
}

int SpriteSheet::get_frame_count() const
{
    return frame_count;
}

Quad<float> SpriteSheet::get_frame(int index) const
{
    if (frame_count <= 0) {
        return { 0.0F, 0.0F, frame_size.x, frame_size.y };
    }

    // Wrap, negative indexes count back from the end
    const int frame = ((index % frame_count) + frame_count) % frame_count;
    const auto column = static_cast<float>(frame % columns);
    const auto row = static_cast<float>(frame / columns);

    return { column * frame_size.x, row * frame_size.y, frame_size.x, frame_size.y };
}

void SpriteSheet::draw_frame(int index, const Quad<float>& dest) const
{
    if (texture == nullptr) {
        return;
    }

    draw::stretch_sprite_blit(texture, get_frame(index), dest);
}

Animation::Animation(int frame_count, float frame_duration, bool loop)
    : frame_count(std::max(1, frame_count))
    , frame_duration(std::max(0.0001F, frame_duration))
    , loop(loop)
{
}

void Animation::update(float dt)
{
    elapsed += dt;

    // Keep time small so float precision holds up over long runs, and so the
    // frame index never overflows
    const float length = frame_duration * static_cast<float>(frame_count);
    if (elapsed >= length) {
        elapsed = loop ? std::fmod(elapsed, length) : length;
    }
}

void Animation::reset()
{
    elapsed = 0.0F;
}

void Animation::set_frame_duration(float frame_duration)
{
    this->frame_duration = std::max(0.0001F, frame_duration);
}

int Animation::get_frame() const
{
    const int frame = static_cast<int>(elapsed / frame_duration);

    if (loop) {
        return frame % frame_count;
    }

    return std::min(frame, frame_count - 1);
}

bool Animation::is_finished() const
{
    return !loop && elapsed >= frame_duration * static_cast<float>(frame_count);
}

} // namespace asw
