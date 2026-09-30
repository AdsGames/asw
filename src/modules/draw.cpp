#include "./asw/modules/draw.h"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <numbers>
#include <unordered_map>
#include <vector>

#include "./asw/modules/display.h"
#include "./asw/modules/util.h"

namespace {
struct TextCacheKey {
    asw::Renderer* renderer;
    asw::Font font;
    std::string text;
    uint32_t color;

    // Changes when the font's size, style or hinting change
    uint32_t font_generation;

    // Output pixels per logical pixel the text was rendered for, in hundredths
    uint32_t render_scale;

    bool operator==(const TextCacheKey&) const = default;
};

struct TextCacheEntry {
    asw::Texture texture;

    // Size in logical pixels
    float width { 0.0F };
    float height { 0.0F };
};

struct TextCacheKeyHash {
    std::size_t operator()(const TextCacheKey& key) const
    {
        std::size_t seed = std::hash<asw::Renderer*> { }(key.renderer);
        seed ^= std::hash<asw::Font> { }(key.font) + 0x9e3779b9 + ((seed << 6) + (seed >> 2));
        seed ^= std::hash<std::string> { }(key.text) + 0x9e3779b9 + ((seed << 6) + (seed >> 2));
        seed ^= std::hash<uint32_t> { }(key.color) + 0x9e3779b9 + ((seed << 6) + (seed >> 2));
        seed ^= std::hash<uint32_t> { }(key.font_generation) + 0x9e3779b9
            + ((seed << 6) + (seed >> 2));
        seed
            ^= std::hash<uint32_t> { }(key.render_scale) + 0x9e3779b9 + ((seed << 6) + (seed >> 2));
        return seed;
    }
};

// A font resized to render text for a larger output
struct ScaledFontKey {
    asw::Font font;
    uint32_t font_generation;
    uint32_t render_scale;

    bool operator==(const ScaledFontKey&) const = default;
};

struct ScaledFontKeyHash {
    std::size_t operator()(const ScaledFontKey& key) const
    {
        std::size_t seed = std::hash<asw::Font> { }(key.font);
        seed ^= std::hash<uint32_t> { }(key.font_generation) + 0x9e3779b9
            + ((seed << 6) + (seed >> 2));
        seed
            ^= std::hash<uint32_t> { }(key.render_scale) + 0x9e3779b9 + ((seed << 6) + (seed >> 2));
        return seed;
    }
};

// Scale is stored in hundredths, so 100 is the logical size
constexpr uint32_t RENDER_SCALE_ONE = 100;

constexpr std::size_t SCALED_FONT_LIMIT = 32;
std::unordered_map<ScaledFontKey, asw::Font, ScaledFontKeyHash> scaled_fonts;

constexpr std::size_t TEXT_CACHE_LIMIT = 256;
std::unordered_map<TextCacheKey, TextCacheEntry, TextCacheKeyHash> text_cache;

uint32_t pack_color(const asw::Color color)
{
    return (static_cast<uint32_t>(color.r) << 24U) | (static_cast<uint32_t>(color.g) << 16U)
        | (static_cast<uint32_t>(color.b) << 8U) | static_cast<uint32_t>(color.a);
}

// Output pixels per logical pixel to render text at, in hundredths. Smooth
// text is rendered at the output resolution so it stays sharp when the window
// is scaled or on high density displays. Pixel fonts keep their hard pixels
// at the logical size, and text drawn into a texture is not scaled.
uint32_t text_render_scale(asw::Renderer* renderer, bool pixel_font)
{
    if (pixel_font || SDL_GetRenderTarget(renderer) != nullptr) {
        return RENDER_SCALE_ONE;
    }

    const float scale = asw::display::get_scale().x;
    if (scale <= 1.0F) {
        return RENDER_SCALE_ONE;
    }

    return static_cast<uint32_t>(std::lround(scale * static_cast<float>(RENDER_SCALE_ONE)));
}

// Copy of a font at render_scale times its size. The copy keeps the style and
// hinting, and leaves the caller's font untouched.
TTF_Font* get_scaled_font(const asw::Font& font, uint32_t font_generation, uint32_t render_scale)
{
    ScaledFontKey key { font, font_generation, render_scale };
    if (auto it = scaled_fonts.find(key); it != scaled_fonts.end()) {
        return it->second.get();
    }

    TTF_Font* copy = TTF_CopyFont(font.get());
    if (copy == nullptr) {
        return nullptr;
    }

    const float scale = static_cast<float>(render_scale) / static_cast<float>(RENDER_SCALE_ONE);
    if (!TTF_SetFontSize(copy, TTF_GetFontSize(font.get()) * scale)) {
        TTF_CloseFont(copy);
        return nullptr;
    }

    if (scaled_fonts.size() >= SCALED_FONT_LIMIT) {
        scaled_fonts.clear();
    }

    asw::Font scaled { copy, [](TTF_Font* f) {
                          if (TTF_WasInit() > 0) {
                              TTF_CloseFont(f);
                          }
                      } };

    return scaled_fonts.try_emplace(std::move(key), std::move(scaled)).first->second.get();
}

asw::Texture make_cached_texture(SDL_Texture* texture)
{
    // Cached text is explicitly cleared before renderer teardown in the normal
    // shutdown path, mirroring the renderer-guarded asset deleters.
    return { texture, [](SDL_Texture* t) {
                if (asw::display::get_renderer() != nullptr) {
                    SDL_DestroyTexture(t);
                }
            } };
}
} // namespace

