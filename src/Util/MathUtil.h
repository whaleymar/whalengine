#pragma once

#include <cmath>
#include <concepts>
#include "Util/Types.h"

#define EPSILON 0.001

namespace whal {

template <class T>
concept SignedNumber = std::signed_integral<T> || std::floating_point<T>;

template <class T>
concept Number = std::integral<T> || std::floating_point<T>;

inline SignedNumber auto abs(SignedNumber auto const number) {
    return number < 0 ? -number : number;
}

template <std::totally_ordered T>
inline T clamp(const T number, const T min, const T max) {
    if (number < min) {
        return min;
    }
    if (number > max) {
        return max;
    }
    return number;
}

inline SignedNumber auto sign(SignedNumber auto const number) {
    return number < 0 ? -1 : 1;
}

// this might be broken
template <std::floating_point T>
inline bool isNearZero(const T value, const T epsilon = EPSILON) {
    return abs(value) < EPSILON;
}

// inline Number auto lerp(Number auto n1, Number auto n2, f32 t) {
// return (1-t) * n1 + t * n2;
inline f32 lerp(const f32 n1, const f32 n2, const f32 t) {
    return std::lerp(n1, n2, t);
}

}  // namespace whal
