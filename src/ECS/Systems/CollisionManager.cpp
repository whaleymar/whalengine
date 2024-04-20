#include "CollisionManager.h"

#include "ECS/Components/RigidBody.h"
#include "ECS/Components/SolidBody.h"
#include "Util/Print.h"

namespace whal {

void RigidBodyManager::update() {
    if (mIsUpdateNeeded == true) {
        print("Updating actors list");
    }
    std::vector<ActorCollider*> newActorList;

    bool isUpdateNeeded = mIsUpdateNeeded;
    mIsUpdateNeeded = false;  // might change if we kill entities

    for (auto& [entityid, entity] : getEntities()) {
        auto pCollider = &entity.get<RigidBody>().collider;
        if (!pCollider->isAlive()) {
            entity.kill();
        } else if (isUpdateNeeded) {
            newActorList.push_back(pCollider);
        }
    }

    if (isUpdateNeeded) {
        mActors = newActorList;
    }
}

void RigidBodyManager::onAdd(ecs::Entity entity) {
    mIsUpdateNeeded = true;
}

void RigidBodyManager::onRemove(ecs::Entity entity) {
    mIsUpdateNeeded = true;
}

const std::vector<ActorCollider*>& RigidBodyManager::getAllActors() const {
    return mActors;
}

void SolidBodyManager::update() {
    if (!mIsUpdateNeeded) {
        return;
    }
    print("updating solids list");
    std::vector<SolidCollider*> newSolidList;
    for (auto& [entityid, entity] : getEntities()) {
        auto pCollider = &entity.get<SolidBody>().collider;
        newSolidList.push_back(pCollider);
    }
    mSolids = newSolidList;
    mIsUpdateNeeded = false;
}

void SolidBodyManager::onAdd(ecs::Entity entity) {
    mIsUpdateNeeded = true;
}

void SolidBodyManager::onRemove(ecs::Entity entity) {
    mIsUpdateNeeded = true;
}

const std::vector<SolidCollider*>& SolidBodyManager::getAllSolids() const {
    return mSolids;
}

}  // namespace whal
