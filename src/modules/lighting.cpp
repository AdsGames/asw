#include "./asw/modules/lighting.h"

#include <SDL3/SDL.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <numbers>

#include "./asw/modules/assets.h"
#include "./asw/modules/display.h"
#include "./asw/modules/draw.h"
#include "./asw/modules/easing.h"

namespace {
constexpr float TAU = std::numbers::pi_v<float> * 2.0F;

// Size of the gradient texture lights are drawn from
constexpr int GRADIENT_SIZE = 256;

// How far shadows are pushed away from a light. Far past any screen
constexpr float SHADOW_FAR = 100000.0F;

using asw::Polygonf;
using asw::Vec2;

// Hash of an integer to 0-1
float hash(uint32_t x)
{
    x = ((x >> 16U) ^ x) * 0x45d9f3bU;
    x = ((x >> 16U) ^ x) * 0x45d9f3bU;
    x = (x >> 16U) ^ x;
    return static_cast<float>(x & 0xffffU) / 65535.0F;
}

// Smooth random value from 0 to 1 that changes with x
float noise(double x)
{
    const double cell = std::floor(x);
    const float t = asw::easing::smoothstep(static_cast<float>(x - cell));
    const auto i = static_cast<uint32_t>(static_cast<int64_t>(cell));
    return hash(i) + ((hash(i + 1) - hash(i)) * t);
}

// Time wrapped into 0 to length
float wrap(float time, float length)
{
    const float t = std::fmod(time, length);
    return t < 0.0F ? t + length : t;
}

asw::Color scale_color(const asw::Color& color, float amount)
{
    return { asw::Color::to_channel(static_cast<float>(color.r) * amount),
        asw::Color::to_channel(static_cast<float>(color.g) * amount),
        asw::Color::to_channel(static_cast<float>(color.b) * amount) };
}

// Draw a texture as a fan from a centre point. Texture coordinates are taken
// from a square of half size radius around the centre
void textured_fan(const asw::Texture& texture, const Vec2<float>& center, float radius,
    const Polygonf& points, const asw::Color& color, bool closed)
{
    auto* r = asw::display::get_renderer();
    if (r == nullptr || points.size() < 2) {
        return;
    }

    const asw::FColor fcolor = color.to_fcolor();
    auto uv = [&](const Vec2<float>& p) {
        return SDL_FPoint { std::clamp(0.5F + ((p.x - center.x) / (radius * 2.0F)), 0.0F, 1.0F),
            std::clamp(0.5F + ((p.y - center.y) / (radius * 2.0F)), 0.0F, 1.0F) };
    };

    static std::vector<SDL_Vertex> vertices;
    static std::vector<int> indices;
    vertices.clear();
    indices.clear();

    vertices.push_back({ { center.x, center.y }, fcolor, { 0.5F, 0.5F } });
    for (const auto& p : points) {
        vertices.push_back({ { p.x, p.y }, fcolor, uv(p) });
    }

    const int count = static_cast<int>(points.size());
    for (int i = 1; i < count; ++i) {
        indices.insert(indices.end(), { 0, i, i + 1 });
    }
    if (closed) {
        indices.insert(indices.end(), { 0, count, 1 });
    }

    SDL_RenderGeometry(r, texture.get(), vertices.data(), static_cast<int>(vertices.size()),
        indices.data(), static_cast<int>(indices.size()));
}

// Arc of a spot light's beam
Polygonf arc(const Vec2<float>& center, float radius, float direction, float cone)
{
    const int segments = std::max(4, static_cast<int>(std::ceil(cone / TAU * 64.0F)));
    Polygonf points;
    for (int i = 0; i <= segments; ++i) {
        const float angle = direction - (cone / 2.0F)
            + (cone * static_cast<float>(i) / static_cast<float>(segments));
        points.emplace_back(
            center.x + (std::cos(angle) * radius), center.y + (std::sin(angle) * radius));
    }
    return points;
}

// Index of a tile in a row-major grid
std::size_t cell(int x, int y, int width)
{
    return (static_cast<std::size_t>(y) * static_cast<std::size_t>(width))
        + static_cast<std::size_t>(x);
}

bool is_spot(float cone)
{
    return cone > 0.0F && cone < TAU;
}
} // namespace

