#include "RailsControl.h"

namespace whal {

RailsControl::RailsControl(f32 moveSpeed_, std::vector<Position> checkPoints_, f32 waitTime_, bool isCycle_)
    : mCheckpoints(checkPoints_), maxSpeed(moveSpeed_), waitTime(waitTime_), curActionTime(waitTime_), isCycle(isCycle_) {
    if (checkPoints_.size() > 0) {
        curTarget = 1;
    }
}

Position RailsControl::getTarget() const {
    return mCheckpoints[curTarget];
}

void RailsControl::step() {
    curTarget++;
    if (curTarget == mCheckpoints.size()) {
        curTarget = 0;
    }
}

}  // namespace whal
