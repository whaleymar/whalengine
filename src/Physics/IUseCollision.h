#pragma once

#include "ECS/Lib/ECS.h"
#include "Physics/Collision/AABB.h"
#include "Physics/Material.h"

namespace whal {

class IUseCollision {
public:
    IUseCollision() = default;
    IUseCollision(AABB collider, Material material = Material::None) : mCollider(collider), mMaterial(material){};
    AABB getCollider() const { return mCollider; }
    Material getMaterial() const { return mMaterial; }
    ecs::Entity getEntity() const { return mSelf; }
    bool isCollidable() const { return mIsCollidable; }
    void setPosition(Vector2i center) { mCollider.setPosition(center); }
    void setPositionFromBottom(Vector2i bottom) { mCollider.setPositionFromBottom(bottom); }
    void setEntity(ecs::Entity entity) { mSelf = entity; }

protected:
    AABB mCollider;
    ecs::Entity mSelf;
    Material mMaterial;
    bool mIsTrigger = false;
    bool mIsCollidable = true;
};

}  // namespace whal
