#include "PathControl.h"

namespace whal {

PathControl::PathControl(f32 moveSpeed_, std::vector<Position> checkPoints_) : moveSpeed(moveSpeed_), checkpoints(checkPoints_) {}

Position PathControl::getTarget() const {
    return checkpoints[target];
}

void PathControl::step() {
    target++;
    if (target == checkpoints.size()) {
        target = 0;
    }
}

}  // namespace whal
