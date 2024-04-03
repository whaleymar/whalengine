#pragma once

#include "Util/Types.h"

namespace whal {

// could be interesting:
// time/function for how fast I arrive at top speed
// jump trajectory
// coyote time window

struct PlayerControlRB {
    PlayerControlRB(f32 moveSpeed_ = 10, f32 jumpHeight_ = 25);
    f32 moveSpeed;
    f32 jumpHeight;  // not used currently
};

struct PlayerControlFree {
    PlayerControlFree(f32 moveSpeed_ = 10);
    f32 moveSpeed;
};

}  // namespace whal
