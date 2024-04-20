#pragma once

#include "Util/Vector.h"

namespace whal {

struct AABB;

// candidates:
// - destroyed (only happens when others move)
//
// enum CollisionInfo : u8 {
//     None = 0,
//     Grounded = 1,
//     HeadBonk = 1 << 1,
// };

struct HitInfo {
    Vector2i normal;
    AABB* other = nullptr;

    HitInfo();
    HitInfo(Vector2i normal, AABB* other);
};

}  // namespace whal
