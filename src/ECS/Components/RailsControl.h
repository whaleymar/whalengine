#pragma once

#include <vector>

#include "Util/Vector.h"

namespace whal {

struct RailsControl {
    enum class Movement {
        LINEAR,
        EASEIO_BEZIER,
        EASEIO_SINE,
        EASEI_QUAD,
        EASEI_CUBE,
    };

    struct CheckPoint {
        Vector2i position;
        Movement movement;
    };

    RailsControl(f32 moveSpeed_ = 5, std::vector<CheckPoint> checkPoints_ = {}, f32 waitTime_ = 0, bool isCycle_ = true);

private:
    std::vector<CheckPoint> mCheckpoints;

public:
    f32 speed;  // tiles per second
    f32 waitTime;

    Vector2f startPosition;
    f32 curActionTime = 0;  // time spent moving or waiting
    bool isCycle;           // if true, repeats after returning to first checkpoint
    bool isWaiting = true;
    bool isVelocityUpdateNeeded = false;
    u64 curTarget = 0;

    CheckPoint getTarget() const;
    void startManually();
    void step();
    f32 getSpeed(Vector2i currentPosition, f32 inv_dt);
    bool isValid() const;
};

}  // namespace whal
