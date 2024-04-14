#include "RailsControl.h"

namespace whal {

RailsControl::RailsControl(f32 moveSpeed_, std::vector<Position> checkPoints_, f32 waitTime_)
    : checkpoints(checkPoints_), moveSpeed(moveSpeed_), waitTime(waitTime_), curWaitTime(waitTime_) {}

Position RailsControl::getTarget() const {
    return checkpoints[curTarget];
}

void RailsControl::step() {
    curTarget++;
    if (curTarget == checkpoints.size()) {
        curTarget = 0;
    }
}

}  // namespace whal
