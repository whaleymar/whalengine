#pragma once

#include "Util/Vector.h"

namespace whal {

struct Position {
    Vector2i e;

    // // TODO macro
    // Position(const Vector2i& other) {
    //     e.e[0] = other.e[0];
    //     e.e[1] = other.e[1];
    // }
    //
    // Position& operator=(const Vector2i& other) {
    //     e.e[0] = other.e[0];
    //     e.e[1] = other.e[1];
    //     return *this;
    // }
    // inline Vector2i operator+(const Vector2i other) const { return Vector2i(e.e[0] + other.e[0], e.e[1] + other.e[1]); }
    // inline Vector2i operator-(const Vector2i other) const { return Vector2i(e.e[0] - other.e[0], e.e[1] - other.e[1]); }
    // inline Vector2i operator*(const Vector2i other) const { return Vector2i(e.e[0] * other.e[0], e.e[1] * other.e[1]); }
    // inline Vector2i operator*(const f32 scalar) const { return Vector2i(e.e[0] * scalar, e.e[1] * scalar); }
};

}  // namespace whal
