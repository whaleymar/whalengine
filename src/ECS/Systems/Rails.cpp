#include "Rails.h"

#include "ECS/Components/Position.h"
#include "ECS/Components/RailsControl.h"
#include "ECS/Components/Velocity.h"

#include "Systems/Deltatime.h"
#include "Util/Vector.h"

namespace whal {

constexpr f32 CHECKPOINT_DISTANCE_THRESHOLD = 5;  // in pixels

void RailsSystem::update() {
    f32 dt = Deltatime::getInstance().get();
    for (auto& [entityid, entity] : getEntities()) {
        auto& rails = entity.get<RailsControl>();
        auto& position = entity.get<Position>();

        Vector2f delta = toFloatVec(rails.getTarget().e - position.e);

        // scale checkpoint threshold with speed
        f32 speed = entity.get<Velocity>().stable.len();
        f32 epsilon = speed > 0 ? CHECKPOINT_DISTANCE_THRESHOLD * speed : CHECKPOINT_DISTANCE_THRESHOLD;

        if (rails.isWaiting || delta.len() < epsilon) {
            if (rails.isWaiting && rails.curWaitTime <= 0) {
                rails.step();
                rails.isVelocityUpdateNeeded = true;
                rails.isWaiting = false;
                rails.curWaitTime = rails.waitTime;
            } else if (rails.isWaiting) {
                rails.curWaitTime -= dt;
            } else {
                entity.set<Velocity>(Velocity());
                rails.isWaiting = true;
            }
        } else if (rails.isVelocityUpdateNeeded) {
            entity.set<Velocity>(Velocity(delta.norm() * rails.moveSpeed));
            rails.isVelocityUpdateNeeded = false;
        }
    }
}

}  // namespace whal
