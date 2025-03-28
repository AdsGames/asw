/// @file random.h
/// @author Allan Legemaate (alegemaate@gmail.com)
/// @brief Random module for the ASW library
/// @date 2025-03-24
///
/// @copyright Copyright (c) 2025
///

#ifndef ASW_RANDOM_H
#define ASW_RANDOM_H

namespace asw::random {

  /// @brief Generate a random integer between 0 and max.
  ///
  /// @param max The maximum value.
  /// @return int The random number.
  ///
  int random(int max);

  /// @brief Generate a random integer between min and max.
  ///
  /// @param min The minimum value.
  /// @param max The maximum value.
  /// @return int The random number.
  ///
  int between(int min, int max);

  /// @brief Generate a random float between 0 and max.
  ///
  /// @param max The maximum value.
  /// @return float The random number.
  ///
  float random(float max);

  /// @brief Generate a random float between min and max.
  ///
  /// @param min The minimum value.
  /// @param max The maximum value.
  /// @return float The random number.
  ///
  float between(float min, float max);

  /// @brief Generate a random boolean.
  ///
  /// @return true - Random true.
  /// @return false - Random false.
  ///
  bool chance();

  /// @brief Generate a random boolean with a given chance of being true.
  ///
  /// @param chance The chance of being true between 0 and 1.
  /// @return true - Random true.
  /// @return false - Random false.
  ///
  bool chance(float chance);

}  // namespace asw::random

#endif  // ASW_RANDOM_H