/// @file assets.h
/// @author Allan Legemaate (alegemaate@gmail.com)
/// @brief Asset routines for the ASW library
/// @date 2023-09-20
///
/// @copyright Copyright (c) 2023
///

#ifndef ASW_ASSETS_H
#define ASW_ASSETS_H

#include <string>

#include "./color.h"
#include "./types.h"
#include "./util.h"

namespace asw::assets {

// --- Paths ---

/// @brief Get the full path to an asset given its filename. The filename is relative to the base
/// path of the application (the executable directory, or Contents/Resources in a macOS bundle).
/// @param filename The asset filename, relative to the base path.
/// @return The full path to the asset, or the filename unchanged if the base path can not be
/// determined.
std::string get_path(const std::string& filename);

/// @brief Get a writable folder for save files and settings, unique to the
/// organisation and application. The folder is created if needed.
///
/// @param org The organisation name.
/// @param app The application name.
/// @return The folder path ending in a path separator, or an empty string if
/// it can not be determined.
///
std::string get_save_path(const std::string& org, const std::string& app);

/// @brief Read save data written by write_save, e.g. progress or settings.
///
/// @details Desktop builds keep it in a file in get_save_path. Web builds
/// keep it in the browser's localStorage, since the web file system is lost
/// when the page reloads.
///
/// @param org The organisation name.
/// @param app The application name.
/// @param name The save's name, e.g. "progress".
/// @return The saved text, or an empty string if there is none.
///
std::string read_save(const std::string& org, const std::string& app, const std::string& name);

/// @brief Write save data, replacing what was saved under the name before.
///
/// @param org The organisation name.
/// @param app The application name.
/// @param name The save's name, e.g. "progress".
/// @param data The text to save, e.g. JSON.
/// @return true - If it was saved.
///
bool write_save(const std::string& org, const std::string& app, const std::string& name,
    const std::string& data);

// --- Texture ---

/// @brief Loads a texture from a file. Formats supported are PNG, ICO, CUR,
/// BMP, GIF, JPG, LBM, PCX, PNM, TIF, XCF, XPM, XV, and WEBP. This will
/// abort if the file is not found.
///
/// @param filename The path to the texture file.
/// @return A Texture object that will automatically free the underlying
/// SDL_Texture when it goes out of scope.
///
asw::Texture load_texture(const std::string& filename);

/// @brief Load a texture from a file and cache it.
///
/// @param filename The path to the texture file.
/// @param key The key to associate with the loaded texture for caching.
/// @return A Texture object that will automatically free the underlying
/// SDL_Texture when it goes out of scope.
///
asw::Texture load_texture(const std::string& filename, const std::string& key);

/// @brief Get a cached texture.
///
/// @param key The key of the cached texture.
/// @return The cached Texture object.
///
asw::Texture get_texture(const std::string& key);

/// @brief Remove a cached texture.
///
/// @param key The key of the cached texture to remove.
///
void unload_texture(const std::string& key);

/// @brief Create a Texture given the specified dimensions.
///
/// @param w The width of the texture.
/// @param h The height of the texture.
/// @return The created Texture object.
///
asw::Texture create_texture(int w, int h);

/// @brief Create a square texture with a radial gradient. The colour goes
/// smoothly from inner at the centre to outer at the edge. Useful for lights.
///
/// @param size The width and height of the texture.
/// @param inner The colour at the centre.
/// @param outer The colour at the edge and in the corners.
/// @return The created Texture object, with linear scaling and blend mode set.
///
asw::Texture create_radial_gradient(int size, asw::Color inner, asw::Color outer);

// --- Font ---

/// @brief Loads a TTF font from a file. This will abort if the file is not
/// found.
///
/// @param filename The path to the font file.
/// @param size The size of the font.
/// @param style Smooth for regular fonts, Pixel for pixel art fonts.
/// @return The loaded Font object.
///
asw::Font load_font(
    const std::string& filename, float size, asw::FontStyle style = asw::FontStyle::Smooth);

/// @brief Load a font from a file and cache it.
///
/// @param filename The path to the font file.
/// @param size The size of the font.
/// @param key The key to associate with the loaded font for caching.
/// @param style Smooth for regular fonts, Pixel for pixel art fonts.
/// @return The loaded Font object.
///
asw::Font load_font(const std::string& filename, float size, const std::string& key,
    asw::FontStyle style = asw::FontStyle::Smooth);

/// @brief Get a cached font.
///
/// @param key The key of the cached font.
/// @return The cached Font object.
///
asw::Font get_font(const std::string& key);

/// @brief Remove a cached font.
///
/// @param key The key of the cached font to remove.
///
void unload_font(const std::string& key);

// --- Sample ---

/// @brief Loads a sample from a file. Formats supported are WAV, AIFF, RIFF,
/// OGG and VOC. This will abort if the file is not found.
///
/// @param filename The path to the sample file.
/// @return The loaded Sample object.
///
asw::Sample load_sample(const std::string& filename);

/// @brief Load a sample from a file and cache it.
///
/// @param filename The path to the sample file.
/// @param key The key to associate with the loaded sample for caching.
/// @return The loaded Sample object.
///
asw::Sample load_sample(const std::string& filename, const std::string& key);

/// @brief Get a cached sample.
///
/// @param key The key of the cached sample.
/// @return The cached Sample object.
///
asw::Sample get_sample(const std::string& key);

/// @brief Remove a cached sample.
///
/// @param key The key of the cached sample to remove.
///
void unload_sample(const std::string& key);

// --- Music ---

/// @brief Loads a music file from a file. Formats supported are WAV, AIFF,
/// RIFF, OGG and VOC. This will abort if the file is not found.
///
/// @param filename The path to the music file.
/// @return The loaded Music object.
///
asw::Music load_music(const std::string& filename);

/// @brief Load a music file from a file and cache it.
///
/// @param filename The path to the music file.
/// @param key The key to associate with the loaded music for caching.
/// @return The loaded Music object.
///
asw::Music load_music(const std::string& filename, const std::string& key);

/// @brief Get a cached music.
///
/// @param key The key of the cached music.
/// @return The cached Music object.
///
asw::Music get_music(const std::string& key);

/// @brief Remove a cached music.
///
/// @param key The key of the cached music to remove.
///
void unload_music(const std::string& key);

// --- Global ---

/// @brief Clear all cached assets. This will remove all cached textures,
/// fonts, samples, and music.
///
void clear_all();

} // namespace asw::assets

#endif // ASW_ASSETS_H
