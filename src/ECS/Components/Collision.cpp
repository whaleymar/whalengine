#include "Collision.h"

#include <algorithm>
#include <cmath>

#include "ECS/Systems/CollisionManager.h"
#include "Gfx/GfxUtil.h"
#include "Physics/Collision/HitInfo.h"
#include "Physics/IUseCollision.h"
#include "Systems/System.h"
#include "Util/MathUtil.h"

namespace whal {

constexpr s32 MOMENTUM_LIFETIME_FRAMES = 10;
constexpr s32 CORNERCORRECTIONWIGGLE = 3 * SPIXELS_PER_TEXEL;

void defaultSquish(ActorCollider* selfCollider, ecs::Entity self, HitInfo hitinfo) {
    selfCollider->squish(hitinfo);
}

ActorCollider::ActorCollider(Vector2f position, Vector2i half) : IUseCollision(AABB(half)) {
    mCollider.setPosition(position);
}

std::optional<HitInfo> ActorCollider::moveX(const Vector2f amount, const ActorCollisionCallback callback) {
    // RESEARCH doesn't handle colliding with other actors

    // include fractional movement from previous calls
    mXRemainder += amount.x();
    s32 toMove = std::round(mXRemainder);

    if (toMove == 0) {
        return std::nullopt;
    }
    auto const& solids = SolidsManager::getInstance()->getAllSolids();
    // auto const& actors = ActorsManager::getInstance()->getAllActors();

    mXRemainder -= toMove;
    const s32 moveSign = sign(toMove);
    const auto moveNormal = Vector2i(moveSign, 0);
    while (toMove != 0) {
        auto nextPos = mCollider.getPosition() + moveNormal;
        auto hitInfo = checkCollisionSolids(solids, nextPos, moveNormal);
        // if (!hitInfo) {
        //     hitInfo = checkCollisionActors(actors, nextPos);
        // }
        if (!hitInfo) {
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

    // include fractional movement from previous calls
    mYRemainder += amount.y();
    s32 toMove = std::round(mYRemainder);
    auto const& solids = SolidsManager::getInstance()->getAllSolids();
    // auto const& actors = ActorsManager::getInstance()->getAllActors();

    if (toMove == 0) {
        IUseCollision* groundCollider = nullptr;
        // if (amount.y() <= 0 && (checkIsGrounded(solids, &groundCollider) || checkIsGroundedOnActors(actors, &groundCollider))) {
        if (amount.y() <= 0 && (checkIsGrounded(solids, &groundCollider))) {
            HitInfo hitinfo(Vector2i(0, -1));
            hitinfo.other = groundCollider->getEntity();
            hitinfo.otherMaterial = groundCollider->getMaterial();
            return hitinfo;
        } else {
            return std::nullopt;
        }
    }

    mYRemainder -= toMove;
    const s32 moveSign = sign(toMove);
    const auto moveNormal = Vector2i(0, moveSign);
    while (toMove != 0) {
        auto nextPos = mCollider.getPosition() + moveNormal;
        auto hitInfo = checkCollisionSolids(solids, nextPos, moveNormal);
        // if (!hitInfo) {
        //     hitInfo = checkCollisionActors(actors, nextPos);
        // }
        if (!hitInfo) {
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

    IUseCollision* groundCollider = nullptr;
    // if (amount.y() <= 0 && (checkIsGrounded(solids, &groundCollider) || checkIsGroundedOnActors(actors, &groundCollider))) {
    if (amount.y() <= 0 && (checkIsGrounded(solids, &groundCollider))) {
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

bool ActorCollider::checkIsGrounded(const std::vector<SolidCollider*>& solids, IUseCollision** groundCollider) {
    for (auto& solid : solids) {
        if (solid->isCollidable() && solid->isGround() && isRiding(solid)) {
            *groundCollider = solid;
            return true;
        }
    }
    return false;
}

bool ActorCollider::checkIsGroundedOnActors(const std::vector<ActorCollider*>& actors, IUseCollision** groundCollider) {
    for (auto& actor : actors) {
        if (!actor->isCollidable() || this == actor) {
            continue;
        }
        auto movedCollider = AABB(mCollider.center + Vector2i::unitDown, mCollider.half);
        if (movedCollider.isOverlapping(actor->getCollider())) {
            *groundCollider = actor;
            return true;
        }
    }
    return false;
}

// returns true if a collision CAN happen given the move normal & collision direction
bool checkDirectionalCollision(const AABB& actor, const AABB& solid, Vector2i moveNormal, CollisionDir collisionDir) {
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

std::optional<HitInfo> ActorCollider::checkCollisionSolids(const std::vector<SolidCollider*>& solids, const Vector2i position,
                                                           const Vector2i moveNormal) const {
    auto movedCollider = AABB(position, mCollider.half);
    for (auto& collider : solids) {
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
            hitInfo->isOtherSolid = true;
            hitInfo->otherMaterial = collider->getMaterial();
            return hitInfo;
        }
    }

    return std::nullopt;
}

std::optional<HitInfo> ActorCollider::checkCollisionActors(const std::vector<ActorCollider*>& actors, const Vector2i position) const {
    auto movedCollider = AABB(position, mCollider.half);
    for (auto& actor : actors) {
        if (!actor->isCollidable() || this == actor) {
            continue;
        }

        std::optional<HitInfo> hitInfo = movedCollider.collide(actor->getCollider());
        if (hitInfo != std::nullopt) {
            hitInfo->other = actor->getEntity();
            hitInfo->otherMaterial = actor->getMaterial();
            return hitInfo;
        }
    }

    return std::nullopt;
}

void ActorCollider::squish(const HitInfo hitInfo) {
    getEntity().kill();
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
bool ActorCollider::tryCornerCorrection(const std::vector<SolidCollider*>& solids, Vector2i nextPosition, s32 moveSignX, Vector2i moveNormal) {
    // if we are on a half texel x coord, start at 0.5 texels of movement
    if (moveSignX >= 0) {
        for (s32 i = SPIXELS_PER_TEXEL - nextPosition.x() % SPIXELS_PER_TEXEL; i <= CORNERCORRECTIONWIGGLE; i += SPIXELS_PER_TEXEL) {
            Vector2i nextPos = nextPosition + Vector2i(i, 0);
            if (!checkCollisionSolids(solids, nextPos, moveNormal)) {
                mCollider.setPosition(nextPos);
                return true;
            }
        }
    }
    if (moveSignX <= 0) {
        for (s32 i = SPIXELS_PER_TEXEL - nextPosition.x() % SPIXELS_PER_TEXEL; i <= CORNERCORRECTIONWIGGLE; i += SPIXELS_PER_TEXEL) {
            Vector2i nextPos = nextPosition + Vector2i(-i, 0);
            if (!checkCollisionSolids(solids, nextPos, moveNormal)) {
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
    mXRemainder += x;
    mYRemainder += y;

    s32 toMoveX = std::round(mXRemainder);
    s32 toMoveY = std::round(mYRemainder);
    if (toMoveX == 0 && toMoveY == 0) {
        return;
    }
    mXRemainder -= toMoveX;
    mYRemainder -= toMoveY;

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
    Vector2i moveNormal;
    if (isXDirection) {
        moveNormal = {sign(toMoveRounded), 0};
    } else {
        moveNormal = {0, sign(toMoveRounded)};
    }
    for (auto& actor : ActorsManager::getInstance()->getAllActors()) {
        // push takes priority over carry
        if (mCollider.isOverlapping(actor->getCollider()) &&
            checkDirectionalCollision(actor->getCollider(), getCollider(), moveNormal, getCollisionDir())) {
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
    for (auto& actor : ActorsManager::getInstance()->getAllActors()) {
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
