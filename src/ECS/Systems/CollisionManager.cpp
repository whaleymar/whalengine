#include "CollisionManagerSystem.h"

#include "Physics/CollisionManager.h"

namespace whal {

// this is a really dumb way of making the collision manager be correct
// but i've tried like 5 other things and at this point I just want it to work
// something I could try: give an onAdd and onRemove vfunc to ISystem and use those here

void RigidBodyManager::update() {
    auto& mgr = CollisionManager::getInstance();
    mgr.clearActors();

    for (auto const& [entityid, entity] : getEntities()) {
        auto ptr = &entity.get<RigidBody>().collider;
        mgr.add(ptr);
    }
}

void SolidBodyManager::update() {
    auto& mgr = CollisionManager::getInstance();
    mgr.clearSolids();

    for (auto const& [entityid, entity] : getEntities()) {
        auto ptr = &entity.get<SolidBody>().collider;
        mgr.add(ptr);
    }
}

}  // namespace whal