void asw::draw::clear_color(asw::Color color)
{
    auto* r = asw::display::get_renderer();
    if (r == nullptr) {
        return;
    }

    SDL_SetRenderDrawColor(r, color.r, color.g, color.b, color.a);
    SDL_RenderClear(r);
}

void asw::draw::sprite(const asw::Texture& tex, const asw::Vec2<float>& position)
{
    auto* r = asw::display::get_renderer();
    if (r == nullptr) {
        return;
    }

    auto size = asw::util::get_texture_size(tex);

    SDL_FRect dest;
    dest.x = position.x;
    dest.y = position.y;
    dest.w = size.x;
    dest.h = size.y;

    SDL_RenderTexture(r, tex.get(), nullptr, &dest);
}

void asw::draw::sprite_flip(
    const asw::Texture& tex, const asw::Vec2<float>& position, bool flip_x, bool flip_y)
{
    auto* r = asw::display::get_renderer();
    if (r == nullptr) {
        return;
    }

    auto size = asw::util::get_texture_size(tex);

    SDL_FRect dest;
    dest.x = position.x;
    dest.y = position.y;
    dest.w = size.x;
    dest.h = size.y;

    SDL_FlipMode flip = SDL_FLIP_NONE;

    if (flip_x) {
        flip = static_cast<SDL_FlipMode>(flip | SDL_FLIP_HORIZONTAL);
    }

    if (flip_y) {
        flip = static_cast<SDL_FlipMode>(flip | SDL_FLIP_VERTICAL);
    }

    SDL_RenderTextureRotated(r, tex.get(), nullptr, &dest, 0, nullptr, flip);
}

void asw::draw::stretch_sprite(const asw::Texture& tex, const asw::Quad<float>& position)
{
    auto* r = asw::display::get_renderer();
    if (r == nullptr) {
        return;
    }

    SDL_FRect dest;
    dest.x = position.position.x;
    dest.y = position.position.y;
    dest.w = position.size.x;
    dest.h = position.size.y;

    SDL_RenderTexture(r, tex.get(), nullptr, &dest);
}

void asw::draw::rotate_sprite(
    const asw::Texture& tex, const asw::Vec2<float>& position, float angle)
{
    auto* r = asw::display::get_renderer();
    if (r == nullptr) {
        return;
    }

    auto size = asw::util::get_texture_size(tex);

    SDL_FRect dest;
    dest.x = position.x;
    dest.y = position.y;
    dest.w = size.x;
    dest.h = size.y;

    // Rad to deg
    const double angleDeg = angle * (180.0 / std::numbers::pi);

    SDL_RenderTextureRotated(r, tex.get(), nullptr, &dest, angleDeg, nullptr, SDL_FLIP_NONE);
}

void asw::draw::stretch_sprite_rotate(
    const asw::Texture& tex, const asw::Quad<float>& dest, float angle, bool flip_x, bool flip_y)
{
    auto* r = asw::display::get_renderer();
    if (r == nullptr) {
        return;
    }

    SDL_FRect r_dest;
    r_dest.x = dest.position.x;
    r_dest.y = dest.position.y;
    r_dest.w = dest.size.x;
    r_dest.h = dest.size.y;

    // Rad to deg
    const double angleDeg = angle * (180.0 / std::numbers::pi);

    SDL_FlipMode flip = SDL_FLIP_NONE;

    if (flip_x) {
        flip = static_cast<SDL_FlipMode>(flip | SDL_FLIP_HORIZONTAL);
    }

    if (flip_y) {
        flip = static_cast<SDL_FlipMode>(flip | SDL_FLIP_VERTICAL);
    }

    SDL_RenderTextureRotated(r, tex.get(), nullptr, &r_dest, angleDeg, nullptr, flip);
}

