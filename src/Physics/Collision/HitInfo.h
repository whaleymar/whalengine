#pragma once

#include "Util/Vector.h"

namespace whal {

struct AABB;

struct HitInfo {
    Vector2i pos;  // definitely don't need this ?
    Vector2i normal;
    AABB* other = nullptr;

    HitInfo();
    HitInfo(Vector2i position, Vector2i normal, AABB* other);
};

}  // namespace whal
