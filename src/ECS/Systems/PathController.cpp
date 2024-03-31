#include "PathController.h"

#include "ECS/Components/PathControl.h"
#include "ECS/Components/Position.h"
#include "ECS/Components/Velocity.h"

#include "Util/Vector.h"

namespace whal {

constexpr f32 CHECKPOINT_DISTANCE_THRESHOLD = 1.0;  // this is pretty big because velocity is currently in pixels/sec

void PathControllerSystem::update() {
    for (auto& [entityid, entity] : getEntities()) {
        auto& path = entity.get<PathControl>();
        auto& position = entity.get<Position>();

        Vector2f delta = toFloatVec(path.getTarget().e - position.e);

        // scale checkpoint threshold with speed
        f32 speed = entity.get<Velocity>().vel.len();
        f32 epsilon = speed > 0 ? CHECKPOINT_DISTANCE_THRESHOLD * speed : CHECKPOINT_DISTANCE_THRESHOLD;
        if (delta.len() < epsilon) {
            path.step();
            entity.set<Velocity>(Velocity());
            continue;
        }

        entity.set<Velocity>(Velocity(delta.norm() * path.moveSpeed));
    }
}

}  // namespace whal
