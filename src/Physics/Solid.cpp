#include "Physics/Solid.h"

#include <algorithm>
#include "Physics/Collision/AABB.h"
#include "Physics/CollisionManager.h"
#include "Util/Vector.h"

namespace whal {

SolidCollider::SolidCollider(Vector2f position, Vector2i half) : IUseCollision(AABB(half)) {
    mCollider.setPosition(position);
}

void SolidCollider::move(f32 x, f32 y) {
    s32 toMoveX = static_cast<s32>(x);
    s32 toMoveY = static_cast<s32>(y);
    if (toMoveX == 0 && toMoveY == 0) {
        return;
    }

    auto riding = getRidingActors();

    // turn off collision so actors moved by the solid don't get stuck on it
    mIsCollidable = false;
    if (toMoveX > 0) {
        moveDirection(toMoveX, Vector2i(toMoveX, 0), mCollider.right(), &AABB::left, riding);
    } else if (toMoveX < 0) {
        moveDirection(toMoveX, Vector2i(toMoveX, 0), mCollider.left(), &AABB::right, riding);
    }

    if (toMoveY > 0) {
        moveDirection(toMoveY, Vector2i(0, toMoveY), mCollider.top(), &AABB::bottom, riding);
    } else if (toMoveY < 0) {
        moveDirection(toMoveY, Vector2i(0, toMoveY), mCollider.bottom(), &AABB::top, riding);
    }

    mIsCollidable = true;
}

void SolidCollider::moveDirection(f32 toMove, Vector2i toMoveVec, f32 solidEdge, EdgeGetter edgeFunc, std::vector<ActorCollider*>& riding) {
    mCollider.setPosition(mCollider.getPosition() + toMoveVec);
    bool isXDirection = toMoveVec.x() != 0;

    for (auto& actor : CollisionManager::getInstance().getAllActors()) {
        // push takes priority over carry
        if (mCollider.isOverlapping(actor->getCollider())) {
            f32 actorEdge = (actor->getCollider().*edgeFunc)();
            actor->moveDirection(isXDirection, solidEdge - actorEdge, &ActorCollider::squish);
        } else if (std::find(riding.begin(), riding.end(), actor) != riding.end()) {
            actor->moveDirection(isXDirection, toMove, nullptr);
        }
    }
}

std::vector<ActorCollider*> SolidCollider::getRidingActors() const {
    std::vector<ActorCollider*> riding;
    for (auto& actor : CollisionManager::getInstance().getAllActors()) {
        if (actor->isRiding(this)) {
            riding.push_back(actor);
        }
    }
    return riding;
}

}  // namespace whal
