#pragma once

#include "Gfx/GfxUtil.h"
#include "Physics/Actor.h"

namespace whal {

// TODO parameters like jumpInitialVelocity, jumpSecondsMax, and coyoteTimeSecondsMax should go in player controller

struct RigidBody {
    RigidBody(s32 half_ = TILE_LEN_PIXELS);
    RigidBody(Vector2f center_, s32 halfx, s32 halfy);

    ActorCollider collider;
    f32 jumpInitialVelocity = 15.5;
    bool isJumping = false;
    f32 jumpSecondsRemaining = 0;
    f32 jumpSecondsMax = 1.25;
    f32 coyoteSecondsRemaining = 0;
    f32 coyoteTimeSecondsMax = 0.1;
    // TODO define jump height & derive velocity from that
};

}  // namespace whal
