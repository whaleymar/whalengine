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

inline SignedNumber auto abs(SignedNumber auto number) {
    return number < 0 ? -number : number;
}

template <std::totally_ordered T>
inline T clamp(T number, T min, T max) {
    if (number < min) {
        return min;
    }
    if (number > max) {
        return max;
    }
    return number;
}

inline SignedNumber auto sign(SignedNumber auto number) {
    return number < 0 ? -1 : 1;
}

template <std::floating_point T>
inline bool isNearZero(T value) {
    return abs(value) < EPSILON;
}

// inline Number auto lerp(Number auto n1, Number auto n2, f32 t) {
// return (1-t) * n1 + t * n2;
inline f32 lerp(f32 n1, f32 n2, f32 t) {
    return std::lerp(n1, n2, t);
}

}  // namespace whal
