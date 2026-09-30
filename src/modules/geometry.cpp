#include "./asw/modules/geometry.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <numbers>
#include <utility>

namespace {
constexpr float TAU = std::numbers::pi_v<float> * 2.0F;

// Small turn either side of each corner, so rays go past it
constexpr float CORNER_NUDGE = 0.0001F;

using asw::Vec2f;

struct Ray {
    // Angle relative to the view direction, used to sort the rays
    float angle;
    Vec2f dir;
};

Vec2f rotate(const Vec2f& v, float cos_a, float sin_a)
{
    return { (v.x * cos_a) - (v.y * sin_a), (v.x * sin_a) + (v.y * cos_a) };
}
} // namespace

std::optional<float> asw::geometry::ray_hit(
    const Vec2f& origin, const Vec2f& direction, const Vec2f& a, const Vec2f& b)
{
    const Vec2f edge = b - a;
    const float denom = direction.cross(edge);
    if (std::abs(denom) < 1e-8F) {
        return std::nullopt;
    }

    const Vec2f to_a = a - origin;
    const float t = to_a.cross(edge) / denom;
    const float u = to_a.cross(direction) / denom;
    if (t < 0.0F || u < 0.0F || u > 1.0F) {
        return std::nullopt;
    }
    return t;
}

asw::Polygonf asw::geometry::visibility(const Vec2f& from, float radius,
    const std::vector<Polygonf>& occluders, float direction, float cone)
{
    Polygonf result;
    visibility(result, from, radius, occluders, direction, cone);
    return result;
}

void asw::geometry::visibility(Polygonf& result, const Vec2f& from, float radius,
    const std::vector<Polygonf>& occluders, float direction, float cone)
{
    result.clear();
    if (!(radius > 0.0F)) {
        return;
    }

    thread_local std::vector<std::pair<Vec2f, Vec2f>> segments;
    thread_local std::vector<Ray> rays;
    segments.clear();
    rays.clear();

    // The edge of the square the view reaches
    const std::array<Vec2f, 4> square { {
        { from.x - radius, from.y - radius },
        { from.x + radius, from.y - radius },
        { from.x + radius, from.y + radius },
        { from.x - radius, from.y + radius },
    } };
    for (std::size_t i = 0; i < square.size(); ++i) {
        segments.emplace_back(square[i], square[(i + 1) % square.size()]);
    }

    const asw::Quadf reach(square[0], Vec2f(radius * 2.0F, radius * 2.0F));
    for (const auto& polygon : occluders) {
        if (polygon.size() < 2 || !bounds(polygon).collides(reach)) {
            continue;
        }
        for (std::size_t i = 0; i < polygon.size(); ++i) {
            segments.emplace_back(polygon[i], polygon[(i + 1) % polygon.size()]);
        }
    }

    const bool spot = cone > 0.0F && cone < TAU;
    const float half_cone = cone / 2.0F;
    auto add_ray = [&](float angle, const Vec2f& dir) {
        if (!spot || std::abs(angle) <= half_cone) {
            rays.push_back({ angle, dir });
        }
    };

    // Every corner starts exactly one segment, so the first end of each
    // segment gives each corner once. Aim at it and just either side of it
    const float nudge_cos = std::cos(CORNER_NUDGE);
    const float nudge_sin = std::sin(CORNER_NUDGE);
    for (const auto& segment : segments) {
        const Vec2f to = segment.first - from;
        const float length = to.magnitude();
        if (length <= 0.0F) {
            continue;
        }

        const Vec2f dir = to / length;
        const float angle = std::remainder(std::atan2(dir.y, dir.x) - direction, TAU);
        add_ray(angle - CORNER_NUDGE, rotate(dir, nudge_cos, -nudge_sin));
        add_ray(angle, dir);
        add_ray(angle + CORNER_NUDGE, rotate(dir, nudge_cos, nudge_sin));
    }

    if (spot) {
        for (const float edge : { -half_cone, half_cone }) {
            add_ray(edge, Vec2f(std::cos(direction + edge), std::sin(direction + edge)));
        }
    }

    std::ranges::sort(rays, { }, &Ray::angle);

    result.reserve(rays.size());
    for (const auto& ray : rays) {
        float nearest = std::numeric_limits<float>::max();
        for (const auto& [a, b] : segments) {
            if (const auto t = ray_hit(from, ray.dir, a, b)) {
                nearest = std::min(nearest, *t);
            }
        }

        if (nearest < std::numeric_limits<float>::max()) {
            result.push_back(from + (ray.dir * nearest));
        }
    }
}
