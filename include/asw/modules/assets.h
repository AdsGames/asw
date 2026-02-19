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

#include "./types.h"
#include "./util.h"

namespace asw::assets {

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
  asw::Texture load_texture(const std::string& filename,
                            const std::string& key);

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

  // --- Font ---

  /// @brief Loads a TTF font from a file. This will abort if the file is not
  /// found.
  ///
  /// @param filename The path to the font file.
  /// @param size The size of the font.
  /// @return The loaded Font object.
  ///
  asw::Font load_font(const std::string& filename, float size);

  /// @brief Load a font from a file and cache it.
  ///
  /// @param filename The path to the font file.
  /// @param size The size of the font.
  /// @param key The key to associate with the loaded font for caching.
  /// @return The loaded Font object.
  ///
  asw::Font load_font(const std::string& filename,
                      float size,
                      const std::string& key);

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

}  // namespace asw::assets

#endif  // ASW_ASSETS_H
