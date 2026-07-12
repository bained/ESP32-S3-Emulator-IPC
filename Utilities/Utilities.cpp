#include "Utilities.h"
#include <random>

// Global pseudorandom number generator engine
static std::mt19937 g_rng(1337); // Standard default seed

long map(long x, long in_min, long in_max, long out_min, long out_max) {
    long divisor = in_max - in_min;
    if (divisor == 0) {
        return out_min; // Protection against division by zero
    }
    return (x - in_min) * (out_max - out_min) / divisor + out_min;
}

long random(long max_val) {
    if (max_val <= 0) {
        return 0;
    }
    std::uniform_int_distribution<long> dist(0, max_val - 1);
    return dist(g_rng);
}

long random(long min_val, long max_val) {
    if (min_val >= max_val) {
        return min_val;
    }
    std::uniform_int_distribution<long> dist(min_val, max_val - 1);
    return dist(g_rng);
}

void randomSeed(unsigned long seed) {
    if (seed != 0) {
        g_rng.seed(seed);
    }
}
