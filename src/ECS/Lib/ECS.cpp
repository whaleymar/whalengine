#include "ECS/Lib/ECS.h"

namespace whal::ecs {

ECS::ECS() {
    mComponentManager = std::make_unique<ComponentManager>();
    mEntityManager = std::make_unique<EntityManager>();
    mSystemManager = std::make_unique<SystemManager>();
}

Expected<Entity> ECS::entity() const {
    return mEntityManager->createEntity();
}

void ECS::kill(Entity entity) const {
    mEntityManager->destroyEntity(entity);
    mComponentManager->entityDestroyed(entity);
    mSystemManager->entityDestroyed(entity);
}

}  // namespace whal::ecs