// --- AmbientCycle ---

asw::lighting::AmbientCycle::AmbientCycle(float length)
    : length(std::max(length, 0.0001F))
{
}

void asw::lighting::AmbientCycle::add(float time, const asw::Color& color)
{
    keys.emplace_back(wrap(time, length), color);
    std::ranges::sort(keys, { }, &std::pair<float, asw::Color>::first);
}

asw::Color asw::lighting::AmbientCycle::at(float time) const
{
    if (keys.empty()) {
        return { };
    }

    const float t = wrap(time, length);

    // First key after t, wrapping to the start of the next cycle
    auto next = std::ranges::upper_bound(keys, t, { }, &std::pair<float, asw::Color>::first);
    const auto& to = next == keys.end() ? keys.front() : *next;
    const auto& from = next == keys.begin() ? keys.back() : *std::prev(next);

    float span = to.first - from.first;
    float into = t - from.first;
    if (span <= 0.0F) {
        span += length;
    }
    if (into < 0.0F) {
        into += length;
    }

    const float f = std::clamp(into / span, 0.0F, 1.0F);
    return from.second.lerp(to.second, asw::easing::smoothstep(f));
}

float asw::lighting::AmbientCycle::get_length() const
{
    return length;
}

// --- TileLight ---

asw::lighting::TileLight::TileLight(int width, int height, float tile_size)
    : width(std::max(width, 1))
    , height(std::max(height, 1))
    , tile_size(tile_size)
    , solid(static_cast<std::size_t>(this->width) * static_cast<std::size_t>(this->height), 0)
    , light(
          static_cast<std::size_t>(this->width) * static_cast<std::size_t>(this->height) * 3, 0.0F)
{
}

bool asw::lighting::TileLight::in_grid(int x, int y) const
{
    return x >= 0 && y >= 0 && x < width && y < height;
}

void asw::lighting::TileLight::set_solid(int x, int y, bool is_solid)
{
    if (in_grid(x, y)) {
        solid[cell(x, y, width)] = is_solid ? 1 : 0;
    }
}

bool asw::lighting::TileLight::is_solid(int x, int y) const
{
    return !in_grid(x, y) || solid[cell(x, y, width)] != 0;
}

void asw::lighting::TileLight::set_falloff(float amount)
{
    falloff = std::clamp(amount, 0.001F, 1.0F);
}

void asw::lighting::TileLight::clear_lights()
{
    sources.clear();
}

void asw::lighting::TileLight::add_light(int x, int y, const asw::Color& color)
{
    if (in_grid(x, y)) {
        sources.push_back({ x, y, color });
    }
}

