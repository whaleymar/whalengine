#pragma once

#include <vector>

#include "ECS/Components/Position.h"

namespace whal {

// 2 variants I can think of:
// - set speed
// - set time per checkpoint

struct PathControl {
    PathControl(f32 moveSpeed_ = 5, std::vector<Position> checkPoints_ = {});
    f32 moveSpeed;
    std::vector<Position> checkpoints;
    u64 target = 0;

    Position getTarget() const;
    void step();
};

}  // namespace whal
