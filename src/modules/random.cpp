#include "./asw/modules/random.h"

#include <random>

namespace asw::random {
  namespace {
    // Random number generator
    std::random_device rd;
    std::mt19937 rng(rd());
  }  // namespace

  int random(int max) {
    std::uniform_int_distribution dist(0, max);
    return dist(rng);
  }

  int between(int min, int max) {
    std::uniform_int_distribution dist(min, max);
    return dist(rng);
  }

  float random(float max) {
    std::uniform_real_distribution dist(0.0F, max);
    return dist(rng);
  }

  float between(float min, float max) {
    std::uniform_real_distribution dist(min, max);
    return dist(rng);
  }

  bool chance() {
    std::uniform_int_distribution dist(0, 1);
    return dist(rng) == 1;
  }

  bool chance(float chance) {
    std::uniform_real_distribution dist(0.0F, 1.0F);
    return dist(rng) < chance;
  }
}  // namespace asw::random
