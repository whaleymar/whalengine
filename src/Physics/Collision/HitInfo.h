#pragma once

#include "ECS/Lib/ECS.h"
#include "Physics/Material.h"
#include "Util/Vector.h"

namespace whal {

struct AABB;

// enum CollisionInfo : u8 {
//     Hit = 0,
//     Above = 1,
//     Below = 1 << 1,
//     Left = 1 << 2,
//     Right = 1 << 3,
//     Grounded = 1 << 4,
//     HeadBonk = 1 << 5,
//     Solid = 1<<6,
// };

struct HitInfo {
    Vector2i normal;
    ecs::Entity other;
    bool isOtherSolid = false;
    Material otherMaterial = Material::None;

    HitInfo();
    HitInfo(Vector2i normal);
};

}  // namespace whal
