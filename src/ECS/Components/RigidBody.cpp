#include "RigidBody.h"

namespace whal {

RigidBody::RigidBody(f32 jumpInitialVelocity_, f32 jumpSecondsMax_, f32 coyoteTimeSecondsMax_)
    : jumpInitialVelocity(jumpInitialVelocity_), jumpSecondsMax(jumpSecondsMax_), coyoteTimeSecondsMax(coyoteTimeSecondsMax_) {}

}  // namespace whal