void asw::draw::stretch_sprite_blit(
    const asw::Texture& tex, const asw::Quad<float>& source, const asw::Quad<float>& dest)
{
    auto* r = asw::display::get_renderer();
    if (r == nullptr) {
        return;
    }

    SDL_FRect r_src;
    r_src.x = source.position.x;
    r_src.y = source.position.y;
    r_src.w = source.size.x;
    r_src.h = source.size.y;

    SDL_FRect r_dest;
    r_dest.x = dest.position.x;
    r_dest.y = dest.position.y;
    r_dest.w = dest.size.x;
    r_dest.h = dest.size.y;

    SDL_RenderTexture(r, tex.get(), &r_src, &r_dest);
}

void asw::draw::stretch_sprite_rotate_blit(const asw::Texture& tex, const asw::Quad<float>& source,
    const asw::Quad<float>& dest, float angle, bool flip_x, bool flip_y)
{
    auto* r = asw::display::get_renderer();
    if (r == nullptr) {
        return;
    }

    SDL_FRect r_src;
    r_src.x = source.position.x;
    r_src.y = source.position.y;
    r_src.w = source.size.x;
    r_src.h = source.size.y;

    SDL_FRect r_dest;
    r_dest.x = dest.position.x;
    r_dest.y = dest.position.y;
    r_dest.w = dest.size.x;
    r_dest.h = dest.size.y;

    const double angleDeg = angle * (180.0 / std::numbers::pi);

    int flip = SDL_FLIP_NONE;
    if (flip_x) {
        flip |= SDL_FLIP_HORIZONTAL;
    }
    if (flip_y) {
        flip |= SDL_FLIP_VERTICAL;
    }

    SDL_RenderTextureRotated(
        r, tex.get(), &r_src, &r_dest, angleDeg, nullptr, static_cast<SDL_FlipMode>(flip));
}

void asw::draw::text(const asw::Font& font, const std::string& text,
    const asw::Vec2<float>& position, asw::Color color, asw::TextJustify justify)
{
    auto* r = asw::display::get_renderer();
    if (text.empty() || font == nullptr || r == nullptr) {
        return;
    }

    // Pixel fonts (mono hinting) render without anti-aliasing. Blended
    // output can still hold partial alpha, solid output never does.
    const bool pixel_font = TTF_GetFontHinting(font.get()) == TTF_HINTING_MONO;
    const uint32_t font_generation = TTF_GetFontGeneration(font.get());
    const uint32_t render_scale = text_render_scale(r, pixel_font);

    // Alpha is applied when drawing, so fading text reuses one cached texture
    const auto opaque = asw::Color(color.r, color.g, color.b, 255);
    TextCacheKey cache_key { r, font, text, pack_color(opaque), font_generation, render_scale };
    auto cached_text = text_cache.find(cache_key);
    if (cached_text == text_cache.end()) {
        TTF_Font* render_font = font.get();
        if (render_scale != RENDER_SCALE_ONE) {
            if (auto* scaled = get_scaled_font(font, font_generation, render_scale);
                scaled != nullptr) {
                render_font = scaled;
            } else {
                // Could not resize, fall back to scaling the logical size text
                cache_key.render_scale = RENDER_SCALE_ONE;
            }
        }

        const float scale
            = static_cast<float>(cache_key.render_scale) / static_cast<float>(RENDER_SCALE_ONE);

        const auto sdlColor = SDL_Color { color.r, color.g, color.b, 255 };
        SDL_Surface* textSurface = pixel_font
            ? TTF_RenderText_Solid(render_font, text.c_str(), 0, sdlColor)
            : TTF_RenderText_Blended(render_font, text.c_str(), 0, sdlColor);

        if (textSurface == nullptr) {
            return;
        }

        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(r, textSurface);
        if (textTexture == nullptr) {
            SDL_DestroySurface(textSurface);
            return;
        }

        SDL_SetTextureBlendMode(textTexture, SDL_BLENDMODE_BLEND);

        if (pixel_font) {
            SDL_SetTextureScaleMode(textTexture, SDL_SCALEMODE_NEAREST);
        } else {
            SDL_SetTextureScaleMode(textTexture, SDL_SCALEMODE_LINEAR);
        }

        // Drawn at its logical size, so the extra pixels fill the output.
        // Glyphs advance slightly differently at the larger size, so use the
        // logical font's measurements to keep layout the same as
        // util::get_text_size.
        float width = static_cast<float>(textSurface->w) / scale;
        float height = static_cast<float>(textSurface->h) / scale;
        if (render_font != font.get()) {
            int logical_w = 0;
            int logical_h = 0;
            if (TTF_GetStringSize(font.get(), text.c_str(), 0, &logical_w, &logical_h)) {
                width = static_cast<float>(logical_w);
                height = static_cast<float>(logical_h);
            }
        }

        TextCacheEntry entry {
            make_cached_texture(textTexture),
            width,
            height,
        };
        SDL_DestroySurface(textSurface);

        if (text_cache.size() >= TEXT_CACHE_LIMIT) {
            // Keep the cache bounded without introducing a heavier eviction
            // structure in this hot path.
            text_cache.clear();
        }

        cached_text = text_cache.try_emplace(std::move(cache_key), std::move(entry)).first;
    }

    SDL_FRect dest;
    dest.x = position.x;
    dest.y = position.y;
    dest.w = cached_text->second.width;
    dest.h = cached_text->second.height;

    // Justification settings
    if (justify == asw::TextJustify::Center) {
        dest.x -= dest.w / 2.0F;
    } else if (justify == asw::TextJustify::Right) {
        dest.x -= dest.w;
    }

    // Snap to whole pixels. Centred odd width text lands on a half pixel,
    // which blurs the glyphs.
    dest.x = std::round(dest.x);
    dest.y = std::round(dest.y);

    SDL_SetTextureAlphaMod(cached_text->second.texture.get(), color.a);
    SDL_RenderTexture(r, cached_text->second.texture.get(), nullptr, &dest);
}

