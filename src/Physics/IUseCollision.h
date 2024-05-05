#pragma once

#include "Physics/Collision/AABB.h"
#include "Physics/Material.h"

namespace whal {

class IUseCollision {
public:
    IUseCollision() = default;
    IUseCollision(AABB collider, Material material = Material::None) : mCollider(collider), mMaterial(material){};
    AABB getCollider() const { return mCollider; }
    Material getMaterial() const { return mMaterial; }
    bool isCollidable() const { return mIsCollidable; }
    void setPosition(Vector2i center) { mCollider.setPosition(center); }
    void setPositionFromBottom(Vector2i bottom) { mCollider.setPositionFromBottom(bottom); }

protected:
    AABB mCollider;
    Material mMaterial;
    bool mIsCollidable = true;
};

}  // namespace whal
