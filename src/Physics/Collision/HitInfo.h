#pragma once

#include "Physics/Material.h"
#include "Util/Vector.h"

namespace whal {

struct AABB;

// enum CollisionInfo : u8 {
//     None = 0,
//     Above = 1,
//     Below = 1 << 1,
//     Left = 1 << 2,
//     Right = 1 << 3,
//     Grounded = 1 << 4,
//     HeadBonk = 1 << 5,
// };

struct HitInfo {
    Vector2i normal;
    AABB* other = nullptr;
    Material otherMaterial = Material::None;

    HitInfo();
    HitInfo(Vector2i normal, AABB* other);
};

}  // namespace whal