void asw::draw::text_shadow(const asw::Font& font, const std::string& text,
    const asw::Vec2<float>& position, asw::Color color, asw::Color shadow,
    const asw::Vec2<float>& offset, asw::TextJustify justify)
{
    shadow.a = static_cast<uint8_t>(
        (static_cast<float>(shadow.a) * static_cast<float>(color.a)) / 255.0F);

    asw::draw::text(font, text, position + offset, shadow, justify);
    asw::draw::text(font, text, position, color, justify);
}

void asw::draw::clear_text_cache()
{
    text_cache.clear();
    scaled_fonts.clear();
}

void asw::draw::point(const asw::Vec2<float>& position, asw::Color color)
{
    auto* r = asw::display::get_renderer();
    if (r == nullptr) {
        return;
    }

    SDL_SetRenderDrawColor(r, color.r, color.g, color.b, color.a);
    SDL_RenderPoint(r, position.x, position.y);
}

void asw::draw::line(
    const asw::Vec2<float>& position1, const asw::Vec2<float>& position2, asw::Color color)
{
    auto* r = asw::display::get_renderer();
    if (r == nullptr) {
        return;
    }

    SDL_SetRenderDrawColor(r, color.r, color.g, color.b, color.a);
    SDL_RenderLine(r, position1.x, position1.y, position2.x, position2.y);
}

void asw::draw::rect(const asw::Quad<float>& position, asw::Color color, float thickness)
{
    auto* r = asw::display::get_renderer();
    if (r == nullptr) {
        return;
    }

    SDL_SetRenderDrawColor(r, color.r, color.g, color.b, color.a);

    const float x = position.position.x;
    const float y = position.position.y;
    const float w = position.size.x;
    const float h = position.size.y;

    if (thickness <= 1.0F) {
        const SDL_FRect rect { x, y, w, h };
        SDL_RenderRect(r, &rect);
        return;
    }

    // Four bands inside the quad, the sides fit between the top and bottom
    const float t = std::min({ thickness, w / 2.0F, h / 2.0F });
    const std::array<SDL_FRect, 4> bands { {
        { x, y, w, t },
        { x, y + h - t, w, t },
        { x, y + t, t, h - (t * 2.0F) },
        { x + w - t, y + t, t, h - (t * 2.0F) },
    } };

    SDL_RenderFillRects(r, bands.data(), static_cast<int>(bands.size()));
}

