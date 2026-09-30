/// @file lighting.h
/// @author Allan Legemaate (alegemaate@gmail.com)
/// @brief 2D lighting with a light map, shadows and tile lighting
/// @date 2026-09-30
///
/// @copyright Copyright (c) 2026
///

#ifndef ASW_LIGHTING_H
#define ASW_LIGHTING_H

#include <SDL3/SDL.h>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

#include "./camera.h"
#include "./color.h"
#include "./geometry.h"
#include "./types.h"

namespace asw::lighting {

/// @brief How lights are blocked by occluders.
enum class ShadowMode {
    /// Light goes through occluders.
    None,

    /// Each edge facing away from a light casts a shadow. Occluders stay lit
    /// on the side that faces the light. One render pass per light.
    Cast,

    /// Each light fills only what it can see. Occluders stay dark. Cheaper
    /// than Cast, and gives hard edges.
    Visibility,
};

/// @brief A light. Positions are in world space when the light map has a
/// camera, otherwise in screen space.
///
struct Light {
    /// @brief Centre of the light.
    asw::Vec2<float> position;

    /// @brief Distance the light reaches, in pixels.
    float radius { 128.0F };

    /// @brief Colour of the light.
    asw::Color color { 255, 255, 255 };

    /// @brief Brightness from 0 to 1. Scales the colour.
    float intensity { 1.0F };

    /// @brief How the light fades to its edge.
    asw::Falloff falloff { asw::Falloff::Smooth };

    /// @brief Direction a spot light points, in radians. 0 is right, and
    /// angles turn clockwise on screen.
    float direction { 0.0F };

    /// @brief Width of a spot light's beam, in radians. 0 lights all round.
    float cone { 0.0F };

    /// @brief Whether occluders block this light.
    bool shadows { true };

    /// @brief Random flicker from 0 to 1. 0.2 suits a torch.
    float flicker { 0.0F };

    /// @brief Size of a steady pulse from 0 to 1.
    float pulse { 0.0F };

    /// @brief Pulses per second.
    float pulse_speed { 1.0F };

    /// @brief Gives lights their own flicker and pulse timing. Use a
    /// different seed for each light.
    int seed { 0 };
};

/// @brief Ambient colour that changes over a repeating cycle, such as a day.
///
class AmbientCycle {
public:
    /// @brief Create a cycle.
    ///
    /// @param length Length of one cycle, in the same units as at().
    ///
    explicit AmbientCycle(float length = 1.0F);

    /// @brief Add a key colour. Colours between keys are blended.
    ///
    /// @param time When in the cycle, from 0 to the length.
    /// @param color The ambient colour at that time.
    ///
    void add(float time, const asw::Color& color);

    /// @brief Get the colour at a time. Times past the length wrap around.
    ///
    /// @param time The time.
    /// @return The blended colour, or black with no keys.
    ///
    asw::Color at(float time) const;

    /// @brief Get the length of one cycle.
    ///
    float get_length() const;

private:
    float length;
    std::vector<std::pair<float, asw::Color>> keys;
};

/// @brief Light that spreads across a grid of tiles and stops at solid tiles.
///
/// @details Lighting is worked out on the CPU in compute(), then kept in a
/// texture with one pixel per tile. The texture is drawn with smooth scaling,
/// so light blends between tiles. Add it to a LightMap with add_tiles().
///
class TileLight {
public:
    /// @brief Create a grid with no solid tiles and no lights.
    ///
    /// @param width Width in tiles.
    /// @param height Height in tiles.
    /// @param tile_size Size of a tile in pixels.
    ///
    TileLight(int width, int height, float tile_size);

    /// @brief Set whether a tile blocks light. A solid tile is lit, but does
    /// not pass light on.
    ///
    void set_solid(int x, int y, bool solid);

    /// @brief Get whether a tile blocks light. Tiles off the grid are solid.
    ///
    bool is_solid(int x, int y) const;

    /// @brief Set how much light is lost per tile, from 0 to 1.
    ///
    void set_falloff(float amount);

    /// @brief Remove all lights.
    ///
    void clear_lights();

    /// @brief Add a light at a tile.
    ///
    /// @param x Tile x.
    /// @param y Tile y.
    /// @param color Colour at the tile. It fades as it spreads.
    ///
    void add_light(int x, int y, const asw::Color& color);

    /// @brief Spread the lights and update the texture. Call after lights or
    /// solid tiles change.
    ///
    void compute();

    /// @brief Get the light at a tile after compute().
    ///
    asw::Color get(int x, int y) const;

    /// @brief Get the texture, with one pixel per tile. Empty before compute().
    ///
    const asw::Texture& get_texture() const;

    /// @brief Get the size of the grid in pixels.
    ///
    asw::Vec2<float> get_size() const;

private:
    struct Source {
        int x;
        int y;
        asw::Color color;
    };

