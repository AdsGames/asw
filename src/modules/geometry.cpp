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

struct Segment {
    Vec2f a;
    Vec2f b;

    // Distance from the view point to the closest point of the segment. No
    // ray can hit the segment closer than this.
    float distance;
};

Vec2f rotate(const Vec2f& v, float cos_a, float sin_a)
{
    return { (v.x * cos_a) - (v.y * sin_a), (v.x * sin_a) + (v.y * cos_a) };
}

float distance_to_segment(const Vec2f& p, const Vec2f& a, const Vec2f& b)
{
    const Vec2f edge = b - a;
    const float length_sq = edge.dot(edge);
    const float t = length_sq > 0.0F ? std::clamp((p - a).dot(edge) / length_sq, 0.0F, 1.0F) : 0.0F;
    return (p - (a + (edge * t))).magnitude();
}

// Clip a segment to an axis aligned box (Liang-Barsky). Returns false when
// no part of it is inside.
bool clip_segment(Vec2f& a, Vec2f& b, const Vec2f& min, const Vec2f& max)
{
    const Vec2f d = b - a;
    float t0 = 0.0F;
    float t1 = 1.0F;

    const std::array<std::pair<float, float>, 4> edges { {
        { -d.x, a.x - min.x },
        { d.x, max.x - a.x },
        { -d.y, a.y - min.y },
        { d.y, max.y - a.y },
    } };
    for (const auto& [p, q] : edges) {
        if (p == 0.0F) {
            if (q < 0.0F) {
                return false;
            }
            continue;
        }

        const float r = q / p;
        if (p < 0.0F) {
            t0 = std::max(t0, r);
        } else {
            t1 = std::min(t1, r);
        }
        if (t0 > t1) {
            return false;
        }
    }

    const Vec2f start = a;
    a = start + (d * t0);
    b = start + (d * t1);
    return true;
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
    thread_local std::vector<Quadf> occluder_bounds;
    occluder_bounds.clear();
    for (const auto& polygon : occluders) {
        occluder_bounds.push_back(bounds(polygon));
    }
    visibility(result, from, radius, occluders, occluder_bounds, direction, cone);
}

void asw::geometry::visibility(Polygonf& result, const Vec2f& from, float radius,
    const std::vector<Polygonf>& occluders, const std::vector<Quadf>& occluder_bounds,
    float direction, float cone)
{
    result.clear();
    if (!(radius > 0.0F)) {
        return;
    }

    thread_local std::vector<Segment> segments;
    thread_local std::vector<Vec2f> corners;
    thread_local std::vector<Ray> rays;
    segments.clear();
    corners.clear();
    rays.clear();

    // The edge of the square the view reaches
    const std::array<Vec2f, 4> square { {
        { from.x - radius, from.y - radius },
        { from.x + radius, from.y - radius },
        { from.x + radius, from.y + radius },
        { from.x - radius, from.y + radius },
    } };
    for (std::size_t i = 0; i < square.size(); ++i) {
        const auto& a = square[i];
        const auto& b = square[(i + 1) % square.size()];
        segments.push_back({ a, b, distance_to_segment(from, a, b) });
        corners.push_back(a);
    }

    // Edges are clipped to the square. Where an edge crosses the square is a
    // corner of the visible area too, so it gets rays like any other corner.
    const asw::Quadf reach(square[0], Vec2f(radius * 2.0F, radius * 2.0F));
    for (std::size_t p = 0; p < occluders.size(); ++p) {
        const auto& polygon = occluders[p];
        if (polygon.size() < 2 || p >= occluder_bounds.size()
            || !occluder_bounds[p].collides(reach)) {
            continue;
        }
        for (std::size_t i = 0; i < polygon.size(); ++i) {
            Vec2f a = polygon[i];
            Vec2f b = polygon[(i + 1) % polygon.size()];
            const Vec2f original_b = b;
            if (!clip_segment(a, b, square[0], square[2])) {
                continue;
            }

            segments.push_back({ a, b, distance_to_segment(from, a, b) });

            // Each corner inside the square starts exactly one edge, so the
            // first end of each edge gives it once. A clipped second end
            // starts no edge, so add it here.
            corners.push_back(a);
            if (b != original_b) {
                corners.push_back(b);
            }
        }
    }

    // Nearest edges first, so each ray can stop once the rest are too far
    std::ranges::sort(segments, { }, &Segment::distance);

    const bool spot = cone > 0.0F && cone < TAU;
    const float half_cone = cone / 2.0F;
    auto add_ray = [&](float angle, const Vec2f& dir) {
        if (!spot || std::abs(angle) <= half_cone) {
            rays.push_back({ angle, dir });
        }
    };

    // Aim at each corner and just either side of it
    const float nudge_cos = std::cos(CORNER_NUDGE);
    const float nudge_sin = std::sin(CORNER_NUDGE);
    for (const auto& corner : corners) {
        const Vec2f to = corner - from;
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
        // Ray directions have length 1, so a hit's t is its distance
        float nearest = std::numeric_limits<float>::max();
        for (const auto& segment : segments) {
            if (segment.distance >= nearest) {
                break;
            }
            if (const auto t = ray_hit(from, ray.dir, segment.a, segment.b)) {
                nearest = std::min(nearest, *t);
            }
        }

        if (nearest < std::numeric_limits<float>::max()) {
            result.push_back(from + (ray.dir * nearest));
        }
    }
}
