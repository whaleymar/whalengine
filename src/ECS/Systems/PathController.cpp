#include "PathController.h"

#include "ECS/Components/PathControl.h"
#include "ECS/Components/Position.h"
#include "ECS/Components/Velocity.h"

#include "Systems/Deltatime.h"
#include "Util/Vector.h"

namespace whal {

constexpr f32 CHECKPOINT_DISTANCE_THRESHOLD = 5;  // in pixels

void PathControllerSystem::update() {
    f32 dt = Deltatime::getInstance().get();
    for (auto& [entityid, entity] : getEntities()) {
        auto& path = entity.get<PathControl>();
        auto& position = entity.get<Position>();

        Vector2f delta = toFloatVec(path.getTarget().e - position.e);

        // scale checkpoint threshold with speed
        f32 speed = entity.get<Velocity>().stable.len();
        f32 epsilon = speed > 0 ? CHECKPOINT_DISTANCE_THRESHOLD * speed : CHECKPOINT_DISTANCE_THRESHOLD;
        if (path.isWaiting || delta.len() < epsilon) {
            if (path.isWaiting && path.curWaitTime <= 0) {
                path.step();
                path.isVelocityUpdateNeeded = true;
                path.isWaiting = false;
                path.curWaitTime = path.waitTime;
            } else if (path.isWaiting) {
                path.curWaitTime -= dt;
            } else {
                entity.set<Velocity>(Velocity());
                path.isWaiting = true;
            }
        } else if (path.isVelocityUpdateNeeded) {
            entity.set<Velocity>(Velocity(delta.norm() * path.moveSpeed));
            path.isVelocityUpdateNeeded = false;
        }
    }
}

}  // namespace whal
