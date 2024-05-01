#include "Relationships.h"

#include "ECS/Components/Draw.h"
#include "ECS/Components/Transform.h"
#include "Gfx/Color.h"

namespace whal {

Follow::Follow(ecs::Entity target_, ecs::Entity self) : targetEntity(target_), currentTarget(target_.get<Transform>().position) {
    if (targetEntity.has<Children>()) {
        targetEntity.get<Children>().entities.push_back(self);
    } else {
        targetEntity.add(Children({self}));
    }
    auto eOpt = ecs::ECS::getInstance().entity();
    if (eOpt.isExpected()) {
        debugTargetTracker = eOpt.value();
        debugTargetTracker.add<Transform>();
        debugTargetTracker.add(Draw(Color::EMERALD));
        targetEntity.get<Children>().entities.push_back(debugTargetTracker);
    }

    auto eOpt2 = ecs::ECS::getInstance().entity();
    if (eOpt2.isExpected()) {
        debugPositionTracker = eOpt2.value();
        debugPositionTracker.add<Transform>();
        debugPositionTracker.add(Draw(Color::MAGENTA));
        targetEntity.get<Children>().entities.push_back(debugPositionTracker);
    }
}

}  // namespace whal
