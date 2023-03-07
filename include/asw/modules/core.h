#ifndef ASW_CORE_H
#define ASW_CORE_H

namespace asw::core {
  /// @brief When set to true, exits the main loop.
  extern bool exit;

  /**
   * @brief Updates core module functionality.
   *
   */
  void update();

  /**
   * @brief Initializes the core module.
   *
   * @param width - The width of the window.
   * @param height - The height of the window.
   * @param scale - The scale of the window.
   */
  void init(int width, int height, int scale = 1);

}  // namespace asw::core

#endif  // ASW_CORE_H