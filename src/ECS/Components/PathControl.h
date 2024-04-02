#pragma once

#include <vector>

#include "ECS/Components/Position.h"

namespace whal {

// 2 variants I can think of:
// - set speed
// - set time per checkpoint

// TODO
enum class Movement {
    CONSTANT_SPEED,
    ACCEL_UNTIL_TIME,  // accelerate for X seconds
    ACCEL_UNTIL_SPEED  // accelerate until speed reached
};

struct PathControl {
    PathControl(f32 moveSpeed_ = 5, std::vector<Position> checkPoints_ = {}, f32 waitTime_ = 0);

    f32 moveSpeed;
    std::vector<Position> checkpoints;
    f32 waitTime;

    u64 curTarget = 0;
    f32 curWaitTime = 0;
    bool isVelocityUpdateNeeded = true;

    Position getTarget() const;
    void step();
};

}  // namespace whal
