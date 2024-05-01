#include "RelationshipManager.h"

#include "ECS/Components/Relationships.h"
#include "ECS/Components/Transform.h"
#include "Game/Events.h"

namespace whal {

EntityChildSystem::EntityChildSystem() : mEntityDeathListener(&removeEntityFromChildList) {
    System::eventMgr.registerListener(Event::DEATH_EVENT, mEntityDeathListener);
}

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

void removeEntityFromChildList(ecs::Entity entity) {
    for (auto [entityid, parent] : EntityChildSystem::instance()->getEntities()) {
        auto& children = parent.get<Children>();
        auto it = std::find(children.entities.begin(), children.entities.end(), entity);
        if (it != children.entities.end()) {
            children.entities.erase(it);
        }
    }
}

}  // namespace whal
