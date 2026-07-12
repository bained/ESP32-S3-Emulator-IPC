#pragma once

#include "../Core/Core.h"
#include <type_traits>

// ==========================================
// Arduino Mathematical Helpers
// ==========================================

template <typename T>
constexpr T sq(T x) {
    return x * x;
}

template <typename T>
constexpr T constrain(T amt, T low, T high) {
    return (amt < low) ? low : ((amt > high) ? high : amt);
}

// Safer modern template alternatives for min and max (common in modern Arduino cores)
#ifndef min
template <typename A, typename B>
constexpr auto min(A a, B b) -> typename std::common_type<A, B>::type {
    return (a < b) ? a : b;
}
#endif

#ifndef max
template <typename A, typename B>
constexpr auto max(A a, B b) -> typename std::common_type<A, B>::type {
    return (a > b) ? a : b;
}
#endif

// Map Function
long map(long x, long in_min, long in_max, long out_min, long out_max);

// ==========================================
// Arduino Random Number Generation
// ==========================================

long random(long max_val);
long random(long min_val, long max_val);
void randomSeed(unsigned long seed);
