#pragma once

#include <concepts>

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
}  // namespace whal
