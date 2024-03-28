#pragma once

#include "Util/Types.h"

namespace whal {

// could be interesting:
// time/function for how fast I arrive at top speed
// jump trajectory
// coyote time window

struct PlayerControl {
    PlayerControl(f32 moveSpeed_ = 10, f32 jumpHeight_ = 25);
    f32 moveSpeed;
    f32 jumpHeight;
};

}  // namespace whal
