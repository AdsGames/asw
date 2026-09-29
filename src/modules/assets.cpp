#include "./asw/modules/assets.h"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <system_error>
#ifdef __EMSCRIPTEN__
#include <cstdlib>
#include <emscripten.h>
#endif

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <algorithm>
#include <cmath>
#include <string>
#include <unordered_map>

#include "./asw/modules/display.h"
#include "./asw/modules/draw.h"
#include "./asw/modules/sound.h"
#include "./asw/modules/types.h"
#include "./asw/modules/util.h"

namespace {
std::unordered_map<std::string, asw::Texture> textures;
std::unordered_map<std::string, asw::Font> fonts;
std::unordered_map<std::string, asw::Sample> samples;
std::unordered_map<std::string, asw::Music> music;

// Destroys audio only while the mixer it was loaded with is running. Shutting
// the mixer down frees all its audio, so audio loaded before a shutdown is not
// destroyed again, even after the mixer is started again.
auto audio_deleter()
{
    return [session = asw::sound::_get_session()](MIX_Audio* a) {
        if (asw::sound::get_mixer() != nullptr && asw::sound::_get_session() == session) {
            MIX_DestroyAudio(a);
        }
    };
}
} // namespace

// --- Paths ---

std::string asw::assets::get_path(const std::string& filename)
{
    // base_path is usually ".../YourGame.app/Contents/Resources/" on mac
    // simply the directory of the executable on other platforms
    const char* base_path = SDL_GetBasePath();
    if (base_path == nullptr) {
        return filename;
    }
    return std::string(base_path) + filename;
}

std::string asw::assets::get_save_path(const std::string& org, const std::string& app)
{
    char* pref_path = SDL_GetPrefPath(org.c_str(), app.c_str());
    if (pref_path == nullptr) {
        return "";
    }

    std::string path(pref_path);
    SDL_free(pref_path);
    return path;
}

// --- Texture ---

asw::Texture asw::assets::load_texture(const std::string& filename)
{
    const auto full_path = get_path(filename);
    SDL_Texture* temp = IMG_LoadTexture(asw::display::get_renderer(), full_path.c_str());

    if (temp == nullptr) {
        asw::util::abort_on_error("Failed to load texture: " + full_path);
    }

    SDL_SetTextureScaleMode(temp, SDL_SCALEMODE_NEAREST);
    SDL_SetTextureBlendMode(temp, SDL_BLENDMODE_BLEND);

    // Guard: if the renderer is already gone when the deleter fires (e.g. a
    // shared_ptr outliving core::shutdown()), skip the SDL call - SDL has
    // already freed the texture via SDL_DestroyRenderer.
    return { temp, [](SDL_Texture* t) {
                if (asw::display::get_renderer() != nullptr) {
                    SDL_DestroyTexture(t);
                }
            } };
}

asw::Texture asw::assets::load_texture(const std::string& filename, const std::string& key)
{
    if (auto it = textures.find(key); it != textures.end()) {
        return it->second;
    }

    Texture tex = load_texture(filename);
    textures.try_emplace(key, tex);
    return tex;
}

asw::Texture asw::assets::get_texture(const std::string& key)
{
    auto it = textures.find(key);
    if (it == textures.end()) {
        asw::util::abort_on_error("Texture not found: " + key);
    }
    return it->second;
}

void asw::assets::unload_texture(const std::string& key)
{
    textures.erase(key);
}

asw::Texture asw::assets::create_texture(int w, int h)
{
    auto* r = asw::display::get_renderer();
    if (r == nullptr) {
        asw::util::abort_on_error("Renderer not initialized");
    }

    SDL_Texture* txr
        = SDL_CreateTexture(r, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);

    if (txr == nullptr) {
        asw::util::abort_on_error("Failed to create texture: " + std::string(SDL_GetError()));
    }

    SDL_SetTextureScaleMode(txr, SDL_SCALEMODE_NEAREST);
    SDL_SetTextureBlendMode(txr, SDL_BLENDMODE_BLEND);

    return { txr, [](SDL_Texture* t) {
                if (asw::display::get_renderer() != nullptr) {
                    SDL_DestroyTexture(t);
                }
            } };
}