void asw::lighting::TileLight::compute()
{
    std::ranges::fill(light, 0.0F);

    const auto cells = static_cast<std::size_t>(width) * static_cast<std::size_t>(height);
    if (level.size() != cells) {
        level.assign(cells, 0.0F);
    }

    // Spread each light out one tile at a time, keeping the brightest light
    // that reaches each tile. The queue is a list with a read position, so
    // after the spread it holds every tile the light reached
    for (const auto& source : sources) {
        const auto start = cell(source.x, source.y, width);
        level[start] = 1.0F;
        open.clear();
        open.push_back(start);

        for (std::size_t head = 0; head < open.size(); ++head) {
            const auto index = open[head];
            const int x = static_cast<int>(index % static_cast<std::size_t>(width));
            const int y = static_cast<int>(index / static_cast<std::size_t>(width));

            const float here = level[index];
            light[(index * 3) + 0]
                = std::max(light[(index * 3) + 0], here * static_cast<float>(source.color.r));
            light[(index * 3) + 1]
                = std::max(light[(index * 3) + 1], here * static_cast<float>(source.color.g));
            light[(index * 3) + 2]
                = std::max(light[(index * 3) + 2], here * static_cast<float>(source.color.b));

            // Solid tiles are lit, but pass nothing on
            if (is_solid(x, y) && index != start) {
                continue;
            }

            const float next = here - falloff;
            if (next <= 0.0F) {
                continue;
            }

            constexpr std::array<std::pair<int, int>, 4> STEPS { {
                { 1, 0 },
                { -1, 0 },
                { 0, 1 },
                { 0, -1 },
            } };
            for (const auto& [dx, dy] : STEPS) {
                const int nx = x + dx;
                const int ny = y + dy;
                if (!in_grid(nx, ny)) {
                    continue;
                }
                const auto n = cell(nx, ny, width);
                if (level[n] < next) {
                    level[n] = next;
                    open.push_back(n);
                }
            }
        }

        // Reset only the tiles this light reached, not the whole grid
        for (const auto index : open) {
            level[index] = 0.0F;
        }
    }

    upload();
}

// Copy the light into the texture, one pixel per tile
void asw::lighting::TileLight::upload()
{
    const auto cells = static_cast<std::size_t>(width) * static_cast<std::size_t>(height);

    auto* r = asw::display::get_renderer();
    if (r == nullptr) {
        return;
    }

    if (texture == nullptr) {
        SDL_Texture* txr = SDL_CreateTexture(
            r, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, width, height);
        if (txr == nullptr) {
            return;
        }
        SDL_SetTextureScaleMode(txr, SDL_SCALEMODE_LINEAR);
        SDL_SetTextureBlendMode(txr, SDL_BLENDMODE_ADD);
        texture = { txr, [](SDL_Texture* t) {
                       if (asw::display::get_renderer() != nullptr) {
                           SDL_DestroyTexture(t);
                       }
                   } };
    }

    pixels.resize(cells * 4);
    for (std::size_t i = 0; i < cells; ++i) {
        pixels[(i * 4) + 0] = asw::Color::to_channel(light[(i * 3) + 0]);
        pixels[(i * 4) + 1] = asw::Color::to_channel(light[(i * 3) + 1]);
        pixels[(i * 4) + 2] = asw::Color::to_channel(light[(i * 3) + 2]);
        pixels[(i * 4) + 3] = 255;
    }
    SDL_UpdateTexture(texture.get(), nullptr, pixels.data(), width * 4);
}

asw::Color asw::lighting::TileLight::get(int x, int y) const
{
    if (!in_grid(x, y)) {
        return { };
    }
    const auto index = cell(x, y, width) * 3;
    return { asw::Color::to_channel(light[index]), asw::Color::to_channel(light[index + 1]),
        asw::Color::to_channel(light[index + 2]) };
}

const asw::Texture& asw::lighting::TileLight::get_texture() const
{
    return texture;
}

asw::Vec2<float> asw::lighting::TileLight::get_size() const
{
    return { static_cast<float>(width) * tile_size, static_cast<float>(height) * tile_size };
}

// --- LightMap ---

void asw::lighting::LightMap::set_ambient(const asw::Color& color)
{
    ambient = color;
}

asw::Color asw::lighting::LightMap::get_ambient() const
{
    return ambient;
}

void asw::lighting::LightMap::set_camera(const asw::Camera* cam)
{
    camera = cam;
}

void asw::lighting::LightMap::set_shadow_mode(ShadowMode mode)
{
    shadow_mode = mode;
}

asw::lighting::ShadowMode asw::lighting::LightMap::get_shadow_mode() const
{
    return shadow_mode;
}

void asw::lighting::LightMap::update(float dt)
{
    time += static_cast<double>(dt);
}

void asw::lighting::LightMap::clear()
{
    lights.clear();
    glows.clear();
    tiles.clear();
}

