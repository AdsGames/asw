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

    /// @brief Get angle of two vectors
    ///
    /// @return The angle of the vector in radians.
    ///
    T angle(const Vec2& other) const {
      return std::atan2(y - other.y, x - other.x);
    }

    /// @brief Calculate the angle of the vector.
    ///
    /// @return T The angle of the vector in radians.
    ///
    T angle() const {
      if (x == 0 && y == 0) {
        return 0;
      }
      return std::atan2(y, x);
    }

    /// @brief Get distance between two vectors
    ///
    /// @return The distance between the vectors.
    ///
    T distance(const Vec2& other) const {
      return std::hypotf(x - other.x, y - other.y);
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

    /// @brief The x component of the vector.
    T x{0};

    /// @brief The y component of the vector.
    T y{0};
  };

  /// @brief A 3D vector in space.
  ///
  /// @details This class represents a 3D vector in space. It is used to
  /// represent points, directions, and sizes.
  ///
  template <typename T>
  class Vec3 {
   public:
    /// @brief Default constructor for the Vec3 class.
    ///
    Vec3() : x(0), y(0), z(0) {}

    /// @brief Constructor for the Vec3 class.
    ///
    /// @param x The x component of the vector.
    /// @param y The y component of the vector.
    /// @param z The z component of the vector.
    ///
    Vec3(T x, T y, T z) : x(x), y(y), z(z) {}

    /// @brief Get angle between two vectors (in 3D space).
    ///
    /// @return The angle of the vector in radians.
    ///
    T angle(const Vec3& other) const {
      T dotProduct = dot(other);
      T magnitudes = magnitude() * other.magnitude();
      return std::acos(dotProduct / magnitudes);
    }

    /// @brief Get distance between two vectors.
    ///
    /// @return The distance between the vectors.
    ///
    T distance(const Vec3& other) const {
      return std::sqrt((x - other.x) * (x - other.x) +
                       (y - other.y) * (y - other.y) +
                       (z - other.z) * (z - other.z));
    }

    /// @brief Calculate the dot product of two vectors.
    ///
    /// @param other The vector to dot with.
    /// @return T The dot product of the vectors.
    ///
    T dot(const Vec3& other) const {
      return (x * other.x) + (y * other.y) + (z * other.z);
    }

    /// @brief Calculate the cross product of two vectors.
    ///
    /// @param other The vector to cross with.
    /// @return Vec3 The cross product of the vectors.
    ///
    Vec3 cross(const Vec3& other) const {
      return Vec3((y * other.z) - (z * other.y), (z * other.x) - (x * other.z),
                  (x * other.y) - (y * other.x));
    }

    /// @brief Calculate the magnitude of the vector.
    ///
    /// @return T The magnitude of the vector.
    ///
    T magnitude() const { return std::sqrt((x * x) + (y * y) + (z * z)); }

    /// @brief Addition operator for the Vec3 class.
    ///
    /// @param other The vector to add.
    /// @return Vec3 The sum of the vectors.
    ///
    Vec3 operator+(const Vec3& other) const {
      return Vec3(x + other.x, y + other.y, z + other.z);
    }

    /// @brief Subtraction operator for the Vec3 class.
    ///
    /// @param other The vector to subtract.
    /// @return Vec3 The difference of the vectors.
    ///
    Vec3 operator-(const Vec3& other) const {
      return Vec3(x - other.x, y - other.y, z - other.z);
    }

    /// @brief Multiplication operator for the Vec3 class.
    ///
    /// @param scalar The scalar to multiply by.
    /// @return Vec3 The scaled vector.
    ///
    Vec3 operator*(const T scalar) const {
      return Vec3(x * scalar, y * scalar, z * scalar);
    }

    /// @brief Division operator for the Vec3 class.
    ///
    /// @param scalar The scalar to divide by.
    /// @return Vec3 The scaled vector.
    ///
    Vec3 operator/(const T scalar) const {
      return Vec3(x / scalar, y / scalar, z / scalar);
    }

    /// @brief Addition assignment operator for the Vec3 class.
    ///
    /// @param other The vector to add.
    /// @return Vec3& The sum of the vectors.
    ///
    Vec3& operator+=(const Vec3& other) {
      x += other.x;
      y += other.y;
      z += other.z;
      return *this;
    }

    /// @brief Subtraction assignment operator for the Vec3 class.
    ///
    /// @param other The vector to subtract.
    /// @return Vec3& The difference of the vectors.
    ///
    Vec3& operator-=(const Vec3& other) {
      x -= other.x;
      y -= other.y;
      z -= other.z;
      return *this;
    }

    /// @brief Multiplication assignment operator for the Vec3 class.
    ///
    /// @param scalar The scalar to multiply by.
    /// @return Vec3& The scaled vector.
    ///
    Vec3& operator*=(const T scalar) {
      x *= scalar;
      y *= scalar;
      z *= scalar;
      return *this;
    }

    /// @brief Division assignment operator for the Vec3 class.
    ///
    /// @param scalar The scalar to divide by.
    /// @return Vec3& The scaled vector.
    ///
    Vec3& operator/=(const T scalar) {
      x /= scalar;
      y /= scalar;
      z /= scalar;
      return *this;
    }

    /// @brief Equality operator for the Vec3 class.
    ///
    /// @param other The vector to compare.
    /// @return bool True if the vectors are equal.
    ///
    bool operator==(const Vec3& other) const {
      return x == other.x && y == other.y && z == other.z;
    }

    /// @brief Inequality operator for the Vec3 class.
    ///
    /// @param other The vector to compare.
    /// @return bool True if the vectors are not equal.
    ///
    bool operator!=(const Vec3& other) const {
      return x != other.x || y != other.y || z != other.z;
    }

    /// @brief The x component of the vector.
    T x{0};

    /// @brief The y component of the vector.
    T y{0};

    /// @brief The z component of the vector.
    T z{0};
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

    /// @brief Set the position of the rectangle.
    ///
    /// @param x The x position of the rectangle.
    /// @param y The y position of the rectangle.
    ///
    void setPosition(T x, T y) { position = Vec2<T>(x, y); }

    /// @brief Set the size of the rectangle.
    ///
    /// @param width The width of the rectangle.
    /// @param height The height of the rectangle.
    ///
    void setSize(T width, T height) { size = Vec2<T>(width, height); }

    /// @brief Get center of the rectangle.
    ///
    /// @return Vec2 The center of the rectangle.
    ///
    Vec2<T> getCenter() const {
      return Vec2<T>(position.x + (size.x / 2.0F),
                     position.y + (size.y / 2.0F));
    }

    /// @brief Check if a point is inside the rectangle.
    ///
    /// @param point The point to check.
    /// @return bool True if the point is inside the rectangle.
    ///
    bool contains(const Vec2<T>& point) const {
      return point.x >= position.x && point.x <= position.x + size.x &&
             point.y >= position.y && point.y <= position.y + size.y;
    }

    /// @brief Check if coordinates are inside the rectangle.
    ///
    /// @param point The point to check.
    /// @return bool True if the point is inside the rectangle.
    ///
    bool contains(T x, T y) const {
      return x >= position.x && x <= position.x + size.x && y >= position.y &&
             y <= position.y + size.y;
    }

    /// @brief Check if a rectangle is inside the rectangle.
    ///
    /// @param other The rectangle to check.
    /// @return bool True if the rectangle is inside the rectangle.
    ///
    bool collides(const Quad& other) const {
      bool is_outside =
          position.x + size.x <= other.position.x ||        // a is left of b
          other.position.x + other.size.x <= position.x ||  // b is left of a
          position.y + size.y <= other.position.y ||        // a is above b
          other.position.y + other.size.y <= position.y;    // b is above a

      return !is_outside;
    }

    // Collision
    bool collidesBottom(const Quad& other) const {
      return position.y < other.position.y + other.size.y &&
             position.y + size.y > other.position.y + other.size.y;
    }

    bool collidesTop(const Quad& other) const {
      return position.y + size.y > other.position.y &&
             position.y < other.position.y;
    }

    bool collidesLeft(const Quad& other) const {
      return position.x + size.x > other.position.x &&
             position.x < other.position.x;
    }

    bool collidesRight(const Quad& other) const {
      return position.x < other.position.x + other.size.x &&
             position.x + size.x > other.position.x + other.size.x;
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

    /// @brief Multiply the rectangle by a scalar.
    ///
    /// @param scalar The scalar to multiply by.
    /// @return Quad The rectangle multiplied by the scalar.
    ///
    Quad operator*(const T scalar) const {
      return Quad(position * scalar, size * scalar);
    }

    /// @brief Divide the rectangle by a scalar.
    ///
    /// @param scalar The scalar to divide by.
    /// @return Quad The rectangle divided by the scalar.
    ///
    Quad operator/(const T scalar) const {
      return Quad(position / scalar, size / scalar);
    }

    /// @brief The position of the rectangle.
    Vec2<T> position;

    /// @brief The size of the rectangle.
    Vec2<T> size;
  };

}  // namespace asw

#endif  // ASW_GEOMETRY_H