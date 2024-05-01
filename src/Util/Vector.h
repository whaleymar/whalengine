#pragma once

#include <cmath>
#include <ostream>
#include "Util/MathUtil.h"
#include "Util/Types.h"

namespace whal {

template <Number T>
struct Vector2 {
    T e[2];

    Vector2() {
        e[0] = 0;
        e[1] = 0;
    }

    Vector2(T elem1, T elem2) {
        e[0] = elem1;
        e[1] = elem2;
    }

    Vector2(const Vector2<T>& other) {
        e[0] = other.e[0];
        e[1] = other.e[1];
    }

    static inline Vector2<T> unitUp = {0, 1};
    static inline Vector2<T> unitDown = {0, -1};
    static inline Vector2<T> unitLeft{-1, 0};
    static inline Vector2<T> unitRight{1, 0};
    static inline Vector2<T> zero{0, 0};

    Vector2<T>& operator=(const Vector2<T>& other) {
        if (this != &other) {
            e[0] = other.e[0];
            e[1] = other.e[1];
        }
        return *this;
    }
    inline Vector2<T> operator+(const Vector2<T> other) const { return Vector2<T>(e[0] + other.e[0], e[1] + other.e[1]); }
    inline Vector2<T> operator+=(const Vector2<T> other) {
        e[0] += other.e[0];
        e[1] += other.e[1];
        return *this;
    }
    inline Vector2<T> operator-(const Vector2<T> other) const { return Vector2<T>(e[0] - other.e[0], e[1] - other.e[1]); }
    inline Vector2<T> operator-=(const Vector2<T> other) {
        e[0] -= other.e[0];
        e[1] -= other.e[1];
        return *this;
    }
    inline Vector2<T> operator*(const Vector2<T> other) const { return Vector2<T>(e[0] * other.e[0], e[1] * other.e[1]); }
    inline Vector2<T> operator*=(const Vector2<T> other) {
        e[0] *= other.e[0];
        e[1] *= other.e[1];
        return *this;
    }
    inline Vector2<T> operator*(const f32 scalar) const { return Vector2<T>(e[0] * scalar, e[1] * scalar); }
    inline Vector2<T> operator*=(const f32 scalar) {
        e[0] *= scalar;
        e[1] *= scalar;
        return *this;
    }

    inline T x() const { return e[0]; }
    inline T y() const { return e[1]; }

    inline T dot(const Vector2<T> other) const { return e[0] * other.e[0] + e[1] * other.e[1]; }
    inline T len() const { return std::sqrt(e[0] * e[0] + e[1] * e[1]); }
    inline Vector2<T> norm() const {
        T divisor = 1 / len();
        return Vector2<T>(e[0] * divisor, e[1] * divisor);
    }

    inline Vector2<T> absolute() const { return Vector2<T>(abs(e[0]), abs(e[1])); }
};

template <typename T>
std::ostream& operator<<(std::ostream& out, Vector2<T> const& self) {
    return out << "(" << self.e[0] << ", " << self.e[1] << ")";
}

typedef Vector2<f32> Vector2f;
typedef Vector2<s32> Vector2i;

Vector2f toFloatVec(const Vector2i intVec);
Vector2i toIntVec(const Vector2f floatVec);

// template <typename T>
// Vector2<T> lerp(const Vector2<T>, const Vector2<T>);
Vector2f lerp(const Vector2f, const Vector2f, const f32);

}  // namespace whal
