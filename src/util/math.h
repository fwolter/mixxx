#pragma once

#include <algorithm>
#include <cmath>
#include <type_traits>

#include "util/assert.h"
#include "util/fpclassify.h"

#if __has_include(<bit>)
#include <bit>
#endif

// If we don't do this then we get the C90 fabs from the global namespace which
// is only defined for double.
using std::fabs;

#define math_max std::max
#define math_min std::min
#define math_max3(a, b, c) std::max({a, b, c});
#define math_min3(a, b, c) std::min({a, b, c});

// Restrict value to the range [min, max]. Undefined behavior if min > max.
template<typename T>
constexpr T math_clamp(T value, T min, T max) {
    // DEBUG_ASSERT compiles out in release builds so it does not affect
    // vectorization or pipelining of clamping in tight loops.
    DEBUG_ASSERT(min <= max);
    return std::clamp(value, min, max);
}

// NOTE(rryan): It is an error to call even() on a floating point number. Do not
// hack this to support floating point values! The programmer should be required
// to manually convert so they are aware of the conversion.
template<typename T>
constexpr bool even(T value) {
    // since we also want to this to work on size_t and ptrdiff_t, is_integer would be too strict.
    static_assert(std::is_arithmetic_v<T> && !std::is_floating_point_v<T>,
            "even only supports integral types");
    return value % 2 == 0;
}

#ifdef _MSC_VER
// Ask VC++ to emit an intrinsic for fabs instead of calling std::fabs.
#pragma intrinsic(fabs)
#endif

// return value of 0 indicates failure (no greater power possible)
constexpr unsigned int roundUpToPowerOf2(unsigned int v) {
#if (defined(__cpp_lib_int_pow2) && __cpp_lib_int_pow2 >= 202002L)
    return std::bit_ceil(v);
#else
    unsigned int power = 1;
    while (power < v && power > 0) {
        power *= 2;
    }
    return power;
#endif
}

// TODO (XXX): make this constexpr once <cmath> has constexpr support
inline double roundToFraction(double value, int denominator) {
    int wholePart = static_cast<int>(value);
    double fractionPart = value - wholePart;
    double numerator = std::round(fractionPart * denominator);
    return wholePart + numerator / denominator;
}

// TODO (XXX): make this constexpr once <cmath> has constexpr support
template<typename T>
inline const T ratio2db(const T a) {
    static_assert(std::is_floating_point_v<T>, "ratio2db works only for floating point types");
    return log10(a) * 20;
}

// TODO (XXX): make this constexpr once <cmath> has constexpr support
template<typename T>
inline const T db2ratio(const T a) {
    static_assert(std::is_floating_point_v<T>, "db2ratio works only for floating point type");
    return static_cast<T>(pow(10, a / 20));
}