void asw::lighting::LightMap::add(const Light& light)
{
    lights.push_back(light);
}

void asw::lighting::LightMap::add_glow(
    const asw::Texture& texture, const asw::Quad<float>& dest, const asw::Color& tint)
{
    glows.push_back({ texture, dest, tint });
}

void asw::lighting::LightMap::add_tiles(
    const TileLight& tile_light, const asw::Vec2<float>& position)
{
    tiles.push_back({ &tile_light, position });
}

void asw::lighting::LightMap::add_occluder(const asw::Polygonf& polygon)
{
    occluders.push_back(polygon);
}

void asw::lighting::LightMap::add_occluder(const asw::Quad<float>& rect)
{
    const auto& p = rect.position;
    const auto& s = rect.size;
    occluders.push_back({ p, { p.x + s.x, p.y }, p + s, { p.x, p.y + s.y } });
}

void asw::lighting::LightMap::clear_occluders()
{
    occluders.clear();
}

const std::vector<asw::Polygonf>& asw::lighting::LightMap::get_occluders() const
{
    return occluders;
}

asw::Vec2<float> asw::lighting::LightMap::to_screen(const asw::Vec2<float>& point) const
{
    return camera != nullptr ? camera->world_to_screen(point) : point;
}

asw::Quad<float> asw::lighting::LightMap::to_screen(const asw::Quad<float>& rect) const
{
    return camera != nullptr ? camera->world_to_screen(rect) : rect;
}

const asw::Texture& asw::lighting::LightMap::gradient(asw::Falloff falloff)
{
    for (const auto& [kind, texture] : gradients) {
        if (kind == falloff) {
            return texture;
        }
    }

    auto texture = asw::assets::create_radial_gradient(
        GRADIENT_SIZE, asw::Color(255, 255, 255), asw::Color(0, 0, 0), falloff);
    asw::draw::set_blend_mode(texture, asw::BlendMode::Add);
    gradients.emplace_back(falloff, texture);
    return gradients.back().second;
}

// Move the occluders to screen space, and work out what every light needs
// from them once per frame instead of once per light
void asw::lighting::LightMap::prepare_occluders()
{
    screen_occluders.resize(occluders.size());
    screen_bounds.resize(occluders.size());
    screen_winding.resize(occluders.size());

    for (std::size_t i = 0; i < occluders.size(); ++i) {
        auto& screen = screen_occluders[i];
        screen.clear();
        for (const auto& p : occluders[i]) {
            screen.push_back(to_screen(p));
        }
        screen_bounds[i] = asw::geometry::bounds(screen);
        screen_winding[i] = asw::geometry::signed_area(screen) < 0.0F ? -1.0F : 1.0F;
    }
}

bool asw::lighting::LightMap::has_blockers(const asw::Quad<float>& reach) const
{
    for (std::size_t i = 0; i < screen_occluders.size(); ++i) {
        if (screen_occluders[i].size() >= 2 && screen_bounds[i].collides(reach)) {
            return true;
        }
    }
    return false;
}

