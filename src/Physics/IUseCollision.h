#pragma once

#include "Physics/Collision/AABB.h"

namespace whal {

class IUseCollision {
public:
    IUseCollision(AABB collider) : mCollider(collider){};
    AABB getCollider() const { return mCollider; }
    bool isCollidable() const { return mIsCollidable; }
    bool wasMovedManually() const { return mWasMovedManually; }
    void manualMoveComplete() { mWasMovedManually = false; }
    void setPosition(Vector2i center) { mCollider.setPosition(center); }
    void setPositionFromBottom(Vector2i bottom) { mCollider.setPositionFromBottom(bottom); }

protected:
    AABB mCollider;
    bool mIsCollidable = true;
    bool mWasMovedManually = false;
};

}  // namespace whal
