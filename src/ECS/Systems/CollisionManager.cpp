#include "CollisionManagerSystem.h"

#include "Physics/CollisionManager.h"

namespace whal {

void RigidBodyManager::update() {
    for (auto& [entityid, entity] : getEntities()) {
        if (!entity.get<RigidBody>().collider.isAlive()) {
            entity.kill();
        }
    }
}

void RigidBodyManager::onAdd(ecs::Entity entity) {
    auto pCollider = &entity.get<RigidBody>().collider;
    CollisionManager::getInstance().add(pCollider);
}

void RigidBodyManager::onRemove(ecs::Entity entity) {
    auto pCollider = &entity.get<RigidBody>().collider;
    CollisionManager::getInstance().remove(pCollider);
}

void SolidBodyManager::onAdd(ecs::Entity entity) {
    auto pCollider = &entity.get<SolidBody>().collider;
    CollisionManager::getInstance().add(pCollider);
}

void SolidBodyManager::onRemove(ecs::Entity entity) {
    auto pCollider = &entity.get<SolidBody>().collider;
    CollisionManager::getInstance().remove(pCollider);
}

}  // namespace whal
