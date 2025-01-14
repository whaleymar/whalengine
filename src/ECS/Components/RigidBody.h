#pragma once

#include "Util/Types.h"

namespace whal {

// TODO parameters like jumpInitialVelocity, jumpSecondsMax, and coyoteTimeSecondsMax should go in player controller
// actually, most of this data is specific to rigid bodies which are controlled
// but non-player controlled entities can jump too
// so maybe i need a generic Controller component, which takes an input method (player, AI) and has flags for jumping, landing, coyote time, and all
// that

struct RigidBody {
    RigidBody() = default;

    f32 jumpInitialVelocity = 15.5;
    bool isJumping = false;
    bool isLanding = false;
    bool isGrounded = false;
    f32 jumpSecondsRemaining = 0;
    f32 jumpSecondsMax = 1.25;
    f32 coyoteSecondsRemaining = 0;
    f32 coyoteTimeSecondsMax = 0.1;
    // TODO define jump height & derive velocity from that
};

}  // namespace whal
