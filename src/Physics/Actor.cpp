#include "Physics/Actor.h"

#include <cmath>
#include <type_traits>

#include "Physics/CollisionManager.h"
#include "Physics/IUseCollision.h"
#include "Physics/Solid.h"
#include "Util/MathUtil.h"

namespace whal {

ActorCollider::ActorCollider(Vector2f position, Vector2i half) : IUseCollision(AABB(half)) {
    mCollider.setPosition(position);
}

void ActorCollider::moveDirection(const bool isXDirection, const f32 amount, const CollisionCallback callback) {
    // TODO doesn't handle colliding with other actors
    s32 toMove = std::round(amount);
    if (toMove == 0) {
        return;
    }

    auto& solids = CollisionManager::getInstance().getAllSolids();
    s32 moveSign = sign(toMove);
    auto moveVector = isXDirection ? Vector2i(moveSign, 0) : Vector2i(0, moveSign);
    while (toMove != 0) {
        auto nextPos = mCollider.getPosition() + moveVector;
        auto hitInfo = checkCollision(solids, nextPos);
        if (hitInfo == std::nullopt) {
            mCollider.setPosition(nextPos);
            toMove -= moveSign;
        } else {
            if (callback != nullptr) {
                ((*this).*callback)(hitInfo.value());
            }
            break;
        }
    }
}

template <typename T>
std::optional<HitInfo> ActorCollider::checkCollision(const std::vector<T*>& objects, const Vector2i position) const {
    static_assert(std::is_base_of_v<IUseCollision, T>, "collider must inherit IUseCollision");

    auto movedCollider = AABB(position, mCollider.mHalf);
    for (auto& object : objects) {
        if (!object->isCollidable()) {
            continue;
        }
        std::optional<HitInfo> hitInfo = movedCollider.collide(object->getCollider());
        if (hitInfo != std::nullopt) {
            return hitInfo;
        }
    }

    return std::nullopt;
}

void ActorCollider::squish(const HitInfo hitInfo) {
    mIsAlive = false;
}

bool ActorCollider::isRiding(const SolidCollider* solid) const {
    // check for collision 1 unit down
    auto movedCollider = AABB(mCollider.mCenter + Vector::unitiDown, mCollider.mHalf);
    if (movedCollider.collide(solid->getCollider()) != std::nullopt) {
        return true;
    }
    return false;
}

}  // namespace whal
