#include "PathControl.h"

namespace whal {

PathControl::PathControl(f32 moveSpeed_, std::vector<Position> checkPoints_, f32 waitTime_)
    : moveSpeed(moveSpeed_), checkpoints(checkPoints_), waitTime(waitTime_), curWaitTime(waitTime_) {}

Position PathControl::getTarget() const {
    return checkpoints[curTarget];
}

void PathControl::step() {
    curTarget++;
    if (curTarget == checkpoints.size()) {
        curTarget = 0;
    }
}

}  // namespace whal
