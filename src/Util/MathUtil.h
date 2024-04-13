#pragma once

#include <algorithm>
#include <cmath>
#include <concepts>
#include "Util/Types.h"

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

template <std::totally_ordered T>
inline bool isBetween(const T number, const T min, const T max) {
    return number >= min && number <= max;
}

inline SignedNumber auto sign(SignedNumber auto const number) {
    return number < 0 ? -1 : 1;
}

// these are not working
template <std::floating_point T>
inline bool isNearZero(const T value, const T epsilon = 0.001) {
    return abs(value) < epsilon;
}

inline bool isNearZerof(const f32 value, const f32 epsilon) {
    return abs(value) < epsilon;
}

// inline Number auto lerp(Number auto n1, Number auto n2, f32 t) {
// return (1-t) * n1 + t * n2;
inline f32 lerp(const f32 n1, const f32 n2, const f32 t) {
    return std::lerp(n1, n2, t);
}

inline f32 approach(const f32 val, const f32 target, const f32 move) {
    if (val <= target) {
        return std::min(val + move, target);
    }
    return std::max(val - move, target);
}

}  // namespace whal