void asw::draw::rect_fill(const asw::Quad<float>& position, asw::Color color)
{
    auto* r = asw::display::get_renderer();
    if (r == nullptr) {
        return;
    }

    SDL_SetRenderDrawColor(r, color.r, color.g, color.b, color.a);
    SDL_FRect rect;
    rect.x = position.position.x;
    rect.y = position.position.y;
    rect.w = position.size.x;
    rect.h = position.size.y;

    SDL_RenderFillRect(r, &rect);
}

void asw::draw::rect_fill_rotate(const asw::Quad<float>& position, float angle, asw::Color color)
{
    auto* r = asw::display::get_renderer();
    if (r == nullptr) {
        return;
    }

    const auto center = position.get_center();
    const float half_w = position.size.x / 2.0F;
    const float half_h = position.size.y / 2.0F;
    const float cos_a = std::cos(angle);
    const float sin_a = std::sin(angle);

    // Screen y points down, so this turns clockwise on screen
    const std::array<SDL_FPoint, 4> corners { {
        { -half_w, -half_h },
        { half_w, -half_h },
        { half_w, half_h },
        { -half_w, half_h },
    } };

    const asw::FColor fcolor = color.to_fcolor();

    std::array<SDL_Vertex, 4> vertices { };
    for (std::size_t i = 0; i < corners.size(); i++) {
        vertices[i].position = { center.x + (corners[i].x * cos_a) - (corners[i].y * sin_a),
            center.y + (corners[i].x * sin_a) + (corners[i].y * cos_a) };
        vertices[i].color = fcolor;
    }

    constexpr std::array<int, 6> indices { 0, 1, 2, 0, 2, 3 };

    SDL_RenderGeometry(r, nullptr, vertices.data(), static_cast<int>(vertices.size()),
        indices.data(), static_cast<int>(indices.size()));
}

void asw::draw::circle(const asw::Vec2<float>& position, float radius, asw::Color color)
{
    auto* r = asw::display::get_renderer();
    if (r == nullptr) {
        return;
    }

    SDL_SetRenderDrawColor(r, color.r, color.g, color.b, color.a);

    // Midpoint circle algorithm — no trig, integer arithmetic only
    auto x = radius;
    auto y = 0.0F;
    auto err = 1.0F - x;
    const float cx = position.x;
    const float cy = position.y;

    while (x >= y) {
        SDL_RenderPoint(r, cx + x, cy + y);
        SDL_RenderPoint(r, cx - x, cy + y);
        SDL_RenderPoint(r, cx + x, cy - y);
        SDL_RenderPoint(r, cx - x, cy - y);
        SDL_RenderPoint(r, cx + y, cy + x);
        SDL_RenderPoint(r, cx - y, cy + x);
        SDL_RenderPoint(r, cx + y, cy - x);
        SDL_RenderPoint(r, cx - y, cy - x);
        y++;
        if (err < 0) {
            err += (2.0F * y) + 1.0F;
        } else {
            x--;
            err += (2.0F * (y - x)) + 1.0F;
        }
    }
}

void asw::draw::circle_fill(const asw::Vec2<float>& position, float radius, asw::Color color)
{
    auto* r = asw::display::get_renderer();
    if (r == nullptr) {
        return;
    }

    SDL_SetRenderDrawColor(r, color.r, color.g, color.b, color.a);

    // One span per pixel row, all sent in a single call. Snapping the centre
    // to a whole pixel keeps every row on the pixel grid, so a centre between
    // pixels leaves no gaps, and no row is drawn twice, so translucent circles
    // blend evenly.
    const float cx = std::round(position.x);
    const float cy = std::round(position.y);
    const int rows = static_cast<int>(radius);

    static std::vector<SDL_FRect> spans;
    spans.clear();

    for (int dy = -rows; dy <= rows; ++dy) {
        const auto fy = static_cast<float>(dy);
        const float half = std::floor(std::sqrt(std::max((radius * radius) - (fy * fy), 0.0F)));
        spans.push_back({ cx - half, cy + fy, (half * 2.0F) + 1.0F, 1.0F });
    }

    SDL_RenderFillRects(r, spans.data(), static_cast<int>(spans.size()));
}

void asw::draw::triangle_fill(const asw::Vec2<float>& a, const asw::Vec2<float>& b,
    const asw::Vec2<float>& c, asw::Color color)
{
    auto* r = asw::display::get_renderer();
    if (r == nullptr) {
        return;
    }

    const asw::FColor fcolor = color.to_fcolor();
    const std::array<SDL_Vertex, 3> vertices { {
        { { a.x, a.y }, fcolor, { 0.0F, 0.0F } },
        { { b.x, b.y }, fcolor, { 0.0F, 0.0F } },
        { { c.x, c.y }, fcolor, { 0.0F, 0.0F } },
    } };

    SDL_RenderGeometry(r, nullptr, vertices.data(), 3, nullptr, 0);
}

