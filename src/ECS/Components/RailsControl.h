#pragma once

#include <vector>

#include "ECS/Components/Position.h"

namespace whal {

// 2 variants I can think of (for constant speed)
// - set speed
// - set time per checkpoint

// would need acceleration value as param?

// and an isIdle flag if I want to trigger movement somewhere else

struct RailsControl {
    // TODO
    // this could apply to all checkpoints, or each one could have a unique value
    enum class Movement {
        LINEAR,
        EASEIO_BEZIER,
        EASEIO_SINE,
        EASEI_QUAD,
        EASEI_CUBE,
        EASEO_QUAD,
        EASEO_CUBE,
    };

    struct CheckPoint {
        Vector2i position;
        Movement movement;
    };

    RailsControl(f32 moveSpeed_ = 5, std::vector<Position> checkPoints_ = {}, f32 waitTime_ = 0);

    std::vector<Position> checkpoints;
    f32 moveSpeed;
    f32 waitTime;

    f32 curWaitTime = 0;
    bool isWaiting = false;
    bool isVelocityUpdateNeeded = true;
    bool isCycle = true;
    u64 curTarget = 0;

    Position getTarget() const;
    void step();
};

}  // namespace whal
