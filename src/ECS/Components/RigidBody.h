#pragma once

#include "Gfx/GfxUtil.h"
#include "Physics/Actor.h"

namespace whal {

struct RigidBody {
    RigidBody(s32 half_ = TILE_LEN_PIXELS);
    RigidBody(Vector2f center_, s32 halfx, s32 halfy);

    ActorCollider collider;
    Vector2f inputVelocity;
    bool isJumping = false;
    f32 jumpSecondsRemaining = 0;
    f32 jumpSecondsMax = 0.5;
    // TODO define jump height & derive velocity from that
};

}  // namespace whal
