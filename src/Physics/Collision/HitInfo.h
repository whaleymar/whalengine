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
//     SemiSolid
// };

// once i hit 17 bytes I'll use the bitfield
struct HitInfo {
    Vector2i normal;
    ecs::Entity other;
    bool isOtherSolid = false;
    bool isOtherSemiSolid = false;
    bool isOtherActor = false;
    Material otherMaterial = Material::None;

    HitInfo();
    HitInfo(Vector2i normal);
};

}  // namespace whal
