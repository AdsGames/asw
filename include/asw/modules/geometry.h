/// @file geometry.h
/// @author Allan Legemaate (alegemaate@gmail.com)
/// @brief Common geometry types
/// @date 2025-03-26
///
/// @copyright Copyright (c) 2025
///

#ifndef ASW_GEOMETRY_H
#define ASW_GEOMETRY_H

#include <cmath>

namespace asw {

  /// @brief A 2D vector in space.
  ///
  /// @details This class represents a 2D vector in space. It is used to
  /// represent points, directions, and sizes.
  ///
  template <typename T>
  class Vec2 {
   public:
    /// @brief Default constructor for the Vec2 class.
    ///
    Vec2() : x(0), y(0) {}

    /// @brief Constructor for the Vec2 class.
    ///
    /// @param x The x component of the vector.
    /// @param y The y component of the vector.
    ///
    Vec2(T x, T y) : x(x), y(y) {}

    /// @brief Addition operator for the Vec2 class.
    ///
    /// @param other The vector to add.
    /// @return Vec2 The sum of the vectors.
    ///
    Vec2 operator+(const Vec2& other) const {
      return Vec2(x + other.x, y + other.y);
    }

    /// @brief Subtraction operator for the Vec2 class.
    ///
    /// @param other The vector to subtract.
    /// @return Vec2 The difference of the vectors.
    ///
    Vec2 operator-(const Vec2& other) const {
      return Vec2(x - other.x, y - other.y);
    }

    /// @brief Multiplication operator for the Vec2 class.
    ///
    /// @param scalar The scalar to multiply by.
    /// @return Vec2 The scaled vector.
    ///
    Vec2 operator*(const T scalar) const {
      return Vec2(x * scalar, y * scalar);
    }

    /// @brief Division operator for the Vec2 class.
    ///
    /// @param scalar The scalar to divide by.
    /// @return Vec2 The scaled vector.
    ///
    Vec2 operator/(const T scalar) const {
      return Vec2(x / scalar, y / scalar);
    }

    /// @brief Addition assignment operator for the Vec2 class.
    ///
    /// @param other The vector to add.
    /// @return Vec2& The sum of the vectors.
    ///
    Vec2& operator+=(const Vec2& other) {
      x += other.x;
      y += other.y;
      return *this;
    }

    /// @brief Subtraction assignment operator for the Vec2 class.
    ///
    /// @param other The vector to subtract.
    /// @return Vec2& The difference of the vectors.
    ///
    Vec2& operator-=(const Vec2& other) {
      x -= other.x;
      y -= other.y;
      return *this;
    }

    /// @brief Multiplication assignment operator for the Vec2 class.
    ///
    /// @param scalar The scalar to multiply by.
    /// @return Vec2& The scaled vector.
    ///
    Vec2& operator*=(const T scalar) {
      x *= scalar;
      y *= scalar;
      return *this;
    }

    /// @brief Division assignment operator for the Vec2 class.
    ///
    /// @param scalar The scalar to divide by.
    /// @return Vec2& The scaled vector.
    ///
    Vec2& operator/=(const T scalar) {
      x /= scalar;
      y /= scalar;
      return *this;
    }

    /// @brief Equality operator for the Vec2 class.
    ///
    /// @param other The vector to compare.
    /// @return bool True if the vectors are equal.
    ///
    bool operator==(const Vec2& other) const {
      return x == other.x && y == other.y;
    }

    /// @brief Inequality operator for the Vec2 class.
    ///
    /// @param other The vector to compare.
    /// @return bool True if the vectors are not equal.
    ///
    bool operator!=(const Vec2& other) const {
      return x != other.x || y != other.y;
    }

    /// @brief Calculate the dot product of two vectors.
    ///
    /// @param other The vector to dot with.
    /// @return T The dot product of the vectors.
    ///
    T dot(const Vec2& other) const { return (x * other.x) + (y * other.y); }

    /// @brief Calculate the cross product of two vectors.
    ///
    /// @param other The vector to cross with.
    /// @return T The cross product of the vectors.
    ///
    T cross(const Vec2& other) const { return (x * other.y) - (y * other.x); }

    /// @brief Calculate the magnitude of the vector.
    ///
    /// @return T The magnitude of the vector.
    ///
    T magnitude() const { return std::sqrt((x * x) + (y * y)); }

    /// @brief The x component of the vector.
    T x{0};

    /// @brief The y component of the vector.
    T y{0};
  };

  /// @brief A 2D rectangle in space.
  ///
  /// @details This class represents a 2D rectangle in space. It is used to
  /// represent areas, sizes, and positions.
  ///
  template <typename T>
  class Quad {
   public:
    /// @brief Default constructor for the Quad class.
    ///
    Quad() : position(0, 0), size(0, 0) {}

    /// @brief Constructor for the Quad class.
    ///
    /// @param position The position of the rectangle.
    /// @param size The size of the rectangle.
    ///
    Quad(const Vec2<T>& position, const Vec2<T>& size)
        : position(position), size(size) {}

    /// @brief Constructor for the Quad class.
    ///
    /// @param x The x position of the rectangle.
    /// @param y The y position of the rectangle.
    /// @param width The width of the rectangle.
    /// @param height The height of the rectangle.
    ///
    Quad(T x, T y, T width, T height) : position(x, y), size(width, height) {}

    /// @brief Check if a point is inside the rectangle.
    ///
    /// @param point The point to check.
    /// @return bool True if the point is inside the rectangle.
    ///
    bool contains(const Vec2<T>& point) const {
      return point.x >= position.x && point.x <= position.x + size.x &&
             point.y >= position.y && point.y <= position.y + size.y;
    }

    /// @brief Check if a rectangle is inside the rectangle.
    ///
    /// @param other The rectangle to check.
    /// @return bool True if the rectangle is inside the rectangle.
    ///
    bool contains(const Quad& other) const {
      return other.position.x >= position.x &&
             other.position.x + other.size.x <= position.x + size.x &&
             other.position.y >= position.y &&
             other.position.y + other.size.y <= position.y + size.y;
    }

    /// @brief Add a vector to the rectangle.
    ///
    /// @param vec The vector to add.
    /// @return Quad The rectangle with the vector added.
    ///
    Quad operator+(const Quad<T>& quad) const {
      return Quad(position + quad.position, size + quad.size);
    }

    /// @brief Subtract a vector from the rectangle.
    ///
    /// @param vec The vector to subtract.
    /// @return Quad The rectangle with the vector subtracted.
    ///
    Quad operator-(const Quad<T>& quad) const {
      return Quad(position - quad.position, size - quad.size);
    }

    /// @brief The position of the rectangle.
    Vec2<T> position;

    /// @brief The size of the rectangle.
    Vec2<T> size;
  };

}  // namespace asw

#endif  // ASW_GEOMETRY_H