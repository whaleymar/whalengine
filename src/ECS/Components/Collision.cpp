#include "Collision.h"

#include <algorithm>
#include <cmath>
#include <type_traits>

#include "ECS/Systems/CollisionManager.h"
#include "Gfx/GfxUtil.h"
#include "Physics/Collision/HitInfo.h"
#include "Physics/IUseCollision.h"
#include "Systems/System.h"
#include "Util/MathUtil.h"

namespace whal {

constexpr s32 MOMENTUM_LIFETIME_FRAMES = 10;
constexpr s32 CORNERCORRECTIONWIGGLE = 4 * SPIXELS_PER_TEXEL;

void defaultSquish(ActorCollider* selfCollider, ecs::Entity self, HitInfo hitinfo) {
    selfCollider->squish(hitinfo);
}

ActorCollider::ActorCollider(Vector2f position, Vector2i half) : IUseCollision(AABB(half)) {
    mCollider.setPosition(position);
}

// TODO Triggers
// CURRENT PLAN
/*
 * Trigger component, holds callback which takes, self(?), other, and hitinfo
 * Pass "self" Entity into actor.move and solid.move
 *
 * in actor.move: check collision with solids. For solids which have collision turned off but have trigger components, add their entity IDs to a list
 *   => after movement is done, run each callback
 *   => if they have collision on, also add to list but then break
 * in actor.move: check collision with actors. For actors which have collision turned off but have trigger components, do same list thingy
 *   => isCollidable only affects whether other actors can collide with an entity
 *   => if I don't want the actor to interact with solids, it should be a solid
 * in actor.move: if the actor has collision turned off don't worry about other triggers -- only check for collidable actors/solids
 *   => add each collidable entity passed through to list & run moving entity's trigger callback on them after movement
 *   => only break from movement loop if colliding with solid
 *
 * in solid.move:
 *   => if solid.isCollidable == true, run callback on each actor we hit AND push/move the actor
 *   => if                    == false run callback on each actor we hit but don't push/move
 */

// UPDATED THOUGHTS
// an entity will never have multiple colliders, multiple triggers, or, a collider + trigger
// so it would make sense to have a single Collider component which is a container for all these things
// and it holds an enum or something saying its type

std::optional<HitInfo> ActorCollider::moveX(const Vector2f amount, const ActorCollisionCallback callback) {
    // RESEARCH doesn't handle colliding with other actors
    s32 toMove = std::round(amount.x());
    auto const& solids = SolidsManager::getInstance()->getAllSolids();

    if (toMove == 0) {
        return std::nullopt;
    }

    const s32 moveSign = sign(toMove);
    const auto moveNormal = Vector2i(moveSign, 0);
    while (toMove != 0) {
        auto nextPos = mCollider.getPosition() + moveNormal;
        auto hitInfo = checkCollision(solids, nextPos, moveNormal);
        if (hitInfo == std::nullopt) {
            mCollider.setPosition(nextPos);
            toMove -= moveSign;
        } else {
            if (callback != nullptr) {
                callback(this, getEntity(), hitInfo.value());
            }
            return hitInfo;
        }
    }
    return std::nullopt;
}

std::optional<HitInfo> ActorCollider::moveY(const Vector2f amount, const ActorCollisionCallback callback) {
    // RESEARCH doesn't handle colliding with other actors
    s32 toMove = std::round(amount.y());
    auto const& solids = SolidsManager::getInstance()->getAllSolids();

    if (toMove == 0) {
        SolidCollider* groundCollider = nullptr;
        if (amount.y() <= 0 && checkIsGrounded(solids, &groundCollider)) {
            HitInfo hitinfo(Vector2i(0, -1));
            hitinfo.other = groundCollider->getEntity();
            hitinfo.otherMaterial = groundCollider->getMaterial();
            return hitinfo;
        } else {
            return std::nullopt;
        }
    }

    const s32 moveSign = sign(toMove);
    const auto moveNormal = Vector2i(0, moveSign);
    while (toMove != 0) {
        auto nextPos = mCollider.getPosition() + moveNormal;
        auto hitInfo = checkCollision(solids, nextPos, moveNormal);
        if (hitInfo == std::nullopt) {
            mCollider.setPosition(nextPos);
            toMove -= moveSign;
        } else {
            if (moveSign == 1 && tryCornerCorrection(solids, nextPos, amount.x(), moveNormal)) {
                continue;  // avoided collision
            }
            if (callback != nullptr) {
                callback(this, getEntity(), hitInfo.value());
            }
            return hitInfo;
        }
    }

    SolidCollider* groundCollider = nullptr;
    if (amount.y() <= 0 && checkIsGrounded(solids, &groundCollider)) {
        HitInfo hitinfo(Vector2i(0, -1));
        hitinfo.other = groundCollider->getEntity();
        hitinfo.otherMaterial = groundCollider->getMaterial();
        return hitinfo;
    } else {
        return std::nullopt;
    }
}

void ActorCollider::setMomentum(const f32 momentum, const bool isXDirection) {
    if (isXDirection) {
        mStoredMomentum.e[0] = momentum;
        mMomentumFramesLeft.e[0] = MOMENTUM_LIFETIME_FRAMES;
    } else {
        mStoredMomentum.e[1] = momentum;
        mMomentumFramesLeft.e[1] = MOMENTUM_LIFETIME_FRAMES;
    }
}

void ActorCollider::addMomentum(const f32 momentum, const bool isXDirection) {
    if (isXDirection) {
        mStoredMomentum.e[0] += momentum;
        mMomentumFramesLeft.e[0] = MOMENTUM_LIFETIME_FRAMES;
    } else {
        mStoredMomentum.e[1] += momentum;
        mMomentumFramesLeft.e[1] = MOMENTUM_LIFETIME_FRAMES;
    }
}

void ActorCollider::maintainMomentum(const bool isXDirection) {
    if (isXDirection) {
        mMomentumFramesLeft.e[0] = MOMENTUM_LIFETIME_FRAMES;
    } else {
        mMomentumFramesLeft.e[1] = MOMENTUM_LIFETIME_FRAMES;
    }
}

void ActorCollider::resetMomentumX() {
    mStoredMomentum.e[0] = 0;
    mMomentumFramesLeft.e[0] = 0;
}

void ActorCollider::resetMomentumY() {
    mStoredMomentum.e[1] = 0;
    mMomentumFramesLeft.e[1] = 0;
}

void ActorCollider::resetMomentum() {
    mStoredMomentum = {0, 0};
    mMomentumFramesLeft = {0, 0};
}

void ActorCollider::momentumNotUsed() {
    mMomentumFramesLeft -= {1, 1};
    if (!mMomentumFramesLeft.x()) {
        resetMomentumX();
    }
    if (!mMomentumFramesLeft.y()) {
        resetMomentumY();
    }
}

bool ActorCollider::checkIsGrounded(const std::vector<std::pair<ecs::Entity, SolidCollider*>>& solids, SolidCollider** groundCollider) {
    for (auto& [entity, solid] : solids) {
        if (solid->isGround() && isRiding(solid)) {
            *groundCollider = solid;
            return true;
        }
    }
    return false;
}

// returns true if a collision CAN happen given the move normal & collision direction
bool checkDirectionalCollision(AABB actor, AABB solid, Vector2i moveNormal, CollisionDir collisionDir) {
    switch (collisionDir) {
    case CollisionDir::ALL:
        return true;

    case CollisionDir::LEFT:
        if (moveNormal.x() <= 0 || actor.right() != solid.left()) {
            return false;
        }
        break;

    case CollisionDir::RIGHT:
        if (moveNormal.x() >= 0 || actor.left() != solid.right()) {
            return false;
        }
        break;

    case CollisionDir::DOWN:
        if (moveNormal.y() <= 0 || actor.top() != solid.bottom()) {
            return false;
        }
        break;

    case CollisionDir::UP:
        if (moveNormal.y() >= 0 || actor.bottom() != solid.top()) {
            return false;
        }
        break;
    }

    return true;
}

// this currently only works for solid objects
template <typename T>
std::optional<HitInfo> ActorCollider::checkCollision(const std::vector<std::pair<ecs::Entity, T*>>& objects, const Vector2i position,
                                                     const Vector2i moveNormal) const {
    static_assert(std::is_base_of_v<IUseCollision, T>, "collider must inherit IUseCollision");

    auto movedCollider = AABB(position, mCollider.half);
    for (auto& [entity, collider] : objects) {
        if (!collider->isCollidable()) {
            continue;
        }

        // check for one-way collision skips
        const CollisionDir collisionDir = collider->getCollisionDir();
        if (!checkDirectionalCollision(getCollider(), collider->getCollider(), moveNormal, collisionDir)) {
            continue;
        }

        std::optional<HitInfo> hitInfo = movedCollider.collide(collider->getCollider());
        if (hitInfo != std::nullopt) {
            hitInfo->other = collider->getEntity();
            hitInfo->otherMaterial = collider->getMaterial();
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
    // (making sure to use the unmoved collider for the directional collision check so the edges are properly aligned)
    auto movedCollider = AABB(mCollider.center + Vector2i::unitDown, mCollider.half);
    if (movedCollider.isOverlapping(solid->getCollider()) &&
        (solid->getCollisionDir() == CollisionDir::ALL ||
         checkDirectionalCollision(getCollider(), solid->getCollider(), {0, -1}, solid->getCollisionDir()))) {
        return true;
    }
    return false;
}

// Try to wiggle out of collision if barely clipping another collider.
// Returns true if successful.
bool ActorCollider::tryCornerCorrection(const std::vector<std::pair<ecs::Entity, SolidCollider*>>& solids, Vector2i nextPosition, s32 moveSignX,
                                        Vector2i moveNormal) {
    // if we are on a half texel x coord, start at 0.5 texels of movement
    if (moveSignX >= 0) {
        for (s32 i = SPIXELS_PER_TEXEL - nextPosition.x() % SPIXELS_PER_TEXEL; i <= CORNERCORRECTIONWIGGLE; i += SPIXELS_PER_TEXEL) {
            Vector2i nextPos = nextPosition + Vector2i(i, 0);
            if (!checkCollision(solids, nextPos, moveNormal)) {
                mCollider.setPosition(nextPos);
                return true;
            }
        }
    }
    if (moveSignX <= 0) {
        for (s32 i = SPIXELS_PER_TEXEL - nextPosition.x() % SPIXELS_PER_TEXEL; i <= CORNERCORRECTIONWIGGLE; i += SPIXELS_PER_TEXEL) {
            Vector2i nextPos = nextPosition + Vector2i(-i, 0);
            if (!checkCollision(solids, nextPos, moveNormal)) {
                mCollider.setPosition(nextPos);
                return true;
            }
        }
    }

    return false;
}

SolidCollider::SolidCollider(Vector2f position, Vector2i half, Material material, ActorCollisionCallback onCollisionEnter_, CollisionDir collisionDir)
    : IUseCollision(AABB(half), material), mCollisionDir(collisionDir), mOnCollisionEnter(onCollisionEnter_) {
    mCollider.setPosition(position);
}

void SolidCollider::move(f32 x, f32 y, bool isManualMove) {
    s32 toMoveX = std::round(x);
    s32 toMoveY = std::round(y);
    if (toMoveX == 0 && toMoveY == 0) {
        return;
    }

    // check riding status *before* moving
    auto riding = getRidingActors();

    // turn off collision so actors moved by the solid don't get stuck on it
    bool wasCollidable = mIsCollidable;
    mIsCollidable = false;
    if (toMoveX > 0) {
        mCollider.setPosition(mCollider.getPosition() + Vector2i(toMoveX, 0));
        moveDirection(toMoveX, x, true, mCollider.right(), &AABB::left, riding, isManualMove);
    } else if (toMoveX < 0) {
        mCollider.setPosition(mCollider.getPosition() + Vector2i(toMoveX, 0));
        moveDirection(toMoveX, x, true, mCollider.left(), &AABB::right, riding, isManualMove);
    }

    if (toMoveY > 0) {
        mCollider.setPosition(mCollider.getPosition() + Vector2i(0, toMoveY));
        moveDirection(toMoveY, y, false, mCollider.top(), &AABB::bottom, riding, isManualMove);
    } else if (toMoveY < 0) {
        mCollider.setPosition(mCollider.getPosition() + Vector2i(0, toMoveY));
        moveDirection(toMoveY, y, false, mCollider.bottom(), &AABB::top, riding, isManualMove);
    }

    mIsCollidable = wasCollidable;
}

void SolidCollider::moveDirection(f32 toMoveRounded, f32 toMoveUnrounded, bool isXDirection, f32 solidEdge, EdgeGetter edgeFunc,
                                  std::vector<ActorCollider*>& riding, bool isManualMove) {
    f32 dt = System::dt();
    for (auto& [entity, actor] : ActorsManager::getInstance()->getAllActors()) {
        // push takes priority over carry
        if (mCollider.isOverlapping(actor->getCollider())) {
            f32 actorEdge = (actor->getCollider().*edgeFunc)();
            toMoveRounded = solidEdge - actorEdge;
            if (isXDirection) {
                actor->moveX(Vector2f(toMoveRounded, 0), &defaultSquish);
            } else {
                actor->moveY(Vector2f(0, toMoveRounded), &defaultSquish);
            }
            if (isManualMove) {
                actor->maintainMomentum(isXDirection);
            } else {
                // don't worry about rounding, we're just moving the overlap distance
                f32 momentum = toMoveRounded / dt;
                actor->setMomentum(momentum, isXDirection);
            }
        } else if (std::find(riding.begin(), riding.end(), actor) != riding.end()) {
            // I might change this for solids moving down faster than gravity RESEARCH
            if (isXDirection) {
                actor->moveX(Vector2f(toMoveRounded, 0), nullptr);
            } else {
                actor->moveY(Vector2f(0, toMoveRounded), nullptr);
            }
            if (isManualMove) {
                actor->maintainMomentum(isXDirection);
            } else {
                f32 momentum = toMoveUnrounded / dt;
                actor->setMomentum(momentum, isXDirection);
            }
        }
    }
}

std::vector<ActorCollider*> SolidCollider::getRidingActors() const {
    std::vector<ActorCollider*> riding;
    for (auto& [entity, actor] : ActorsManager::getInstance()->getAllActors()) {
        if (actor->isRiding(this)) {
            riding.push_back(actor);
        }
    }
    return riding;
}

void SolidCollider::setCollisionCallback(ActorCollisionCallback callback) {
    mOnCollisionEnter = callback;
    SolidsManager::getInstance()->setUpdateNeeded();
}

bool SolidCollider::isGround() const {
    return mCollisionDir == CollisionDir::ALL || mCollisionDir == CollisionDir::UP;
}

}  // namespace whal
