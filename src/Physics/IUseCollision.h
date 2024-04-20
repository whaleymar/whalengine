#pragma once

#include "Physics/Collision/AABB.h"

namespace whal {

class IUseCollision {
public:
    IUseCollision() = default;
    IUseCollision(AABB collider) : mCollider(collider){};
    AABB getCollider() const { return mCollider; }
    bool isCollidable() const { return mIsCollidable; }
    void setPosition(Vector2i center) { mCollider.setPosition(center); }
    void setPositionFromBottom(Vector2i bottom) { mCollider.setPositionFromBottom(bottom); }

protected:
    AABB mCollider;
    bool mIsCollidable = true;
};

}  // namespace whal
