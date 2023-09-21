/**
 * @file core.h
 * @author Allan Legemaate (alegemaate@gmail.com)
 * @brief Core routines including main loop and initialization
 * @date 2023-09-20
 *
 * @copyright Copyright (c) 2023
 *
 */
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
   * @param width The width of the window.
   * @param height The height of the window.
   * @param scale The scale of the window.
   */
  void init(int width, int height, int scale = 1);

  /**
   * @brief Prints information about the core module.
   *
   */
  void print_info();

}  // namespace asw::core

#endif  // ASW_CORE_H