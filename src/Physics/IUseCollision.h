#pragma once

#include "Physics/Collision/AABB.h"

namespace whal {

class IUseCollision {
public:
    IUseCollision(AABB collider) : mCollider(collider){};
    AABB getCollider() const { return mCollider; }
    bool isCollidable() const { return mIsCollidable; }

protected:
    AABB mCollider;
    bool mIsCollidable = true;
};

}  // namespace whal
