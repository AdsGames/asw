#include "./asw/modules/draw.h"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <cmath>
#include <numbers>

#include "./asw/modules/display.h"
#include "./asw/modules/util.h"

void asw::draw::clear_color(asw::Color color)
{
    if (asw::display::renderer == nullptr) {
        return;
    }

    SDL_SetRenderDrawColor(asw::display::renderer, color.r, color.g, color.b, color.a);
    SDL_RenderClear(asw::display::renderer);
}

void asw::draw::sprite(const asw::Texture& tex, const asw::Vec2<float>& position)
{
    if (asw::display::renderer == nullptr) {
        return;
    }

    auto size = asw::util::get_texture_size(tex);

    SDL_FRect dest;
    dest.x = position.x;
    dest.y = position.y;
    dest.w = size.x;
    dest.h = size.y;

    SDL_RenderTexture(asw::display::renderer, tex.get(), nullptr, &dest);
}

void asw::draw::sprite_flip(
    const asw::Texture& tex, const asw::Vec2<float>& position, bool flip_x, bool flip_y)
{
    if (asw::display::renderer == nullptr) {
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

    SDL_RenderTextureRotated(asw::display::renderer, tex.get(), nullptr, &dest, 0, nullptr, flip);
}

void asw::draw::stretch_sprite(const asw::Texture& tex, const asw::Quad<float>& position)
{
    if (asw::display::renderer == nullptr) {
        return;
    }

    SDL_FRect dest;
    dest.x = position.position.x;
    dest.y = position.position.y;
    dest.w = position.size.x;
    dest.h = position.size.y;

    SDL_RenderTexture(asw::display::renderer, tex.get(), nullptr, &dest);
}

void asw::draw::rotate_sprite(
    const asw::Texture& tex, const asw::Vec2<float>& position, float angle)
{
    if (asw::display::renderer == nullptr) {
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

    SDL_RenderTextureRotated(
        asw::display::renderer, tex.get(), nullptr, &dest, angleDeg, nullptr, SDL_FLIP_NONE);
}

void asw::draw::stretch_sprite_blit(
    const asw::Texture& tex, const asw::Quad<float>& source, const asw::Quad<float>& dest)
{
    if (asw::display::renderer == nullptr) {
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

    SDL_RenderTexture(asw::display::renderer, tex.get(), &r_src, &r_dest);
}

void asw::draw::stretch_sprite_rotate_blit(const asw::Texture& tex, const asw::Quad<float>& source,
    const asw::Quad<float>& dest, float angle)
{
    if (asw::display::renderer == nullptr) {
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

    SDL_RenderTextureRotated(
        asw::display::renderer, tex.get(), &r_src, &r_dest, angleDeg, nullptr, SDL_FLIP_NONE);
}

void asw::draw::text(const asw::Font& font, const std::string& text,
    const asw::Vec2<float>& position, asw::Color color, asw::TextJustify justify)
{
    if (text.empty() || asw::display::renderer == nullptr) {
        return;
    }

    const auto sdlColor = SDL_Color { color.r, color.g, color.b, color.a };
    SDL_Surface* textSurface = TTF_RenderText_Solid(font.get(), text.c_str(), 0, sdlColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(asw::display::renderer, textSurface);

    SDL_SetTextureBlendMode(textTexture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureScaleMode(textTexture, SDL_SCALEMODE_NEAREST);

    SDL_FRect dest;
    dest.x = position.x;
    dest.y = position.y;
    dest.w = float(textSurface->w);
    dest.h = float(textSurface->h);

    // Justification settings
    if (justify == asw::TextJustify::Center) {
        dest.x -= dest.w / 2.0F;
    } else if (justify == asw::TextJustify::Right) {
        dest.x -= dest.w;
    }

    SDL_RenderTexture(asw::display::renderer, textTexture, nullptr, &dest);
    SDL_DestroySurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

void asw::draw::point(const asw::Vec2<float>& position, asw::Color color)
{
    if (asw::display::renderer == nullptr) {
        return;
    }

    SDL_SetRenderDrawColor(asw::display::renderer, color.r, color.g, color.b, color.a);
    SDL_RenderPoint(asw::display::renderer, position.x, position.y);
}

void asw::draw::line(
    const asw::Vec2<float>& position1, const asw::Vec2<float>& position2, asw::Color color)
{
    if (asw::display::renderer == nullptr) {
        return;
    }

    SDL_SetRenderDrawColor(asw::display::renderer, color.r, color.g, color.b, color.a);
    SDL_RenderLine(asw::display::renderer, position1.x, position1.y, position2.x, position2.y);
}

void asw::draw::rect(const asw::Quad<float>& position, asw::Color color)
{
    if (asw::display::renderer == nullptr) {
        return;
    }

    SDL_SetRenderDrawColor(asw::display::renderer, color.r, color.g, color.b, color.a);
    SDL_FRect rect;
    rect.x = position.position.x;
    rect.y = position.position.y;
    rect.w = position.size.x;
    rect.h = position.size.y;

    SDL_RenderRect(asw::display::renderer, &rect);
}

void asw::draw::rect_fill(const asw::Quad<float>& position, asw::Color color)
{
    if (asw::display::renderer == nullptr) {
        return;
    }

    SDL_SetRenderDrawColor(asw::display::renderer, color.r, color.g, color.b, color.a);
    SDL_FRect rect;
    rect.x = position.position.x;
    rect.y = position.position.y;
    rect.w = position.size.x;
    rect.h = position.size.y;

    SDL_RenderFillRect(asw::display::renderer, &rect);
}

void asw::draw::circle(const asw::Vec2<float>& position, float radius, asw::Color color)
{
    if (asw::display::renderer == nullptr) {
        return;
    }

    SDL_SetRenderDrawColor(asw::display::renderer, color.r, color.g, color.b, color.a);

    // Midpoint circle algorithm — no trig, integer arithmetic only
    auto x = radius;
    auto y = 0.0F;
    auto err = 1.0F - x;
    const float cx = position.x;
    const float cy = position.y;

    while (x >= y) {
        SDL_RenderPoint(asw::display::renderer, cx + x, cy + y);
        SDL_RenderPoint(asw::display::renderer, cx - x, cy + y);
        SDL_RenderPoint(asw::display::renderer, cx + x, cy - y);
        SDL_RenderPoint(asw::display::renderer, cx - x, cy - y);
        SDL_RenderPoint(asw::display::renderer, cx + y, cy + x);
        SDL_RenderPoint(asw::display::renderer, cx - y, cy + x);
        SDL_RenderPoint(asw::display::renderer, cx + y, cy - x);
        SDL_RenderPoint(asw::display::renderer, cx - y, cy - x);
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
    if (asw::display::renderer == nullptr) {
        return;
    }

    SDL_SetRenderDrawColor(asw::display::renderer, color.r, color.g, color.b, color.a);

    // Midpoint circle with horizontal scanlines — no gaps, no trig
    float x = radius;
    float y = 0.0F;
    float err = 1.0F - x;
    const float cx = position.x;
    const float cy = position.y;

    while (x >= y) {
        SDL_RenderLine(asw::display::renderer, cx - x, cy + y, cx + x, cy + y);
        SDL_RenderLine(asw::display::renderer, cx - x, cy - y, cx + x, cy - y);
        SDL_RenderLine(asw::display::renderer, cx - y, cy + x, cx + y, cy + x);
        SDL_RenderLine(asw::display::renderer, cx - y, cy - x, cx + y, cy - x);
        y++;
        if (err < 0) {
            err += (2.0F * y) + 1.0F;
        } else {
            x--;
            err += (2.0F * (y - x)) + 1.0F;
        }
    }
}

void asw::draw::set_blend_mode(const asw::Texture& texture, asw::BlendMode mode)
{
    SDL_SetTextureBlendMode(texture.get(), static_cast<SDL_BlendMode>(mode));
}

void asw::draw::set_alpha(const asw::Texture& texture, float alpha)
{
    SDL_SetTextureAlphaModFloat(texture.get(), alpha);
}
