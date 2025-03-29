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
    asw::Vec2<float> velocity;
    asw::Vec2<float> acceleration;
    float angularVelocity{0};
    float angularAcceleration{0};
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
    /// @param deltaTime The time since the last update.
    ///
    virtual void update(float deltaTime) {
      body.velocity += body.acceleration * deltaTime;
      transform.position += body.velocity * deltaTime;
      body.angularVelocity += body.angularAcceleration * deltaTime;
      rotation += body.angularVelocity * deltaTime;
    };

    /// @brief Draw the object to the screen.
    ///
    virtual void draw() {};

    /// @brief Set position
    ///
    /// @param x The x position of the object.
    /// @param y The y position of the object.
    ///
    void setPosition(float x, float y) {
      transform.position.x = x;
      transform.position.y = y;
    }

    /// @brief Set position
    ///
    /// @param position The position of the object.
    ///
    void setPosition(const asw::Vec2<float>& position) {
      transform.position = position;
    }

    /// @brief Set size
    ///
    /// @param width The width of the object.
    /// @param height The height of the object.
    ///
    void setSize(float width, float height) {
      transform.size.x = width;
      transform.size.y = height;
    }

    /// @brief Set size
    ///
    /// @param size The size of the object.
    ///
    void setSize(const asw::Vec2<float>& size) { transform.size = size; }

    /// @brief The transform of the object.
    ///
    asw::Quad<float> transform;

    /// @brief The rotation of the object in degrees.
    ///
    float rotation{0};

    /// @brief The layer that the object is on.
    /// @details Objects on higher layers are drawn on top of objects on lower
    /// layers.
    ///
    int zIndex{0};

    /// @brief Whether or not the object is active.
    /// @details If an object is not active, it will not be updated or drawn.
    ///
    bool active{true};

    /// @brief Opacity of the object.
    ///
    float alpha{1.0F};

    /// @brief Physics body of the object.
    ///
    Physics body;
  };

  /// @brief Sprite Object
  ///
  class Sprite : public GameObject {
   public:
    /// @brief Set the texture of the sprite.
    ///
    /// @param texture The texture to set.
    /// @param autoSize Whether or not to automatically set the size of the
    /// sprite based on the texture dimensions.
    ///
    void setTexture(const asw::Texture& texture, bool autoSize = true) {
      this->texture = texture;

      if (autoSize) {
        transform.size = asw::util::getTextureSize(texture);
      }
    }

    /// @brief Update the sprite.
    ///
    /// @param deltaTime The time since the last update.
    ///
    void update(float deltaTime) override { GameObject::update(deltaTime); }

    /// @brief Draw the sprite to the screen.
    /// @details If a rotation is set, the sprite will be rotated around its
    /// center.
    ///
    void draw() override {
      if (texture == nullptr) {
        return;
      }

      if (alpha < 1.0F) {
        asw::draw::setAlpha(texture, alpha);
      }

      if (rotation) {
        asw::draw::rotateSprite(texture, transform.position, rotation);
      } else {
        asw::draw::stretchSprite(texture, transform);
      }

      if (alpha < 1.0F) {
        asw::draw::setAlpha(texture, 1.0F);
      }
    }

   private:
    /// @brief The texture of the sprite.
    ///
    asw::Texture texture;
  };

  /// @brief Text Object
  ///
  class Text : public GameObject {
   public:
    /// @brief Set the font of the text.
    ///
    /// @param font The font to set.
    ///
    void setFont(const asw::Font& font) { this->font = font; }

    /// @brief Set the text of the text object.
    ///
    /// @param text The text to set.
    ///
    void setText(const std::string& text) { this->text = text; }

    /// @brief Set the color of the text.
    ///
    /// @param color The color to set.
    ///
    void setColor(const asw::Color& color) { this->color = color; }

    /// @brief Set the size of the text.
    ///
    /// @param size The size of the text.
    ///
    void update(float deltaTime) override { GameObject::update(deltaTime); }

    void draw() override {
      asw::draw::text(font, text, transform.position, color);
    }

   private:
    std::string text;
    asw::Font font;
    asw::Color color;
  };

};  // namespace asw::game

#endif  // ASW_COMPONENTS_H