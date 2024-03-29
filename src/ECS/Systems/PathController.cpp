#include "PathController.h"

#include "ECS/Components/PathControl.h"
#include "ECS/Components/Position.h"
#include "ECS/Components/Velocity.h"

#include "Util/MathUtil.h"
#include "Util/Print.h"
#include "Util/Vector.h"

namespace whal {

void PathControllerSystem::update() {
    for (auto& [entityid, entity] : getEntities()) {
        auto& path = entity.get<PathControl>();
        auto& position = entity.get<Position>();

        Vector2f delta = toFloatVec(path.getTarget().e - position.e);
        if (isNearZero(delta.len())) {
            print("setting next checkpoint for entity", entityid);
            path.step();
            entity.set<Velocity>(Velocity());
            continue;
        }

        entity.set<Velocity>(Velocity(delta.norm() * path.moveSpeed));
    }
}

}  // namespace whal
