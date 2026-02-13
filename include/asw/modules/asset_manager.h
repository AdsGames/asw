/// @file asset_manager.h
/// @author Allan Legemaate (alegemaate@gmail.com)
/// @brief Centralized asset store with deduplication
/// @date 2026-02-13
///
/// @copyright Copyright (c) 2026
///

#ifndef ASW_ASSET_MANAGER_H
#define ASW_ASSET_MANAGER_H

#include <string>
#include <unordered_map>

#include "./assets.h"
#include "./types.h"

namespace asw {

  /// @brief Centralized asset manager with caching.
  ///
  /// @details Wraps the asw::assets loading functions with a cache layer.
  /// Loading the same asset path twice returns the same shared_ptr without
  /// hitting disk. Not a singleton — create one per scene or share globally.
  ///
  class AssetManager {
   public:
    // --- Textures ---

    /// @brief Get a texture by path (loads and caches on first access).
    ///
    /// @param path The path to the texture file.
    /// @return The loaded texture.
    ///
    Texture getTexture(const std::string& path) {
      auto it = textures.find(path);
      if (it != textures.end()) {
        return it->second;
      }
      auto tex = asw::assets::loadTexture(path);
      textures[path] = tex;
      return tex;
    }

    /// @brief Check if a texture is cached.
    ///
    /// @param path The path to the texture file.
    /// @return True if the texture is in the cache.
    ///
    bool hasTexture(const std::string& path) const {
      return textures.find(path) != textures.end();
    }

    // --- Fonts ---

    /// @brief Get a font by path and size (loads and caches on first access).
    ///
    /// @param path The path to the font file.
    /// @param size The font size.
    /// @return The loaded font.
    ///
    Font getFont(const std::string& path, float size) {
      std::string key = path + ":" + std::to_string(size);
      auto it = fonts.find(key);
      if (it != fonts.end()) {
        return it->second;
      }
      auto font = asw::assets::loadFont(path, size);
      fonts[key] = font;
      return font;
    }

    /// @brief Check if a font is cached.
    ///
    /// @param path The path to the font file.
    /// @param size The font size.
    /// @return True if the font is in the cache.
    ///
    bool hasFont(const std::string& path, float size) const {
      std::string key = path + ":" + std::to_string(size);
      return fonts.find(key) != fonts.end();
    }

    // --- Samples ---

    /// @brief Get a sample by path (loads and caches on first access).
    ///
    /// @param path The path to the sample file.
    /// @return The loaded sample.
    ///
    Sample getSample(const std::string& path) {
      auto it = samples.find(path);
      if (it != samples.end()) {
        return it->second;
      }
      auto sample = asw::assets::loadSample(path);
      samples[path] = sample;
      return sample;
    }

    /// @brief Check if a sample is cached.
    ///
    /// @param path The path to the sample file.
    /// @return True if the sample is in the cache.
    ///
    bool hasSample(const std::string& path) const {
      return samples.find(path) != samples.end();
    }

    // --- Music ---

    /// @brief Get music by path (loads and caches on first access).
    ///
    /// @param path The path to the music file.
    /// @return The loaded music.
    ///
    Music getMusic(const std::string& path) {
      auto it = music.find(path);
      if (it != music.end()) {
        return it->second;
      }
      auto m = asw::assets::loadMusic(path);
      music[path] = m;
      return m;
    }

    /// @brief Check if music is cached.
    ///
    /// @param path The path to the music file.
    /// @return True if the music is in the cache.
    ///
    bool hasMusic(const std::string& path) const {
      return music.find(path) != music.end();
    }

    // --- Management ---

    /// @brief Unload a texture from the cache.
    ///
    /// @param path The path to the texture file.
    ///
    void unloadTexture(const std::string& path) { textures.erase(path); }

    /// @brief Unload a font from the cache.
    ///
    /// @param path The path to the font file.
    /// @param size The font size.
    ///
    void unloadFont(const std::string& path, float size) {
      fonts.erase(path + ":" + std::to_string(size));
    }

    /// @brief Unload a sample from the cache.
    ///
    /// @param path The path to the sample file.
    ///
    void unloadSample(const std::string& path) { samples.erase(path); }

    /// @brief Unload music from the cache.
    ///
    /// @param path The path to the music file.
    ///
    void unloadMusic(const std::string& path) { music.erase(path); }

    /// @brief Clear all cached assets.
    ///
    void clear() {
      textures.clear();
      fonts.clear();
      samples.clear();
      music.clear();
    }

    /// @brief Get the number of cached textures.
    ///
    /// @return The number of cached textures.
    ///
    size_t getTextureCount() const { return textures.size(); }

    /// @brief Get the number of cached fonts.
    ///
    /// @return The number of cached fonts.
    ///
    size_t getFontCount() const { return fonts.size(); }

    /// @brief Get the number of cached samples.
    ///
    /// @return The number of cached samples.
    ///
    size_t getSampleCount() const { return samples.size(); }

    /// @brief Get the number of cached music tracks.
    ///
    /// @return The number of cached music tracks.
    ///
    size_t getMusicCount() const { return music.size(); }

    /// @brief Get the total number of cached assets.
    ///
    /// @return The total number of cached assets.
    ///
    size_t getTotalCount() const {
      return textures.size() + fonts.size() + samples.size() + music.size();
    }

   private:
    std::unordered_map<std::string, Texture> textures;
    std::unordered_map<std::string, Font> fonts;
    std::unordered_map<std::string, Sample> samples;
    std::unordered_map<std::string, Music> music;
  };

}  // namespace asw

#endif  // ASW_ASSET_MANAGER_H