asw::Texture asw::assets::create_radial_gradient(int size, asw::Color inner, asw::Color outer)
{
    auto* r = asw::display::get_renderer();
    if (r == nullptr) {
        asw::util::abort_on_error("Renderer not initialized");
    }

    SDL_Surface* surface = SDL_CreateSurface(size, size, SDL_PIXELFORMAT_RGBA32);
    if (surface == nullptr) {
        asw::util::abort_on_error("Failed to create gradient surface");
    }

    const auto* details = SDL_GetPixelFormatDetails(surface->format);
    const float half = static_cast<float>(size) / 2.0F;

    auto mix = [](uint8_t a, uint8_t b, float t) {
        return static_cast<uint8_t>(
            std::lround(static_cast<float>(a) + (static_cast<float>(b) - a) * t));
    };

    for (int y = 0; y < size; y++) {
        auto* row = reinterpret_cast<uint32_t*>(
            static_cast<uint8_t*>(surface->pixels) + y * surface->pitch);

        for (int x = 0; x < size; x++) {
            const float distance = std::hypot(static_cast<float>(x) + 0.5F - half,
                                       static_cast<float>(y) + 0.5F - half)
                / half;

            // Smoothstep from the centre to the edge
            const float t = std::clamp(distance, 0.0F, 1.0F);
            const float amount = t * t * (3.0F - 2.0F * t);

            row[x] = SDL_MapRGBA(details, nullptr, mix(inner.r, outer.r, amount),
                mix(inner.g, outer.g, amount), mix(inner.b, outer.b, amount),
                mix(inner.a, outer.a, amount));
        }
    }

    SDL_Texture* txr = SDL_CreateTextureFromSurface(r, surface);
    SDL_DestroySurface(surface);

    if (txr == nullptr) {
        asw::util::abort_on_error("Failed to create gradient texture");
    }

    SDL_SetTextureScaleMode(txr, SDL_SCALEMODE_LINEAR);
    SDL_SetTextureBlendMode(txr, SDL_BLENDMODE_BLEND);

    return { txr, [](SDL_Texture* t) {
                if (asw::display::get_renderer() != nullptr) {
                    SDL_DestroyTexture(t);
                }
            } };
}

// --- Font ---

asw::Font asw::assets::load_font(const std::string& filename, float size, asw::FontStyle style)
{
    const auto full_path = get_path(filename);
    TTF_Font* temp = TTF_OpenFont(full_path.c_str(), size);

    if (temp == nullptr) {
        asw::util::abort_on_error("Failed to load font: " + full_path);
    }

    // Mono hinting renders glyphs without anti-aliasing. draw::text reads it
    // back to pick nearest filtering.
    if (style == asw::FontStyle::Pixel) {
        TTF_SetFontHinting(temp, TTF_HINTING_MONO);
    }

    // Only close while SDL_ttf is running. Checking the renderer instead
    // leaked every font in OpenGL mode, which has no renderer.
    return { temp, [](TTF_Font* f) {
                if (TTF_WasInit() > 0) {
                    TTF_CloseFont(f);
                }
            } };
}

asw::Font asw::assets::load_font(
    const std::string& filename, float size, const std::string& key, asw::FontStyle style)
{
    if (auto it = fonts.find(key); it != fonts.end()) {
        return it->second;
    }

    Font font = load_font(filename, size, style);
    fonts.try_emplace(key, font);
    return font;
}

asw::Font asw::assets::get_font(const std::string& key)
{
    auto it = fonts.find(key);
    if (it == fonts.end()) {
        asw::util::abort_on_error("Font not found: " + key);
    }
    return it->second;
}

void asw::assets::unload_font(const std::string& key)
{
    fonts.erase(key);
    asw::draw::clear_text_cache();
    asw::util::clear_text_size_cache();
}

// --- Sample ---

asw::Sample asw::assets::load_sample(const std::string& filename)
{
    const auto full_path = get_path(filename);
    MIX_Audio* temp = MIX_LoadAudio(asw::sound::get_mixer(), full_path.c_str(), true);

    if (temp == nullptr) {
        asw::util::abort_on_error("Failed to load sample: " + full_path);
    }

    return { temp, audio_deleter() };
}

