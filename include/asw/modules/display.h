/// @file display.h
/// @author Allan Legemaate (alegemaate@gmail.com)
/// @brief Display and window routines for the ASW library
/// @date 2023-09-20
///
/// @copyright Copyright (c) 2023
///

#ifndef ASW_DISPLAY_H
#define ASW_DISPLAY_H

#include <string>

#include "./color.h"
#include "./types.h"

namespace asw::display {

  /// @brief The renderer for the display module.
  extern asw::Renderer* renderer;

  /// @brief The window for the display module.
  extern asw::Window* window;

  /// @brief Set the title of the window.
  ///
  /// @param title The title to display at the top of the window.
  ///
  void setTitle(const std::string& title);

  /// @brief Set the icon to display on the window. If it does not exist, this
  /// will silently fail.
  ///
  /// @param path A path to the icon to display.
  ///
  void setIcon(const std::string& path);

  /// @brief Set the window to fullscreen or windowed.
  ///
  /// @param fullscreen Whether or not to set the window to fullscreen.
  ///
  void setFullscreen(bool fullscreen);

  /// @brief Set the resolution of the window.
  ///
  /// @param w The width of the window.
  /// @param h The height of the window.
  ///
  void setResolution(int w, int h);

  /// @brief Set resizable flag of the window.
  ///
  /// @param resizable Whether or not the window is resizable.
  ///
  void setResizable(bool resizable);

  /// @brief Get the size of the window.
  ///
  /// @return The size of the window.
  ///
  SDL_Point getSize();

  /// @brief Get the logical size of the window. This may differ from the
  /// actual size if scaling is enabled.
  ///
  /// @return The logical size of the window.
  ///
  SDL_Point getLogicalSize();

  /// @brief Get the scale of the window. This is equivalent to the logical
  /// size divided by the actual size.
  ///
  /// @return The scale of the window.
  ///
  SDL_FPoint getScale();

  /// @brief Set the render target to the window.
  ///
  void setRenderTarget(const asw::Texture& texture);

  /// @brief Reset the render target to the default.
  ///
  void resetRenderTarget();

  /// @brief Clear the window.
  ///
  void clear();

  /// @brief Clear to colour
  ///
  /// @param color The color to clear to.
  ///
  void clear(const asw::Color& color);

  /// @brief Present the window.
  ///
  void present();

  /// @brief Set the blend mode of a texture.
  ///
  /// @param texture The texture to set the blend mode of.
  /// @param mode The blend mode to set.
  ///
  void setBlendMode(asw::BlendMode mode);

}  // namespace asw::display

#endif  // ASW_DISPLAY_H