/// @file geometry.h
/// @author Allan Legemaate (alegemaate@gmail.com)
/// @brief Common geometry types
/// @date 2025-03-26
///
/// @copyright Copyright (c) 2025
///

#ifndef ASW_GEOMETRY_H
#define ASW_GEOMETRY_H

#include <algorithm>
#include <cmath>
#include <concepts>
#include <cstddef>
#include <optional>
#include <type_traits>
#include <vector>

namespace asw {

/// @brief A 2D vector in space.
///
/// @details This class represents a 2D vector in space. It is used to
/// represent points, directions, and sizes.
///
template <typename T> class Vec2 {
public:
    /// @brief Type used for lengths and angles. Integer vectors use float so
    /// results are not truncated.
    using Real = std::conditional_t<std::is_floating_point_v<T>, T, float>;

    /// @brief Default constructor for the Vec2 class.
    ///
    Vec2() = default;

    /// @brief Constructor for the Vec2 class.
    ///
    /// @param x The x component of the vector.
    /// @param y The y component of the vector.
    ///
    Vec2(T x, T y)
        : x(x)
        , y(y)
    {
    }

    /// @brief Get angle of two vectors
    ///
    /// @return The angle of the vector in radians.
    ///
    Real angle(const Vec2& other) const
    {
        return std::atan2(static_cast<Real>(y - other.y), static_cast<Real>(x - other.x));
    }

    /// @brief Calculate the angle of the vector.
    ///
    /// @return T The angle of the vector in radians.
    ///
    Real angle() const
    {
        if (x == 0 && y == 0) {
            return 0;
        }
        return std::atan2(static_cast<Real>(y), static_cast<Real>(x));
    }

    /// @brief Create a vector pointing at an angle.
    ///
    /// @param angle The angle in radians, clockwise from the positive x axis
    /// like the rest of asw.
    /// @param length The length of the vector.
    /// @return Vec2 The vector.
    ///
    static Vec2 from_angle(T angle, T length = T(1))
        requires std::floating_point<T>
    {
        return Vec2(std::cos(angle) * length, std::sin(angle) * length);
    }

    /// @brief Get distance between two vectors
    ///
    /// @return The distance between the vectors.
    ///
    Real distance(const Vec2& other) const
    {
        return std::hypot(static_cast<Real>(x - other.x), static_cast<Real>(y - other.y));
    }

    /// @brief Get the distance from this point to the nearest point on a line
    /// segment. Useful for beams, lasers and swept hit checks.
    ///
    /// @param start One end of the segment.
    /// @param end The other end of the segment.
    /// @return The distance, or the distance to @p start if both ends are the
    /// same point.
    ///
    Real distance_to_segment(const Vec2& start, const Vec2& end) const
    {
        const auto seg_x = static_cast<Real>(end.x - start.x);
        const auto seg_y = static_cast<Real>(end.y - start.y);
        const auto rel_x = static_cast<Real>(x - start.x);
        const auto rel_y = static_cast<Real>(y - start.y);

        const Real length_sq = (seg_x * seg_x) + (seg_y * seg_y);
        const Real t = length_sq > Real(0)
            ? std::clamp(((rel_x * seg_x) + (rel_y * seg_y)) / length_sq, Real(0), Real(1))
            : Real(0);

        return std::hypot(rel_x - (seg_x * t), rel_y - (seg_y * t));
    }

    /// @brief Calculate the dot product of two vectors.
    ///
    /// @param other The vector to dot with.
    /// @return T The dot product of the vectors.
    ///
    T dot(const Vec2& other) const
    {
        return (x * other.x) + (y * other.y);
    }

    /// @brief Calculate the cross product of two vectors.
    ///
    /// @param other The vector to cross with.
    /// @return T The cross product of the vectors.
    ///
    T cross(const Vec2& other) const
    {
        return (x * other.y) - (y * other.x);
    }

    /// @brief Calculate the magnitude of the vector.
    ///
    /// @return T The magnitude of the vector.
    ///
    Real magnitude() const
    {
        return std::sqrt(static_cast<Real>((x * x) + (y * y)));
    }

    /// @brief Get a vector with the same direction and a length of one.
    ///
    /// @return Vec2 The unit vector, or a zero vector if this vector has no
    /// length.
    ///
    Vec2 normalized() const
        requires std::floating_point<T>
    {
        const T length = magnitude();
        if (length == T(0)) {
            return Vec2(T(0), T(0));
        }

        return Vec2(x / length, y / length);
    }

    /// @brief Addition operator for the Vec2 class.
    ///
    /// @param other The vector to add.
    /// @return Vec2 The sum of the vectors.
    ///
    Vec2 operator+(const Vec2& other) const
    {
        return Vec2(x + other.x, y + other.y);
    }

    /// @brief Subtraction operator for the Vec2 class.
    ///
    /// @param other The vector to subtract.
    /// @return Vec2 The difference of the vectors.
    ///
    Vec2 operator-(const Vec2& other) const
    {
        return Vec2(x - other.x, y - other.y);
    }

    /// @brief Multiplication operator for the Vec2 class.
    ///
    /// @param scalar The scalar to multiply by.
    /// @return Vec2 The scaled vector.
    ///
    Vec2 operator*(const T scalar) const
    {
        return Vec2(x * scalar, y * scalar);
    }

    /// @brief Division operator for the Vec2 class.
    ///
    /// @param scalar The scalar to divide by.
    /// @return Vec2 The scaled vector.
    ///
    Vec2 operator/(const T scalar) const
    {
        return Vec2(x / scalar, y / scalar);
    }

    /// @brief Modulo the Vec2 object by a scalar
    ///
    /// @param scalar The scalar to modulo by
    /// @return Vec2 The result of the modulo operation.
    ///
    Vec2 operator%(const T scalar) const
    {
        return Vec2(x % scalar, y % scalar);
    }

    /// @brief Addition assignment operator for the Vec2 class.
    ///
    /// @param other The vector to add.
    /// @return Vec2& The sum of the vectors.
    ///
    Vec2& operator+=(const Vec2& other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    /// @brief Subtraction assignment operator for the Vec2 class.
    ///
    /// @param other The vector to subtract.
    /// @return Vec2& The difference of the vectors.
    ///
    Vec2& operator-=(const Vec2& other)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    /// @brief Multiplication assignment operator for the Vec2 class.
    ///
    /// @param scalar The scalar to multiply by.
    /// @return Vec2& The scaled vector.
    ///
    Vec2& operator*=(const T scalar)
    {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    /// @brief Division assignment operator for the Vec2 class.
    ///
    /// @param scalar The scalar to divide by.
    /// @return Vec2& The scaled vector.
    ///
    Vec2& operator/=(const T scalar)
    {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    /// @brief Modulo assignment operator for the Vec2 class.
    ///
    /// @param scalar The scalar to modulo by.
    /// @return Vec2& The result of the modulo operation.
    ///
    Vec2& operator%=(const T scalar)
    {
        x %= scalar;
        y %= scalar;
        return *this;
    }

    /// @brief Equality operator for the Vec2 class.
    ///
    /// @param other The vector to compare.
    /// @return bool True if the vectors are equal.
    ///
    bool operator==(const Vec2& other) const = default;

    /// @brief The x component of the vector.
    T x { 0 };

    /// @brief The y component of the vector.
    T y { 0 };
};

/// @brief A 3D vector in space.
///
/// @details This class represents a 3D vector in space. It is used to
/// represent points, directions, and sizes.
///
template <typename T> class Vec3 {
public:
    /// @brief Type used for lengths and angles. Integer vectors use float so
    /// results are not truncated.
    using Real = std::conditional_t<std::is_floating_point_v<T>, T, float>;

    /// @brief Default constructor for the Vec3 class.
    ///
    Vec3() = default;

    /// @brief Constructor for the Vec3 class.
    ///
    /// @param x The x component of the vector.
    /// @param y The y component of the vector.
    /// @param z The z component of the vector.
    ///
    Vec3(T x, T y, T z)
        : x(x)
        , y(y)
        , z(z)
    {
    }

    /// @brief Get angle between two vectors (in 3D space).
    ///
    /// @return The angle of the vector in radians.
    ///
    Real angle(const Vec3& other) const
    {
        const auto magnitudes = magnitude() * other.magnitude();
        if (magnitudes == Real(0)) {
            return 0;
        }

        // Rounding can push the ratio just past 1, which acos turns into NaN
        const auto ratio = static_cast<Real>(dot(other)) / magnitudes;
        return std::acos(std::clamp(ratio, Real(-1), Real(1)));
    }

    /// @brief Get distance between two vectors.
    ///
    /// @return The distance between the vectors.
    ///
    Real distance(const Vec3& other) const
    {
        return std::sqrt(static_cast<Real>((x - other.x) * (x - other.x)
            + (y - other.y) * (y - other.y) + (z - other.z) * (z - other.z)));
    }

    /// @brief Calculate the dot product of two vectors.
    ///
    /// @param other The vector to dot with.
    /// @return T The dot product of the vectors.
    ///
    T dot(const Vec3& other) const
    {
        return (x * other.x) + (y * other.y) + (z * other.z);
    }

    /// @brief Calculate the cross product of two vectors.
    ///
    /// @param other The vector to cross with.
    /// @return Vec3 The cross product of the vectors.
    ///
    Vec3 cross(const Vec3& other) const
    {
        return Vec3((y * other.z) - (z * other.y), (z * other.x) - (x * other.z),
            (x * other.y) - (y * other.x));
    }

    /// @brief Calculate the magnitude of the vector.
    ///
    /// @return The magnitude of the vector.
    ///
    Real magnitude() const
    {
        return std::sqrt(static_cast<Real>((x * x) + (y * y) + (z * z)));
    }

    /// @brief Addition operator for the Vec3 class.
    ///
    /// @param other The vector to add.
    /// @return Vec3 The sum of the vectors.
    ///
    Vec3 operator+(const Vec3& other) const
    {
        return Vec3(x + other.x, y + other.y, z + other.z);
    }

    /// @brief Subtraction operator for the Vec3 class.
    ///
    /// @param other The vector to subtract.
    /// @return Vec3 The difference of the vectors.
    ///
    Vec3 operator-(const Vec3& other) const
    {
        return Vec3(x - other.x, y - other.y, z - other.z);
    }

    /// @brief Multiplication operator for the Vec3 class.
    ///
    /// @param scalar The scalar to multiply by.
    /// @return Vec3 The scaled vector.
    ///
    Vec3 operator*(const T scalar) const
    {
        return Vec3(x * scalar, y * scalar, z * scalar);
    }

    /// @brief Division operator for the Vec3 class.
    ///
    /// @param scalar The scalar to divide by.
    /// @return Vec3 The scaled vector.
    ///
    Vec3 operator/(const T scalar) const
    {
        return Vec3(x / scalar, y / scalar, z / scalar);
    }

    /// @brief Modulo the Vec3 object by a scalar
    ///
    /// @param scalar The scalar to modulo by
    /// @return Vec3 The result of the modulo
    ///
    Vec3 operator%(const T scalar) const
    {
        return Vec3(x % scalar, y % scalar, z % scalar);
    }

    /// @brief Addition assignment operator for the Vec3 class.
    ///
    /// @param other The vector to add.
    /// @return Vec3& The sum of the vectors.
    ///
    Vec3& operator+=(const Vec3& other)
    {
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
    Vec3& operator-=(const Vec3& other)
    {
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
    Vec3& operator*=(const T scalar)
    {
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
    Vec3& operator/=(const T scalar)
    {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }

    /// @brief Modulo assignment operator for the Vec3 class.
    ///
    /// @param scalar The scalar to modulo by.
    /// @return Vec3& The result of the modulo operation.
    ///
    Vec3& operator%=(const T scalar)
    {
        x %= scalar;
        y %= scalar;
        z %= scalar;
        return *this;
    }

    /// @brief Equality operator for the Vec3 class.
    ///
    /// @param other The vector to compare.
    /// @return bool True if the vectors are equal.
    ///
    bool operator==(const Vec3& other) const = default;

    /// @brief The x component of the vector.
    T x { 0 };

    /// @brief The y component of the vector.
    T y { 0 };

    /// @brief The z component of the vector.
    T z { 0 };
};

/// @brief A 2D rectangle in space.
///
/// @details This class represents a 2D rectangle in space. It is used to
/// represent areas, sizes, and positions.
///
template <typename T> class Quad {
public:
    /// @brief Default constructor for the Quad class.
    ///
    Quad()
        : position(0, 0)
        , size(0, 0)
    {
    }

    /// @brief Constructor for the Quad class.
    ///
    /// @param position The position of the rectangle.
    /// @param size The size of the rectangle.
    ///
    Quad(const Vec2<T>& position, const Vec2<T>& size)
        : position(position)
        , size(size)
    {
    }

    /// @brief Constructor for the Quad class.
    ///
    /// @param x The x position of the rectangle.
    /// @param y The y position of the rectangle.
    /// @param width The width of the rectangle.
    /// @param height The height of the rectangle.
    ///
    Quad(T x, T y, T width, T height)
        : position(x, y)
        , size(width, height)
    {
    }

    /// @brief Set the position of the rectangle.
    ///
    /// @param x The x position of the rectangle.
    /// @param y The y position of the rectangle.
    ///
    void set_position(T x, T y)
    {
        position = Vec2<T>(x, y);
    }

    /// @brief Set the size of the rectangle.
    ///
    /// @param width The width of the rectangle.
    /// @param height The height of the rectangle.
    ///
    void set_size(T width, T height)
    {
        size = Vec2<T>(width, height);
    }

    /// @brief Get center of the rectangle.
    ///
    /// @return Vec2 The center of the rectangle.
    ///
    Vec2<T> get_center() const
    {
        return Vec2<T>(position.x + (size.x / 2.0F), position.y + (size.y / 2.0F));
    }

    /// @brief Check if a point is inside the rectangle. The left and top edges
    /// are inside, the right and bottom edges are not, as for collides, so
    /// rectangles that share an edge never both contain a point.
    ///
    /// @param point The point to check.
    /// @return bool True if the point is inside the rectangle.
    ///
    bool contains(const Vec2<T>& point) const
    {
        return contains(point.x, point.y);
    }

    /// @brief Check if coordinates are inside the rectangle. The left and top
    /// edges are inside, the right and bottom edges are not, as for collides.
    ///
    /// @param x The x coordinate to check.
    /// @param y The y coordinate to check.
    /// @return bool True if the point is inside the rectangle.
    ///
    bool contains(T x, T y) const
    {
        return x >= position.x && x < position.x + size.x && y >= position.y
            && y < position.y + size.y;
    }

    /// @brief Check if a rectangle is inside the rectangle.
    ///
    /// @param other The rectangle to check.
    /// @return bool True if the rectangle is inside the rectangle.
    ///
    bool collides(const Quad& other) const
    {
        bool is_outside = position.x + size.x <= other.position.x || // a is left of b
            other.position.x + other.size.x <= position.x || // b is left of a
            position.y + size.y <= other.position.y || // a is above b
            other.position.y + other.size.y <= position.y; // b is above a

        return !is_outside;
    }

    /// @brief Get the point in or on the rectangle closest to another point.
    ///
    /// @param point The point to check.
    /// @return Vec2 The closest point. This is the point itself when it is
    /// inside the rectangle.
    ///
    Vec2<T> closest_point(const Vec2<T>& point) const
    {
        // The size can be negative, clamp between the edges in either order
        const T x1 = position.x + size.x;
        const T y1 = position.y + size.y;
        return Vec2<T>(std::clamp(point.x, std::min(position.x, x1), std::max(position.x, x1)),
            std::clamp(point.y, std::min(position.y, y1), std::max(position.y, y1)));
    }

    /// @brief Get the distance from a point to the edge of the rectangle.
    ///
    /// @param point The point to check.
    /// @return The distance, 0 when the point is inside the rectangle.
    ///
    typename Vec2<T>::Real distance_to(const Vec2<T>& point) const
    {
        return closest_point(point).distance(point);
    }

    /// @brief Get the smallest move that pushes this rectangle out of another.
    /// Useful to stop a moving box from entering solid scenery.
    ///
    /// @param other The rectangle to push out of.
    /// @return Vec2 The move to add to this rectangle's position, along
    /// whichever axis has the smallest overlap. A zero vector when the
    /// rectangles do not overlap.
    ///
    Vec2<T> get_push_out(const Quad& other) const
    {
        if (!collides(other)) {
            return Vec2<T>(T(0), T(0));
        }

        const T left = (position.x + size.x) - other.position.x;
        const T right = (other.position.x + other.size.x) - position.x;
        const T up = (position.y + size.y) - other.position.y;
        const T down = (other.position.y + other.size.y) - position.y;

        const T push_x = left < right ? -left : right;
        const T push_y = up < down ? -up : down;

        if (std::min(left, right) < std::min(up, down)) {
            return Vec2<T>(push_x, T(0));
        }

        return Vec2<T>(T(0), push_y);
    }

    // Collision
    bool collides_bottom(const Quad& other) const
    {
        return position.y < other.position.y + other.size.y
            && position.y + size.y > other.position.y + other.size.y;
    }

    bool collides_top(const Quad& other) const
    {
        return position.y + size.y > other.position.y && position.y < other.position.y;
    }

    bool collides_left(const Quad& other) const
    {
        return position.x + size.x > other.position.x && position.x < other.position.x;
    }

    bool collides_right(const Quad& other) const
    {
        return position.x < other.position.x + other.size.x
            && position.x + size.x > other.position.x + other.size.x;
    }

    /// @brief Add another rectangle's position and size to this one.
    ///
    /// @param quad The rectangle to add.
    /// @return Quad The rectangle with both position and size added.
    ///
    Quad operator+(const Quad<T>& quad) const
    {
        return Quad(position + quad.position, size + quad.size);
    }

    /// @brief Subtract another rectangle's position and size from this one.
    ///
    /// @param quad The rectangle to subtract.
    /// @return Quad The rectangle with both position and size subtracted.
    ///
    Quad operator-(const Quad<T>& quad) const
    {
        return Quad(position - quad.position, size - quad.size);
    }

    /// @brief Multiply the rectangle by a scalar.
    ///
    /// @param scalar The scalar to multiply by.
    /// @return Quad The rectangle multiplied by the scalar.
    ///
    Quad operator*(const T scalar) const
    {
        return Quad(position * scalar, size * scalar);
    }

    /// @brief Divide the rectangle by a scalar.
    ///
    /// @param scalar The scalar to divide by.
    /// @return Quad The rectangle divided by the scalar.
    ///
    Quad operator/(const T scalar) const
    {
        return Quad(position / scalar, size / scalar);
    }

    /// @brief The position of the rectangle.
    Vec2<T> position;

    /// @brief The size of the rectangle.
    Vec2<T> size;
};

/// Type aliases for common vector and rectangle types.
using Vec2f = Vec2<float>;
using Vec2i = Vec2<int>;
using Vec3f = Vec3<float>;
using Vec3i = Vec3<int>;
using Quadf = Quad<float>;
using Quadi = Quad<int>;

/// @brief A polygon, as its corners in order. The last corner joins the first.
template <typename T> using Polygon = std::vector<Vec2<T>>;
using Polygonf = Polygon<float>;

namespace geometry {

    /// @brief Get the signed area of a polygon.
    ///
    /// @param polygon The polygon.
    /// @return The area. It is positive when the corners go clockwise on screen
    /// and negative when they go anticlockwise.
    ///
    template <typename T> typename Vec2<T>::Real signed_area(const Polygon<T>& polygon)
    {
        using Real = typename Vec2<T>::Real;
        Real area = 0;
        for (std::size_t i = 0; i < polygon.size(); ++i) {
            area += static_cast<Real>(polygon[i].cross(polygon[(i + 1) % polygon.size()]));
        }
        return area / Real(2);
    }

    /// @brief Get the smallest rectangle that holds every corner of a polygon.
    ///
    /// @param polygon The polygon.
    /// @return The bounds, or an empty rectangle at 0, 0 for no corners.
    ///
    template <typename T> Quad<T> bounds(const Polygon<T>& polygon)
    {
        if (polygon.empty()) {
            return { };
        }

        Vec2<T> lo = polygon.front();
        Vec2<T> hi = polygon.front();
        for (const auto& p : polygon) {
            lo = { std::min(lo.x, p.x), std::min(lo.y, p.y) };
            hi = { std::max(hi.x, p.x), std::max(hi.y, p.y) };
        }
        return { lo, hi - lo };
    }

    /// @brief Check if a point is inside a triangle. Points on an edge are inside.
    ///
    /// @param point The point to check.
    /// @param a The first corner.
    /// @param b The second corner.
    /// @param c The third corner.
    /// @return true if the point is inside or on the edge.
    ///
    template <typename T>
    bool point_in_triangle(
        const Vec2<T>& point, const Vec2<T>& a, const Vec2<T>& b, const Vec2<T>& c)
    {
        const T d1 = (b - a).cross(point - a);
        const T d2 = (c - b).cross(point - b);
        const T d3 = (a - c).cross(point - c);
        const bool has_neg = d1 < 0 || d2 < 0 || d3 < 0;
        const bool has_pos = d1 > 0 || d2 > 0 || d3 > 0;
        return !(has_neg && has_pos);
    }

    /// @brief Find where a ray hits a line segment.
    ///
    /// @param origin Where the ray starts.
    /// @param direction Which way the ray goes. It does not need to be normalized.
    /// @param a One end of the segment.
    /// @param b The other end of the segment.
    /// @return How far along the ray the hit is, in lengths of @p direction, or
    /// nothing when the ray misses or runs along the segment.
    ///
    std::optional<float> ray_hit(
        const Vec2f& origin, const Vec2f& direction, const Vec2f& a, const Vec2f& b);

    /// @brief Find the area that can be seen from a point.
    ///
    /// @details Useful for field of view and for light. Rays stop at the first
    /// polygon edge, and at a square of half size @p radius around @p from.
    ///
    /// @param from The point to look from.
    /// @param radius How far to look.
    /// @param occluders Polygons that block the view.
    /// @param direction Direction to look, in radians, when @p cone is set.
    /// @param cone Width of the view in radians. 0 looks all round.
    /// @return The edge of the visible area in angle order, not including @p from.
    ///
    Polygonf visibility(const Vec2f& from, float radius, const std::vector<Polygonf>& occluders,
        float direction = 0.0F, float cone = 0.0F);

    /// @brief Find the area that can be seen from a point, into a polygon you keep
    /// between calls so it does not allocate each time.
    ///
    /// @param result Replaced with the edge of the visible area.
    ///
    void visibility(Polygonf& result, const Vec2f& from, float radius,
        const std::vector<Polygonf>& occluders, float direction = 0.0F, float cone = 0.0F);

} // namespace geometry

} // namespace asw

#endif // ASW_GEOMETRY_H