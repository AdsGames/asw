#include "./asw/modules/draw.h"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <algorithm>
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

    bool operator==(const TextCacheKey&) const = default;
};

struct TextCacheEntry {
    asw::Texture texture;
    int width { 0 };
    int height { 0 };
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
        return seed;
    }
};

constexpr std::size_t TEXT_CACHE_LIMIT = 256;
std::unordered_map<TextCacheKey, TextCacheEntry, TextCacheKeyHash> text_cache;

uint32_t pack_color(const asw::Color color)
{
    return (static_cast<uint32_t>(color.r) << 24U) | (static_cast<uint32_t>(color.g) << 16U)
        | (static_cast<uint32_t>(color.b) << 8U) | static_cast<uint32_t>(color.a);
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
    const asw::Quad<float>& dest, float angle)
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

    SDL_RenderTextureRotated(r, tex.get(), &r_src, &r_dest, angleDeg, nullptr, SDL_FLIP_NONE);
}

void asw::draw::text(const asw::Font& font, const std::string& text,
    const asw::Vec2<float>& position, asw::Color color, asw::TextJustify justify)
{
    auto* r = asw::display::get_renderer();
    if (text.empty() || font == nullptr || r == nullptr) {
        return;
    }

    // Alpha is applied when drawing, so fading text reuses one cached texture
    const auto opaque = asw::Color(color.r, color.g, color.b, 255);
    TextCacheKey cache_key { r, font, text, pack_color(opaque),
        TTF_GetFontGeneration(font.get()) };
    auto cached_text = text_cache.find(cache_key);
    if (cached_text == text_cache.end()) {
        // Pixel fonts (mono hinting) render without anti-aliasing. Blended
        // output can still hold partial alpha, solid output never does.
        const bool pixel_font = TTF_GetFontHinting(font.get()) == TTF_HINTING_MONO;

        const auto sdlColor = SDL_Color { color.r, color.g, color.b, 255 };
        SDL_Surface* textSurface = pixel_font
            ? TTF_RenderText_Solid(font.get(), text.c_str(), 0, sdlColor)
            : TTF_RenderText_Blended(font.get(), text.c_str(), 0, sdlColor);

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

        TextCacheEntry entry {
            make_cached_texture(textTexture),
            textSurface->w,
            textSurface->h,
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
    dest.w = static_cast<float>(cached_text->second.width);
    dest.h = static_cast<float>(cached_text->second.height);

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

void asw::draw::clear_text_cache()
{
    text_cache.clear();
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

void asw::draw::rect(const asw::Quad<float>& position, asw::Color color)
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

    SDL_RenderRect(r, &rect);
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
