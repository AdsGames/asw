/// @file components.h
/// @author Allan Legemaate (alegemaate@gmail.com)
/// @brief Common game modules
/// @date 2025-03-26
///
/// @copyright Copyright (c) 2025
///

#ifndef ASW_COMPONENTS_H
#define ASW_COMPONENTS_H

#include <string>

#include "./color.h"
#include "./draw.h"
#include "./geometry.h"
#include "./util.h"

namespace asw::game {
/// Components
///

/// @brief Physics Component
///
class Physics {
public:
    // Velocity in pixels per second
    asw::Vec2<float> velocity;
    // Acceleration in pixels per second squared
    asw::Vec2<float> acceleration;
    // Angular velocity in radians per second
    float angular_velocity { 0.0F };
    // Angular acceleration in radians per second squared
    float angular_acceleration { 0.0F };
};

/// Objects
///

/// @brief  Game Object
///
class GameObject {
public:
    GameObject() = default;

    virtual ~GameObject() = default;

    /// @brief Update the object.
    ///
    /// @param dt The time in seconds since the last update.
    ///
    virtual void update(float dt)
    {
        body.velocity += body.acceleration * dt;
        transform.position += body.velocity * dt;
        body.angular_velocity += body.angular_acceleration * dt;
        rotation += body.angular_velocity * dt;
    };

    /// @brief Draw the object to the screen.
    ///
    virtual void draw() {
        // Do nothing by default
    };

    /// @brief Get transform
    ///
    /// @return The position of the object.
    ///
    const asw::Quad<float>& get_transform() const
    {
        return transform;
    }

    /// @brief The transform of the object.
    ///
    asw::Quad<float> transform;

    /// @brief The rotation of the object in radians.
    ///
    float rotation { 0.0F };

    /// @brief The layer that the object is on.
    /// @details Objects on higher layers are drawn on top of objects on lower
    /// layers.
    ///
    int z_index { 0 };

    /// @brief Whether or not the object is active.
    /// @details If an object is not active, it will not be updated or drawn.
    ///
    bool active { true };

    /// @brief Opacity of the object.
    ///
    float alpha { 1.0F };

    /// @brief Physics body of the object.
    ///
    Physics body;

    /// @brief Alive state
    ///
    bool alive { true };
};

/// @brief Sprite Object
///
class Sprite : public GameObject {
public:
    /// @brief Set the texture of the sprite.
    ///
    /// @param texture The texture to set.
    /// @param auto_size Whether or not to automatically set the size of the
    /// sprite based on the texture dimensions.
    ///
    void set_texture(const asw::Texture& texture, bool auto_size = true)
    {
        this->texture_ = texture;

        if (auto_size) {
            transform.size = asw::util::get_texture_size(texture_);
        }
    }

    /// @brief Draw the sprite to the screen.
    /// @details If a rotation is set, the sprite will be rotated around its
    /// center.
    ///
    void draw() override
    {
        if (texture_ == nullptr) {
            return;
        }

        if (alpha < 1.0F) {
            asw::draw::set_alpha(texture_, alpha);
        }

        if (rotation != 0.0F) {
            asw::draw::rotate_sprite(texture_, transform.position, rotation);
        } else {
            asw::draw::stretch_sprite(texture_, transform);
        }

        if (alpha < 1.0F) {
            asw::draw::set_alpha(texture_, 1.0F);
        }
    }

private:
    /// @brief The texture of the sprite.
    ///
    asw::Texture texture_;
};

/// @brief Text Object
///
class Text : public GameObject {
public:
    /// @brief Set the font of the text.
    ///
    /// @param font The font to set.
    ///
    void set_font(const asw::Font& font)
    {
        this->font_ = font;
    }

    /// @brief Set the text of the text object.
    ///
    /// @param text The text to set.
    ///
    void set_text(const std::string_view& text)
    {
        this->text_ = text;
    }

    /// @brief Set the color of the text.
    ///
    /// @param color The color to set.
    ///
    void set_color(const asw::Color& color)
    {
        this->color_ = color;
    }

    /// @brief Set the justification of the text.
    ///
    /// @param justify The justification to set.
    ///
    void set_justify(asw::TextJustify justify)
    {
        this->justify_ = justify;
    }

    /// @brief Draw the text to the screen.
    ///
    void draw() override
    {
        asw::draw::text(font_, text_, transform.position, color_, justify_);
    }

private:
    std::string text_;
    asw::Font font_;
    asw::Color color_;
    asw::TextJustify justify_ { asw::TextJustify::Left };
};

}; // namespace asw::game

#endif // ASW_COMPONENTS_H