    bool in_grid(int x, int y) const;
    void upload();

    int width;
    int height;
    float tile_size;
    float falloff { 0.1F };
    std::vector<uint8_t> solid;
    std::vector<Source> sources;
    std::vector<float> light;
    asw::Texture texture;

    // Kept between compute() calls so they do not allocate each time
    std::vector<float> level;
    std::vector<std::size_t> open;
    std::vector<uint8_t> pixels;
};

/// @brief A screen sized texture of light, multiplied over the scene.
///
/// @details Each frame: clear(), then add lights, glows and tiles, then draw
/// the scene and call draw(). Occluders stay until clear_occluders(), so
/// walls only need adding once.
///
class LightMap {
public:
    /// @brief Set the colour of unlit areas. Black is pitch dark, white is
    /// fully lit.
    ///
    void set_ambient(const asw::Color& color);

    /// @brief Get the colour of unlit areas.
    ///
    asw::Color get_ambient() const;

    /// @brief Take positions in world space and convert them with a camera.
    /// The camera must outlive the light map, or be cleared first.
    ///
    void set_camera(const asw::Camera* camera);

    /// @brief Set how occluders block light.
    ///
    void set_shadow_mode(ShadowMode mode);

    /// @brief Get how occluders block light.
    ///
    ShadowMode get_shadow_mode() const;

    /// @brief Advance flicker and pulse. Call once per update.
    ///
    /// @param dt The time in seconds since the last update.
    ///
    void update(float dt);

    /// @brief Remove the lights, glows and tiles added this frame.
    ///
    void clear();

    /// @brief Add a light for this frame.
    ///
    void add(const Light& light);

    /// @brief Add a texture that gives off light, such as lava or a lamp
    /// sprite. Its colours are added to the light map.
    ///
    /// @param texture The texture.
    /// @param dest Where to draw it.
    /// @param tint Colour to multiply the texture by.
    ///
    void add_glow(const asw::Texture& texture, const asw::Quad<float>& dest,
        const asw::Color& tint = asw::Color(255, 255, 255));

    /// @brief Add tile lighting for this frame. The grid must outlive the
    /// next render() or draw().
    ///
    /// @param tile_light The grid, after compute().
    /// @param position Where the top left of the grid is.
    ///
    void add_tiles(const TileLight& tile_light, const asw::Vec2<float>& position = { });

    /// @brief Add a polygon that blocks light.
    ///
    void add_occluder(const asw::Polygonf& polygon);

    /// @brief Add a rectangle that blocks light.
    ///
    void add_occluder(const asw::Quad<float>& rect);

    /// @brief Remove all occluders.
    ///
    void clear_occluders();

    /// @brief Get the occluders.
    ///
    const std::vector<asw::Polygonf>& get_occluders() const;

    /// @brief Draw the light map and multiply it over the current render
    /// target.
    ///
    void draw();

    /// @brief Render the light map without drawing it, to draw it yourself.
    /// The render target is the same afterwards as before.
    ///
    /// @return The light map texture, at the logical screen size.
    ///
    const asw::Texture& render();

private:
    struct Glow {
        asw::Texture texture;
        asw::Quad<float> dest;
        asw::Color tint;
    };

    struct Tiles {
        const TileLight* grid;
        asw::Vec2<float> position;
    };

    asw::Vec2<float> to_screen(const asw::Vec2<float>& point) const;
    asw::Quad<float> to_screen(const asw::Quad<float>& rect) const;
    const asw::Texture& gradient(asw::Falloff falloff);
    void prepare_occluders();
    bool has_blockers(const asw::Quad<float>& reach) const;
    void draw_light(const Light& light);
    void draw_shadows(const asw::Vec2<float>& center, const asw::Quad<float>& reach);

    asw::Color ambient { 0, 0, 0 };
    const asw::Camera* camera { nullptr };
    ShadowMode shadow_mode { ShadowMode::None };

    // Double, so flicker and pulse stay smooth in long sessions
    double time { 0.0 };

    std::vector<Light> lights;
    std::vector<Glow> glows;
    std::vector<Tiles> tiles;
    std::vector<asw::Polygonf> occluders;

    // Occluders in screen space for this frame, with their bounds and which
    // way round their corners go. Kept between frames to reuse the memory
    std::vector<asw::Polygonf> screen_occluders;
    std::vector<asw::Quad<float>> screen_bounds;
    std::vector<float> screen_winding;

    // Reused each light
    asw::Polygonf seen;
    std::vector<SDL_Vertex> shadow_vertices;
    std::vector<int> shadow_indices;

    asw::Texture map;
    asw::Texture scratch;
    asw::Vec2<int> map_size;
    std::vector<std::pair<asw::Falloff, asw::Texture>> gradients;
};

} // namespace asw::lighting

#endif // ASW_LIGHTING_H
