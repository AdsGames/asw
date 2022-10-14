#ifndef ASW_DISPLAY_H
#define ASW_DISPLAY_H

#include <string>

#include "./types.h"

namespace asw::display {

  // Renderer
  extern asw::Renderer* renderer;
  extern asw::Window* window;

  /**
   * @brief Set the title of the window.
   *
   * @param title - The title to display at the top of the window.
   */
  void setTitle(const std::string& title);

  /**
   * @brief Set the icon to display on the window. If it does not exist, this
   * will silently fail.
   *
   * @param path - A path to the icon to display.
   */
  void setIcon(const std::string& path);

  /**
   * @brief Set the window to fullscreen or windowed.
   *
   * @param fullscreen - Whether or not to set the window to fullscreen.
   */
  void setFullscreen(bool fullscreen);

  /**
   * @brief Set the resolution of the window.
   *
   * @param w - The width of the window.
   * @param h - The height of the window.
   */
  void setResolution(int w, int h);

  /**
   * @brief Get the size of the window.
   *
   * @return SDL_Point - The size of the window.
   */
  SDL_Point getSize();

  /**
   * @brief Get the logical size of the window. This may differ from the
   * actual size if scaling is enabled.
   *
   * @return SDL_Point - The logical size of the window.
   */
  SDL_Point getLogicalSize();

  /**
   * @brief Get the scale of the window. This is equivalent to the logical
   * size divided by the actual size.
   *
   * @return SDL_FPoint - The scale of the window.
   */
  SDL_FPoint getScale();

}  // namespace asw::display

#endif  // ASW_DISPLAY_H