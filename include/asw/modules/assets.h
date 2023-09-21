/**
 * @file assets.h
 * @author Allan Legemaate (alegemaate@gmail.com)
 * @brief Asset routines for the ASW library
 * @date 2023-09-20
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef ASW_ASSETS_H
#define ASW_ASSETS_H

#include <string>

#include "./types.h"
#include "./util.h"

namespace asw::assets {
  /**
   * @brief Loads a texture from a file. Formats supported are PNG, ICO, CUR,
   * BMP, GIF, JPG, LBM, PCX, PNM, TIF, XCF, XPM, XV, and WEBP. This will abort
   * if the file is not found.
   *
   * @param filename The path to the file.
   * @return The loaded texture.
   */
  asw::Texture loadTexture(const std::string& filename);

  /**
   * @brief Loads a TTF font from a file. This will abort if the file is not
   * found.
   *
   * @param filename The path to the file.
   * @param size The size of the font.
   * @return The loaded font.
   */
  asw::Font loadFont(const std::string& filename, int size);

  /**
   * @brief Loads a sample from a file. Formats supported are WAV, AIFF, RIFF,
   * OGG and VOC. This will abort if the file is not found.
   *
   * @param filename
   * @return The loaded sample.
   */
  asw::Sample loadSample(const std::string& filename);

  /**
   * @brief Create a Texture given the specified dimensions.
   *
   * @param w The width of the texture.
   * @param h The height of the texture.
   * @return The created texture.
   */
  asw::Texture createTexture(int w, int h);
}  // namespace asw::assets

#endif  // ASW_ASSETS_H