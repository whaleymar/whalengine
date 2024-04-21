#pragma once

#include "Systems/Deltatime.h"
#include "Systems/Frametracker.h"
#include "Systems/InputHandler.h"
#include "Systems/Random.h"

namespace whal {

struct System {
    inline static InputHandler input;
    inline static Deltatime dt;
    inline static RNG rng;
    inline static Frametracker frame;
};

}  // namespace whal
