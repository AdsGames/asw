/// @file sprite_sheet.h
/// @author Allan Legemaate (alegemaate@gmail.com)
/// @brief Sprite sheets and frame animation
/// @date 2026-09-27
///
/// @copyright Copyright (c) 2026
///

#ifndef ASW_SPRITE_SHEET_H
#define ASW_SPRITE_SHEET_H

#include "./geometry.h"
#include "./types.h"

namespace asw {

/// @brief A texture split into equal frames, read left to right then top to
/// bottom.
///
class SpriteSheet {
public:
    /// @brief Create an empty sprite sheet.
    ///
    SpriteSheet() = default;

    /// @brief Create a sprite sheet.
    ///
    /// @param texture The texture holding the frames.
    /// @param frame_size The size of one frame in pixels.
    /// @param frame_count The number of frames, 0 to use every frame that fits
    /// in the texture.
    ///
    SpriteSheet(const Texture& texture, const Vec2<float>& frame_size, int frame_count = 0);

    /// @brief Get the texture.
    ///
    /// @return The texture.
    ///
    const Texture& get_texture() const;

    /// @brief Get the size of one frame.
    ///
    /// @return The frame size in pixels.
    ///
    Vec2<float> get_frame_size() const;

    /// @brief Get the number of frames.
    ///
    /// @return The frame count.
    ///
    int get_frame_count() const;

    /// @brief Get the area of the texture a frame covers.
    ///
    /// @param index The frame, wraps around past the last frame.
    /// @return The source area in pixels.
    ///
    Quad<float> get_frame(int index) const;

    /// @brief Draw a frame stretched to an area of the screen.
    ///
    /// @param index The frame, wraps around past the last frame.
    /// @param dest The area to draw to.
    ///
    void draw_frame(int index, const Quad<float>& dest) const;

private:
    Texture texture;
    Vec2<float> frame_size;
    int columns { 0 };
    int frame_count { 0 };
};

/// @brief Steps through frames over time, to use with a SpriteSheet.
///
class Animation {
public:
    /// @brief Create an animation with a single frame.
    ///
    Animation() = default;

    /// @brief Create an animation.
    ///
    /// @param frame_count The number of frames.
    /// @param frame_duration Seconds each frame is shown.
    /// @param loop Whether to start over after the last frame.
    ///
    Animation(int frame_count, float frame_duration, bool loop = true);

    /// @brief Advance the animation.
    ///
    /// @param dt The time in seconds since the last update.
    ///
    void update(float dt);

    /// @brief Go back to the first frame.
    ///
    void reset();

    /// @brief Set seconds each frame is shown.
    ///
    /// @param frame_duration The duration in seconds.
    ///
    void set_frame_duration(float frame_duration);

    /// @brief Get the frame to draw.
    ///
    /// @return The frame index.
    ///
    int get_frame() const;

    /// @brief Check if a non looping animation has reached its last frame.
    ///
    /// @return True when finished, always false for looping animations.
    ///
    bool is_finished() const;

private:
    int frame_count { 1 };
    float frame_duration { 0.1F };
    bool loop { true };
    float elapsed { 0.0F };
};

} // namespace asw

#endif // ASW_SPRITE_SHEET_H
