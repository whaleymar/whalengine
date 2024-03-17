#pragma once

#include <vector>

// #include "Physics/Collision/AABB.h"
#include "Physics/Collision/HitInfo.h"
#include "Physics/IUseCollision.h"
#include "Util/Types.h"
#include "Util/Vector.h"

namespace whal {

class SolidCollider;
class ActorCollider;

using CollisionCallback = void (ActorCollider::*)(HitInfo);

class ActorCollider : public IUseCollision {
public:
    ActorCollider(Vector2f position, Vector2i half);

    void moveDirection(const bool isXDirection, const f32 amount, const CollisionCallback callback);

    template <typename T>
    std::optional<HitInfo> checkCollision(const std::vector<T*>& objects, const Vector2i position) const;

    virtual void squish(const HitInfo hitInfo);
    virtual bool isRiding(const SolidCollider* solid) const;

private:
    // TODO store pointer to parent entity, then send kill/move signal from here?
    bool mIsAlive = true;
};

}  // namespace whal
