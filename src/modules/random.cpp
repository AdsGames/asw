#include "./asw/modules/random.h"

#include <algorithm>
#include <random>

namespace asw::random {
namespace {
    // Random number generator
    std::random_device rd;
    std::mt19937 rng(rd());
} // namespace

// The std distributions are undefined when min > max, so order the range
int random(int max)
{
    return between(0, max);
}

int between(int min, int max)
{
    const auto [lo, hi] = std::minmax(min, max);
    std::uniform_int_distribution dist(lo, hi);
    return dist(rng);
}

float random(float max)
{
    return between(0.0F, max);
}

float between(float min, float max)
{
    const auto [lo, hi] = std::minmax(min, max);
    std::uniform_real_distribution dist(lo, hi);
    return dist(rng);
}

bool chance()
{
    std::uniform_int_distribution dist(0, 1);
    return dist(rng) == 1;
}

bool chance(float chance)
{
    std::uniform_real_distribution dist(0.0F, 1.0F);
    return dist(rng) < chance;
}
} // namespace asw::random