// Black out the shadow behind each edge that faces away from the light, in
// one draw call
void asw::lighting::LightMap::draw_shadows(
    const asw::Vec2<float>& center, const asw::Quad<float>& reach)
{
    auto* r = asw::display::get_renderer();
    shadow_vertices.clear();
    shadow_indices.clear();

    const asw::FColor black = asw::Color(0, 0, 0).to_fcolor();
    auto away = [&](const Vec2<float>& p) {
        const Vec2<float> dir = p - center;
        const float length = dir.magnitude();
        return length > 0.0F ? p + (dir * (SHADOW_FAR / length)) : p;
    };

    for (std::size_t i = 0; i < screen_occluders.size(); ++i) {
        const auto& polygon = screen_occluders[i];
        if (polygon.size() < 2 || !screen_bounds[i].collides(reach)) {
            continue;
        }

        for (std::size_t j = 0; j < polygon.size(); ++j) {
            const auto& a = polygon[j];
            const auto& b = polygon[(j + 1) % polygon.size()];
            const Vec2<float> edge = b - a;
            const Vec2<float> outward = Vec2<float>(edge.y, -edge.x) * screen_winding[i];
            const Vec2<float> mid = (a + b) / 2.0F;

            // Only edges facing away from the light cast a shadow, so the
            // side facing the light stays lit
            if (outward.dot(mid - center) <= 0.0F) {
                continue;
            }

            // The shadow is convex, so a fan from a covers it
            const int base = static_cast<int>(shadow_vertices.size());
            for (const auto& p : { a, b, away(b), away(mid), away(a) }) {
                shadow_vertices.push_back({ { p.x, p.y }, black, { 0.0F, 0.0F } });
            }
            shadow_indices.insert(shadow_indices.end(),
                { base, base + 1, base + 2, base, base + 2, base + 3, base, base + 3, base + 4 });
        }
    }

    if (shadow_indices.empty()) {
        return;
    }

    // Geometry with no texture uses the draw blend mode. Replace, so the
    // shadow is black whatever the game left it on
    SDL_BlendMode previous = SDL_BLENDMODE_NONE;
    SDL_GetRenderDrawBlendMode(r, &previous);
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);
    SDL_RenderGeometry(r, nullptr, shadow_vertices.data(), static_cast<int>(shadow_vertices.size()),
        shadow_indices.data(), static_cast<int>(shadow_indices.size()));
    SDL_SetRenderDrawBlendMode(r, previous);
}

void asw::lighting::LightMap::draw_light(const Light& light)
{
    // Flicker dims the light at random, pulse brightens and dims it steadily.
    // Phases are worked out in double, then wrapped, so they stay smooth
    const float flicker = 1.0F
        - (std::clamp(light.flicker, 0.0F, 1.0F)
            * noise((time * 12.0) + (static_cast<double>(light.seed) * 31.7)));
    const auto phase
        = static_cast<float>(std::fmod(time * static_cast<double>(light.pulse_speed), 1.0));
    const float pulse = 1.0F
        + (std::clamp(light.pulse, 0.0F, 1.0F)
            * std::sin((phase * TAU) + static_cast<float>(light.seed)));
    const float strength = std::max(flicker * pulse, 0.0F);

    const float radius = light.radius * (1.0F + ((strength - 1.0F) * 0.35F));
    if (radius <= 0.0F) {
        return;
    }

    const auto center = to_screen(light.position);
    const asw::Quad<float> square(
        center - Vec2<float>(radius, radius), Vec2<float>(radius * 2.0F, radius * 2.0F));

    // Skip lights that do not reach the screen
    const asw::Quad<float> screen(
        0.0F, 0.0F, static_cast<float>(map_size.x), static_cast<float>(map_size.y));
    if (!square.collides(screen)) {
        return;
    }

    const auto color = scale_color(light.color, std::clamp(light.intensity, 0.0F, 1.0F) * strength);
    const auto& glow = gradient(light.falloff);
    const bool spot = is_spot(light.cone);

    // Draw the lit shape with no shadows
    auto draw_shape = [&]() {
        if (spot) {
            textured_fan(glow, center, radius, arc(center, radius, light.direction, light.cone),
                color, false);
        } else {
            asw::draw::set_tint(glow, color);
            asw::draw::stretch_sprite(glow, square);
            asw::draw::set_tint(glow, asw::Color(255, 255, 255));
        }
    };

    if (!light.shadows || shadow_mode == ShadowMode::None || !has_blockers(square)) {
        draw_shape();
        return;
    }

    if (shadow_mode == ShadowMode::Visibility) {
        asw::geometry::visibility(
            seen, center, radius, screen_occluders, light.direction, light.cone);
        textured_fan(glow, center, radius, seen, color, !spot);
        return;
    }

    // Cast: light the scratch texture, black out the shadows, then add it to
    // the map. Only the part of the screen the light reaches is touched
    auto* r = asw::display::get_renderer();
    const float left = std::max(std::floor(square.position.x), 0.0F);
    const float top = std::max(std::floor(square.position.y), 0.0F);
    const float right = std::min(std::ceil(square.position.x + square.size.x), screen.size.x);
    const float bottom = std::min(std::ceil(square.position.y + square.size.y), screen.size.y);
    const asw::Quad<float> area(left, top, right - left, bottom - top);
    const SDL_Rect clip { static_cast<int>(left), static_cast<int>(top),
        static_cast<int>(right - left), static_cast<int>(bottom - top) };

    asw::display::set_render_target(scratch);
    SDL_SetRenderClipRect(r, &clip);
    asw::draw::rect_fill(area, asw::Color(0, 0, 0));
    draw_shape();
    draw_shadows(center, square);
    SDL_SetRenderClipRect(r, nullptr);

    asw::display::set_render_target(map);
    asw::draw::stretch_sprite_blit(scratch, area, area);
}

