#include "Util/Vector.h"

namespace whal {

Vector2f toFloatVec(const Vector2i intVec) {
    return Vector2f(static_cast<f32>(intVec.x()), static_cast<f32>(intVec.y()));
}

Vector2i toIntVec(const Vector2f floatVec) {
    return Vector2i(static_cast<s32>(floatVec.x()), static_cast<s32>(floatVec.y()));
}

// template <typename T>
// Vector2<T> lerp(const Vector2<T>, const Vector2<T>) {
//     return Vector2<T>()
// }
Vector2f lerp(const Vector2f vec1, const Vector2f vec2, const f32 t) {
    return Vector2f(lerp(vec1.x(), vec2.x(), t), lerp(vec1.y(), vec2.y(), t));
}

}  // namespace whal
