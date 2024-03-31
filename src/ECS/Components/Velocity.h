#pragma once

#include "Util/Vector.h"

namespace whal {

struct Velocity {
    Vector2f stable;
    Vector2f impulse;
    Vector2f total;  // for tracking true speed

    // immediately going from impulse -> nothing feels very jarring. this is used to have a smoother transition
    Vector2f residualImpulse;
};

}  // namespace whal
