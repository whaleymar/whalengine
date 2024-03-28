#include "RigidBody.h"

#include "Physics/CollisionManager.h"
#include "Util/Vector.h"

namespace whal {

RigidBody::RigidBody(s32 half_) : collider(Vector2f(), Vector2i(half_, half_)) {
    // TODO bad bad bad; getting added for entire component array
    // CollisionManager::getInstance().add(&collider);
    // onAdd = [this]() { CollisionManager::getInstance().add(&collider); };
    // onRemove = [this]() { CollisionManager::getInstance().remove(&collider); };
}

RigidBody::RigidBody(Vector2f center, s32 halfx, s32 halfy) : collider(center, Vector2i(halfx, halfy)) {
    // CollisionManager::getInstance().add(&collider);
    // onAdd = [this]() { CollisionManager::getInstance().add(&collider); };
    // onRemove = [this]() { CollisionManager::getInstance().remove(&collider); };
}

// void RigidBody::onAdd() {
//     CollisionManager::getInstance().add(&collider);
// }
//
// void RigidBody::onRemove() {
//     CollisionManager::getInstance().remove(&collider);
// }

}  // namespace whal