const asw::Texture& asw::lighting::LightMap::render()
{
    auto* r = asw::display::get_renderer();
    const auto size = asw::display::get_logical_size();
    if (r == nullptr || size.x <= 0 || size.y <= 0) {
        return map;
    }

    if (map == nullptr || size != map_size) {
        map = asw::assets::create_texture(size.x, size.y);
        asw::draw::set_blend_mode(map, asw::BlendMode::Modulate);
        scratch = asw::assets::create_texture(size.x, size.y);
        asw::draw::set_blend_mode(scratch, asw::BlendMode::Add);
        map_size = size;
    }

    // Put the render target back as it was when done, so a game drawing into
    // its own texture keeps drawing there
    SDL_Texture* previous_target = SDL_GetRenderTarget(r);

    if (shadow_mode != ShadowMode::None) {
        prepare_occluders();
    }

    asw::display::set_render_target(map);
    asw::display::clear(asw::Color(ambient.r, ambient.g, ambient.b));

    for (const auto& grid : tiles) {
        if (grid.grid->get_texture() != nullptr) {
            asw::draw::stretch_sprite(grid.grid->get_texture(),
                to_screen(asw::Quad<float>(grid.position, grid.grid->get_size())));
        }
    }

    for (const auto& light : lights) {
        draw_light(light);
    }

    // Draw glows added, then put the texture back as it was
    for (const auto& glow : glows) {
        SDL_Texture* txr = glow.texture.get();
        if (txr == nullptr) {
            continue;
        }

        SDL_BlendMode mode = SDL_BLENDMODE_NONE;
        uint8_t red = 255;
        uint8_t green = 255;
        uint8_t blue = 255;
        uint8_t alpha = 255;
        SDL_GetTextureBlendMode(txr, &mode);
        SDL_GetTextureColorMod(txr, &red, &green, &blue);
        SDL_GetTextureAlphaMod(txr, &alpha);

        SDL_SetTextureBlendMode(txr, SDL_BLENDMODE_ADD);
        SDL_SetTextureColorMod(txr, glow.tint.r, glow.tint.g, glow.tint.b);
        SDL_SetTextureAlphaMod(txr, glow.tint.a);
        asw::draw::stretch_sprite(glow.texture, to_screen(glow.dest));

        SDL_SetTextureBlendMode(txr, mode);
        SDL_SetTextureColorMod(txr, red, green, blue);
        SDL_SetTextureAlphaMod(txr, alpha);
    }

    SDL_SetRenderTarget(r, previous_target);
    return map;
}

void asw::lighting::LightMap::draw()
{
    const auto& texture = render();
    if (texture == nullptr) {
        return;
    }

    asw::draw::stretch_sprite(
        texture, { 0.0F, 0.0F, static_cast<float>(map_size.x), static_cast<float>(map_size.y) });
}
