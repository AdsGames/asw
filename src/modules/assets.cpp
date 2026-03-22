#include "./asw/modules/assets.h"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>
#include <unordered_map>

#include "./asw/modules/display.h"
#include "./asw/modules/sound.h"
#include "./asw/modules/types.h"
#include "./asw/modules/util.h"

namespace {
std::unordered_map<std::string, asw::Texture> textures;
std::unordered_map<std::string, asw::Font> fonts;
std::unordered_map<std::string, asw::Sample> samples;
std::unordered_map<std::string, asw::Music> music;
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

    SDL_SetTextureScaleMode(txr, SDL_SCALEMODE_NEAREST);
    SDL_SetTextureBlendMode(txr, SDL_BLENDMODE_BLEND);

    return { txr, [](SDL_Texture* t) {
                if (asw::display::get_renderer() != nullptr) {
                    SDL_DestroyTexture(t);
                }
            } };
}

// --- Font ---

asw::Font asw::assets::load_font(const std::string& filename, float size)
{
    const auto full_path = get_path(filename);
    TTF_Font* temp = TTF_OpenFont(full_path.c_str(), size);

    if (temp == nullptr) {
        asw::util::abort_on_error("Failed to load font: " + full_path);
    }

    // Use renderer as proxy for "SDL still alive" - renderer is nulled in
    // display::_shutdown() before TTF_Quit() is called.
    return { temp, [](TTF_Font* f) {
                if (asw::display::get_renderer() != nullptr) {
                    TTF_CloseFont(f);
                }
            } };
}

asw::Font asw::assets::load_font(const std::string& filename, float size, const std::string& key)
{
    if (auto it = fonts.find(key); it != fonts.end()) {
        return it->second;
    }

    Font font = load_font(filename, size);
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
}

// --- Sample ---

asw::Sample asw::assets::load_sample(const std::string& filename)
{
    const auto full_path = get_path(filename);
    MIX_Audio* temp = MIX_LoadAudio(asw::sound::get_mixer(), full_path.c_str(), true);

    if (temp == nullptr) {
        asw::util::abort_on_error("Failed to load sample: " + full_path);
    }

    return { temp, [](MIX_Audio* a) {
                if (asw::sound::get_mixer() != nullptr) {
                    MIX_DestroyAudio(a);
                }
            } };
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

    return { temp, [](MIX_Audio* a) {
                if (asw::sound::get_mixer() != nullptr) {
                    MIX_DestroyAudio(a);
                }
            } };
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
    textures.clear();
    fonts.clear();
    samples.clear();
    music.clear();
}