void asw::draw::polygon(const asw::Polygonf& points, asw::Color color)
{
    auto* r = asw::display::get_renderer();
    if (r == nullptr || points.size() < 2) {
        return;
    }

    static std::vector<SDL_FPoint> line_points;
    line_points.clear();
    for (const auto& p : points) {
        line_points.push_back({ p.x, p.y });
    }
    line_points.push_back({ points.front().x, points.front().y });

    SDL_SetRenderDrawColor(r, color.r, color.g, color.b, color.a);
    SDL_RenderLines(r, line_points.data(), static_cast<int>(line_points.size()));
}

void asw::draw::polygon_fill(const asw::Polygonf& points, asw::Color color)
{
    auto* r = asw::display::get_renderer();
    if (r == nullptr || points.size() < 3) {
        return;
    }

    // Ear clipping: cut off corners that bulge out and hold no other point
    // until one triangle is left
    const float winding = asw::geometry::signed_area(points) < 0.0F ? -1.0F : 1.0F;

    static std::vector<int> remaining;
    static std::vector<int> indices;
    remaining.clear();
    indices.clear();
    for (std::size_t i = 0; i < points.size(); ++i) {
        remaining.push_back(static_cast<int>(i));
    }

    while (remaining.size() > 3) {
        const std::size_t n = remaining.size();
        bool clipped = false;

        for (std::size_t i = 0; i < n; ++i) {
            const int prev = remaining[(i + n - 1) % n];
            const int cur = remaining[i];
            const int next = remaining[(i + 1) % n];
            const auto& a = points[static_cast<std::size_t>(prev)];
            const auto& b = points[static_cast<std::size_t>(cur)];
            const auto& c = points[static_cast<std::size_t>(next)];

            if ((b - a).cross(c - b) * winding <= 0.0F) {
                continue;
            }

            // A point on a corner of the ear, such as a repeated point, does
            // not block it
            const bool holds_point = std::ranges::any_of(remaining, [&](int other) {
                const auto& p = points[static_cast<std::size_t>(other)];
                return other != prev && other != cur && other != next && p != a && p != b && p != c
                    && asw::geometry::point_in_triangle(p, a, b, c);
            });
            if (holds_point) {
                continue;
            }

            indices.insert(indices.end(), { prev, cur, next });
            remaining.erase(remaining.begin() + static_cast<std::ptrdiff_t>(i));
            clipped = true;
            break;
        }

        // Crossing edges or repeated points leave no ear. Fill the rest as a
        // fan rather than drawing nothing
        if (!clipped) {
            for (std::size_t i = 1; i + 1 < remaining.size(); ++i) {
                indices.insert(indices.end(), { remaining[0], remaining[i], remaining[i + 1] });
            }
            remaining.clear();
        }
    }

    if (remaining.size() == 3) {
        indices.insert(indices.end(), { remaining[0], remaining[1], remaining[2] });
    }

    const asw::FColor fcolor = color.to_fcolor();
    static std::vector<SDL_Vertex> vertices;
    vertices.clear();
    for (const auto& p : points) {
        vertices.push_back({ { p.x, p.y }, fcolor, { 0.0F, 0.0F } });
    }

    SDL_RenderGeometry(r, nullptr, vertices.data(), static_cast<int>(vertices.size()),
        indices.data(), static_cast<int>(indices.size()));
}

void asw::draw::set_blend_mode(const asw::Texture& texture, asw::BlendMode mode)
{
    SDL_SetTextureBlendMode(texture.get(), static_cast<SDL_BlendMode>(mode));
}

void asw::draw::set_alpha(const asw::Texture& texture, float alpha)
{
    SDL_SetTextureAlphaModFloat(texture.get(), alpha);
}

void asw::draw::set_tint(const asw::Texture& texture, asw::Color tint)
{
    SDL_SetTextureColorMod(texture.get(), tint.r, tint.g, tint.b);
}

void asw::draw::set_scale_mode(const asw::Texture& texture, asw::ScaleMode mode)
{
    SDL_SetTextureScaleMode(texture.get(), static_cast<SDL_ScaleMode>(mode));
}
