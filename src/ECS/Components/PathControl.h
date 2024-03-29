#pragma once

#include <vector>

#include "ECS/Components/Position.h"

namespace whal {

// 2 variants I can think of:
// - set speed
// - set time per checkpoint

struct PathControl {
    std::vector<Position> checkpoints;
    f32 moveSpeed;
    s32 checkpointIx;
};

}  // namespace whal
