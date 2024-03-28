#include "SolidBody.h"

#include "Physics/CollisionManager.h"
#include "Util/Vector.h"

namespace whal {

SolidBody::SolidBody(s32 half_) : collider(Vector2f(), Vector2i(half_, half_)) {
    // CollisionManager::getInstance().add(&collider);
    // onAdd = [this]() { CollisionManager::getInstance().add(&collider); };
    // onRemove = [this]() { CollisionManager::getInstance().remove(&collider); };
}

SolidBody::SolidBody(Vector2f center, s32 halfx, s32 halfy) : collider(center, Vector2i(halfx, halfy)) {
    // CollisionManager::getInstance().add(&collider);
    // onAdd = [this]() { CollisionManager::getInstance().add(&collider); };
    // onRemove = [this]() { CollisionManager::getInstance().remove(&collider); };
}

// void SolidBody::onAdd() {
//     CollisionManager::getInstance().add(&collider);
// }
//
// void SolidBody::onRemove() {
//     CollisionManager::getInstance().remove(&collider);
// }

}  // namespace whal
