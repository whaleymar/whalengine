#pragma once

#include <vector>

#include "ECS/Components/Position.h"

namespace whal {

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

    RailsControl(f32 moveSpeed_ = 5, std::vector<Position> checkPoints_ = {}, f32 waitTime_ = 0, bool isCycle_ = true);

private:
    std::vector<Position> mCheckpoints;

public:
    f32 maxSpeed;
    f32 waitTime;

    f32 curActionTime = 0;  // time spent moving or waiting
    f32 invSegmentDistance = 0;
    bool isCycle;  // if true, repeats after returning to first checkpoint
    bool isWaiting = false;
    bool isMoving = false;
    bool isVelocityUpdateNeeded = false;
    u64 curTarget = 0;

    Position getTarget() const;
    void step();
};

}  // namespace whal
