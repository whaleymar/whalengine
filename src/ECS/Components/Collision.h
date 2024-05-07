#pragma once

#include <vector>

#include "Physics/IUseCollision.h"
#include "Physics/Material.h"
#include "Util/Vector.h"

namespace whal {

class ActorCollider;
struct HitInfo;
namespace ecs {
class Entity;
}

using ActorCollisionCallback = void (*)(ActorCollider* selfCollider, ecs::Entity actorEntity, HitInfo hitinfo);

enum class CollisionDir : u8 { ALL, LEFT, RIGHT, DOWN, UP };

class SolidCollider : public IUseCollision {
public:
    SolidCollider() = default;
    SolidCollider(Vector2f position, Vector2i half, Material material = Material::None, ActorCollisionCallback onCollisionEnter_ = nullptr,
                  CollisionDir collisionDir = CollisionDir::ALL);

    void move(f32 x, f32 y, bool isManualMove = false);
    std::vector<ActorCollider*> getRidingActors() const;
    void setCollisionCallback(ActorCollisionCallback callback);
    bool isGround() const;
    ActorCollisionCallback getOnCollisionEnter() const { return mOnCollisionEnter; }
    CollisionDir getCollisionDir() const { return mCollisionDir; }
    void setCollisionDir(CollisionDir dir) { mCollisionDir = dir; }

private:
    void moveDirection(f32 toMoveRounded, f32 toMoveUnrounded, bool isXDirection, f32 solidEdge, EdgeGetter edgeFunc,
                       std::vector<ActorCollider*>& riding, bool isManualMove);
    CollisionDir mCollisionDir;
    ActorCollisionCallback mOnCollisionEnter;
};

// TODO material in constructor
class ActorCollider : public IUseCollision {
public:
    ActorCollider() = default;
    ActorCollider(Vector2f position, Vector2i half);

    std::optional<HitInfo> moveX(const Vector2f amount, const ActorCollisionCallback callback);
    std::optional<HitInfo> moveY(const Vector2f amount, const ActorCollisionCallback callback);
    void setMomentum(const f32 momentum, const bool isXDirection);
    void addMomentum(const f32 momentum, const bool isXDirection);
    void maintainMomentum(const bool isXDirection);
    void resetMomentumX();
    void resetMomentumY();
    void resetMomentum();
    Vector2f getMomentum() const { return mStoredMomentum; }
    bool isMomentumStoredX() const { return mMomentumFramesLeft.x() > 0; }
    bool isMomentumStoredY() const { return mMomentumFramesLeft.y() > 0; }
    void momentumNotUsed();
    bool checkIsGrounded(const std::vector<SolidCollider*>& solids, IUseCollision** groundCollider);
    bool checkIsGroundedOnActors(const std::vector<ActorCollider*>& actors, IUseCollision** groundCollider);

    std::optional<HitInfo> checkCollisionSolids(const std::vector<SolidCollider*>& solids, const Vector2i position, const Vector2i moveNormal) const;
    std::optional<HitInfo> checkCollisionActors(const std::vector<ActorCollider*>& actors, const Vector2i position) const;

    // squish is a CollisionCallback. Not sure if I will define others
    // TODO these can't be virtual bc ECS
    virtual void squish(const HitInfo hitInfo);
    virtual bool isRiding(const SolidCollider* solid) const;

private:
    bool tryCornerCorrection(const std::vector<SolidCollider*>& solids, Vector2i nextPos, s32 moveSign, Vector2i moveNormal);

    Vector2f mStoredMomentum;
    Vector2i mMomentumFramesLeft = {0, 0};
    // f32 mMass = 1; // could give solids a mass and use mass ratio to calculate force
};

void defaultSquish(ActorCollider* selfCollider, ecs::Entity self, HitInfo hitinfo);

}  // namespace whal
