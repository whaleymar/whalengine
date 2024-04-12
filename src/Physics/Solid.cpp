#include "Physics/Solid.h"

#include <algorithm>
#include <cmath>

#include "Physics/Actor.h"
#include "Physics/Collision/AABB.h"
#include "Physics/CollisionManager.h"
#include "Systems/Deltatime.h"
#include "Util/Vector.h"

namespace whal {

SolidCollider::SolidCollider(Vector2f position, Vector2i half) : IUseCollision(AABB(half)) {
    mCollider.setPosition(position);
}

void SolidCollider::move(f32 x, f32 y) {
    s32 toMoveX = std::round(x);
    s32 toMoveY = std::round(y);
    if (toMoveX == 0 && toMoveY == 0) {
        return;
    }

    // check riding status *before* moving
    auto riding = getRidingActors();

    // turn off collision so actors moved by the solid don't get stuck on it
    mIsCollidable = false;
    if (toMoveX > 0) {
        mCollider.setPosition(mCollider.getPosition() + Vector2i(toMoveX, 0));
        moveDirection(toMoveX, true, mCollider.right(), &AABB::left, riding);
    } else if (toMoveX < 0) {
        mCollider.setPosition(mCollider.getPosition() + Vector2i(toMoveX, 0));
        moveDirection(toMoveX, true, mCollider.left(), &AABB::right, riding);
    }

    if (toMoveY > 0) {
        mCollider.setPosition(mCollider.getPosition() + Vector2i(0, toMoveY));
        moveDirection(toMoveY, false, mCollider.top(), &AABB::bottom, riding);
    } else if (toMoveY < 0) {
        mCollider.setPosition(mCollider.getPosition() + Vector2i(0, toMoveY));
        moveDirection(toMoveY, false, mCollider.bottom(), &AABB::top, riding);
    }

    mIsCollidable = true;
}

void SolidCollider::moveDirection(f32 toMove, bool isXDirection, f32 solidEdge, EdgeGetter edgeFunc, std::vector<ActorCollider*>& riding) {
    f32 dt = Deltatime::getInstance().get();
    for (auto& actor : CollisionManager::getInstance().getAllActors()) {
        // push takes priority over carry
        if (mCollider.isOverlapping(actor->getCollider())) {
            f32 actorEdge = (actor->getCollider().*edgeFunc)();
            toMove = solidEdge - actorEdge;
            actor->moveDirection(isXDirection, toMove, &ActorCollider::squish);
            f32 momentum = toMove / dt;
            actor->setMomentum(momentum, isXDirection);
        } else if (std::find(riding.begin(), riding.end(), actor) != riding.end()) {
            // I might change this for solids moving down faster than gravity RESEARCH
            actor->moveDirection(isXDirection, toMove, nullptr);
            f32 momentum = toMove / dt;
            actor->setMomentum(momentum, isXDirection);
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
