#pragma once

#include <vector>

#include "Physics/IUseCollision.h"
#include "Physics/Material.h"
#include "Util/Vector.h"

namespace whal {

class ActorCollider;
class SolidCollider;
class SemiSolidCollider;
struct HitInfo;
namespace ecs {
class Entity;
}

// TODO consolidate these using IUseCollision* for first arg
using ActorCollisionCallback = void (*)(ActorCollider* selfCollider, ecs::Entity actorEntity, HitInfo hitinfo);
using SemiSolidCollisionCallback = void (*)(SemiSolidCollider* selfCollider, ecs::Entity ssEntity, HitInfo hitinfo);

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
    bool checkIsGroundedOnSemiSolids(const std::vector<SemiSolidCollider*>& semis, IUseCollision** groundCollider);
    bool checkIsGroundedOnActors(const std::vector<ActorCollider*>& actors, IUseCollision** groundCollider);

    std::optional<HitInfo> checkCollisionSolids(const std::vector<SolidCollider*>& solids, const Vector2i position, const Vector2i moveNormal) const;
    std::optional<HitInfo> checkCollisionSemiSolids(const std::vector<SemiSolidCollider*>& solids, const Vector2i position) const;
    std::optional<HitInfo> checkCollisionActors(const std::vector<ActorCollider*>& actors, const Vector2i position) const;

    // squish is a CollisionCallback. Not sure if I will define others
    // TODO these can't be virtual bc ECS
    virtual void squish(const HitInfo hitInfo);
    virtual bool isRiding(const SolidCollider* solid) const;

private:
    bool tryCornerCorrection(const std::vector<SolidCollider*>& solids, Vector2i nextPos, s32 moveSignX, Vector2i moveNormal);
    bool tryCornerCorrectionSemiSolids(const std::vector<SemiSolidCollider*>& semis, Vector2i nextPos, s32 moveSignX);

    Vector2f mStoredMomentum = {0, 0};
    Vector2i mMomentumFramesLeft = {0, 0};
    // f32 mMass = 1; // could give solids a mass and use mass ratio to calculate force
};

class SolidCollider : public IUseCollision {
public:
    SolidCollider() = default;
    SolidCollider(Vector2f position, Vector2i half, Material material = Material::None, ActorCollisionCallback onCollisionEnter_ = nullptr,
                  CollisionDir collisionDir = CollisionDir::ALL);

    void move(f32 x, f32 y, bool isManualMove = false);
    std::vector<ActorCollider*> getRidingActors() const;
    std::vector<SemiSolidCollider*> getRidingSemiSolids() const;
    void setCollisionCallback(ActorCollisionCallback callback);
    bool isGround() const;
    ActorCollisionCallback getOnCollisionEnter() const { return mOnCollisionEnter; }
    CollisionDir getCollisionDir() const { return mCollisionDir; }
    void setCollisionDir(CollisionDir dir) { mCollisionDir = dir; }

protected:
    // TODO toMoves should be ints
    void moveActors(f32 toMoveRounded, f32 toMoveUnrounded, bool isXDirection, f32 solidEdge, EdgeGetter edgeFunc,
                    std::vector<ActorCollider*>& riding, bool isManualMove);

private:
    void moveSemiSolids(bool isXDirection, s32 toMoveRounded, s32 solidEdge, EdgeGetter edgeFunc, std::vector<SemiSolidCollider*>& riding,
                        bool isManualMove);

    CollisionDir mCollisionDir;
    ActorCollisionCallback mOnCollisionEnter;
};

// A collider which acts like a Solid when interacting with Actors, but acts like an Actor when interacting with solids
// More specifically, it
//     - carries/pushes actors when moving, and stops actors that move into it
//     - stops when moving into a solid, and is carried/pushed by moving solids
//     - stops when moving into other SemiSolidColliders (may change in the future)
//
// Other stuff: can be grounded, does not have corner correction, does not have momentum(?), can be destroyed, no one-way collision variants
class SemiSolidCollider : public SolidCollider {
public:
    SemiSolidCollider() = default;
    SemiSolidCollider(Vector2f position, Vector2i half, Material material = Material::None, ActorCollisionCallback onCollisionEnter_ = nullptr);

    std::optional<HitInfo> moveX(const f32 amount, const SemiSolidCollisionCallback callback, bool isManualMove = false);
    std::optional<HitInfo> moveY(const f32 amount, const SemiSolidCollisionCallback callback, bool isManualMove = false);

    std::optional<HitInfo> checkCollisionSolids(const std::vector<SolidCollider*>& solids, const Vector2i position, const Vector2i moveNormal) const;
    std::optional<HitInfo> checkCollisionSemiSolids(const std::vector<SemiSolidCollider*>& solids, const Vector2i position) const;

    bool checkIsGrounded(const std::vector<SolidCollider*>& solids, IUseCollision** groundCollider);
    // TODO checkIsGroundedOnSemiSolids

    // still not sure if/how/should i use component inheritance with my ECS, but I'll stay consistent for now
    virtual void squish(const HitInfo hitInfo);
    virtual bool isRiding(const SolidCollider* solid) const;
};

void defaultSquish(ActorCollider* selfCollider, ecs::Entity self, HitInfo hitinfo);
void defaultSquishSemiSolid(SemiSolidCollider* selfCollider, ecs::Entity self, HitInfo hitinfo);

}  // namespace whal