asw::Sample asw::assets::load_sample(const std::string& filename, const std::string& key)
{
    if (auto it = samples.find(key); it != samples.end()) {
        return it->second;
    }

    Sample sample = load_sample(filename);
    samples.try_emplace(key, sample);
    return sample;
}

asw::Sample asw::assets::get_sample(const std::string& key)
{
    auto it = samples.find(key);
    if (it == samples.end()) {
        asw::util::abort_on_error("Sample not found: " + key);
    }
    return it->second;
}

void asw::assets::unload_sample(const std::string& key)
{
    samples.erase(key);
}

// --- Music ---

asw::Music asw::assets::load_music(const std::string& filename)
{
    const auto full_path = get_path(filename);
    MIX_Audio* temp = MIX_LoadAudio(asw::sound::get_mixer(), full_path.c_str(), false);

    if (temp == nullptr) {
        asw::util::abort_on_error("Failed to load music: " + full_path);
    }

    return { temp, audio_deleter() };
}

asw::Music asw::assets::load_music(const std::string& filename, const std::string& key)
{
    if (auto it = music.find(key); it != music.end()) {
        return it->second;
    }

    Music mus = load_music(filename);
    music.try_emplace(key, mus);
    return mus;
}

asw::Music asw::assets::get_music(const std::string& key)
{
    auto it = music.find(key);
    if (it == music.end()) {
        asw::util::abort_on_error("Music not found: " + key);
    }
    return it->second;
}

void asw::assets::unload_music(const std::string& key)
{
    music.erase(key);
}

// --- Global ---

void asw::assets::clear_all()
{
    asw::draw::clear_text_cache();
    asw::util::clear_text_size_cache();
    textures.clear();
    fonts.clear();
    samples.clear();
    music.clear();
}

#ifdef __EMSCRIPTEN__
namespace {
// Storage can be blocked, which reads as empty and fails to write
// clang-format off
EM_JS(char*, asw_storage_read, (const char* key), {
    try {
        const value = localStorage.getItem(UTF8ToString(key));
        return value === null ? 0 : stringToNewUTF8(value);
    } catch (e) {
        return 0;
    }
});

EM_JS(int, asw_storage_write, (const char* key, const char* value), {
    try {
        localStorage.setItem(UTF8ToString(key), UTF8ToString(value));
        return 1;
    } catch (e) {
        return 0;
    }
});
// clang-format on

std::string storage_key(const std::string& org, const std::string& app, const std::string& name)
{
    return org + "/" + app + "/" + name;
}
} // namespace

std::string asw::assets::read_save(
    const std::string& org, const std::string& app, const std::string& name)
{
    char* value = asw_storage_read(storage_key(org, app, name).c_str());
    if (value == nullptr) {
        return "";
    }
    std::string out(value);
    free(value); // NOLINT(cppcoreguidelines-no-malloc), allocated by emscripten
    return out;
}

bool asw::assets::write_save(
    const std::string& org, const std::string& app, const std::string& name, const std::string& data)
{
    return asw_storage_write(storage_key(org, app, name).c_str(), data.c_str()) != 0;
}
#else
std::string asw::assets::read_save(
    const std::string& org, const std::string& app, const std::string& name)
{
    const auto folder = get_save_path(org, app);
    if (folder.empty()) {
        return "";
    }
    std::ifstream file(folder + name);
    if (!file) {
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool asw::assets::write_save(
    const std::string& org, const std::string& app, const std::string& name, const std::string& data)
{
    const auto folder = get_save_path(org, app);
    if (folder.empty()) {
        return false;
    }

    // Write a temporary file and swap it in, so a crash mid write keeps the
    // old save
    const std::string path = folder + name;
    const std::string temp = path + ".tmp";
    {
        std::ofstream file(temp, std::ios::trunc | std::ios::binary);
        if (!file || !(file << data)) {
            return false;
        }
    }
    std::error_code ec;
    std::filesystem::rename(temp, path, ec);
    if (ec) {
        // Some platforms do not replace an existing file on rename
        std::filesystem::remove(path, ec);
        std::filesystem::rename(temp, path, ec);
    }
    return !ec;
}
#endif
