#include "RelationshipManager.h"

#include "ECS/Components/Relationships.h"
#include "ECS/Components/Transform.h"

namespace whal {

void EntityChildSystem::onRemove(ecs::Entity entity) {
    std::vector<ecs::Entity> childrencopy = std::move(entity.get<Children>().entities);
    for (auto childEntity : childrencopy) {
        childEntity.kill();
    }
}

void FollowSystem::update() {
    for (auto [entityid, entity] : getEntities()) {
        auto& follow = entity.get<Follow>();
        Transform targetTransCopy = follow.target.get<Transform>();
        targetTransCopy.isManuallyMoved = true;
        entity.set(targetTransCopy);
    }
}

}  // namespace whal
