/**
 * @file random.cpp
 * @author
 * @brief Implementation of the random module for the ASW library
 * @date 2025-03-24
 */

#include "./asw/modules/random.h"

#include <random>

namespace asw::random {
  // Random number generator
  static std::random_device rd;
  static std::mt19937 rng(rd());

  int random(int max) {
    std::uniform_int_distribution<int> dist(0, max);
    return dist(rng);
  }

  int between(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
  }

  float random(float max) {
    std::uniform_real_distribution<float> dist(0.0f, max);
    return dist(rng);
  }

  float between(float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(rng);
  }

  bool chance() {
    std::uniform_int_distribution<int> dist(0, 1);
    return dist(rng) == 1;
  }

  bool chance(float chance) {
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    return dist(rng) < chance;
  }
}  // namespace asw::random
