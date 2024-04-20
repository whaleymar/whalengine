#pragma once

#include <vector>

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
    bool isAlive() const { return mIsAlive; }
    bool isGrounded() const { return mIsGrounded; }
    void setGrounded(const bool grounded) { mIsGrounded = grounded; }
    void setMomentum(const f32 momentum, const bool isXDirection);
    void addMomentum(const f32 momentum, const bool isXDirection);
    void resetMomentum();
    Vector2f getMomentum() const { return mStoredMomentum; }
    bool isMomentumStored() const { return mMomentumFramesLeft > 0; }
    void momentumNotUsed();
    void checkIsGrounded(const std::vector<SolidCollider*>& solids);

    template <typename T>
    std::optional<HitInfo> checkCollision(const std::vector<T*>& objects, const Vector2i position) const;

    // squish is a CollisionCallback. Not sure if I will define others
    virtual void squish(const HitInfo hitInfo);
    virtual bool isRiding(const SolidCollider* solid) const;

private:
    bool tryCornerCorrection(const std::vector<SolidCollider*>& solids, Vector2i nextPos, s32 moveSign);

    Vector2f mStoredMomentum;
    s32 mMomentumFramesLeft = 0;
    // f32 mMass = 1; // could give solids a mass and use mass ratio to calculate force
    bool mIsAlive = true;
    bool mIsGrounded = false;
};

}  // namespace whal
