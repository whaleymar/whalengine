#pragma once

#include "Util/Vector.h"

namespace whal {

struct HitInfo {
    Vector2i mPos;
    Vector2i mDelta;
    Vector2i mNormal;

    HitInfo();
    HitInfo(Vector2i position, Vector2i delta, Vector2i normal);
};

}  // namespace whal
