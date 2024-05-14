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

void defaultSquish(ecs::Entity callbackEntity, ecs::Entity other, IUseCollision* callbackEntityCollider, IUseCollision* otherCollider,
                   Vector2i moveNormal) {
    callbackEntityCollider->squish();
}

ActorCollider::ActorCollider(Transform transform, Vector2i half, Material material, CollisionCallback onCollisionEnter_)
    : IUseCollision(AABB(transform, half), material, onCollisionEnter_) {}

std::optional<HitInfo> ActorCollider::moveX(const Vector2f amount, const CollisionCallback callback) {
    // RESEARCH doesn't handle colliding with other actors

    // include fractional movement from previous calls
    mXRemainder += amount.x();
    s32 toMove = std::round(mXRemainder);

    if (toMove == 0) {
        return std::nullopt;
    }
    auto const& solids = SolidsManager::getInstance()->getAllSolids();
    auto const& semiSolids = SemiSolidsManager::getInstance()->getAllSemiSolids();
    // auto const& actors = ActorsManager::getInstance()->getAllActors();

    mXRemainder -= toMove;
    const s32 moveSign = sign(toMove);
    const auto moveNormal = Vector2i(moveSign, 0);
    while (toMove != 0) {
        auto nextPos = mCollider.getPosition() + moveNormal;
        auto hitInfo = checkCollisionSolids(solids, nextPos, moveNormal);
        if (!hitInfo) {
            hitInfo = checkCollisionSemiSolids(semiSolids, nextPos);
        }

        if (!hitInfo) {
            mCollider.setPosition(nextPos);
            toMove -= moveSign;
        } else {
            if (callback != nullptr) {
                callback(getEntity(), hitInfo->other, this, nullptr, moveNormal);  // TODO nullptr shouldn't be here
            }
            return hitInfo;
        }
    }
    return std::nullopt;
}

std::optional<HitInfo> ActorCollider::moveY(const Vector2f amount, const CollisionCallback callback, bool isGroundedCheckNeeded) {
    // RESEARCH doesn't handle colliding with other actors

    // include fractional movement from previous calls
    mYRemainder += amount.y();
    s32 toMove = std::round(mYRemainder);
    auto const& solids = SolidsManager::getInstance()->getAllSolids();
    auto const& semiSolids = SemiSolidsManager::getInstance()->getAllSemiSolids();

    auto groundedCheck = [this](f32 amountY, const std::vector<SolidCollider*>& solids,
                                const std::vector<SemiSolidCollider*>& semis) -> std::optional<HitInfo> {
        IUseCollision* groundCollider = nullptr;
        if (amountY > 0) {
            return std::nullopt;
        }

        if (checkIsGrounded(solids, &groundCollider)) {
            HitInfo hitinfo(Vector2i(0, -1));
            hitinfo.other = groundCollider->getEntity();
            hitinfo.otherMaterial = groundCollider->getMaterial();
            hitinfo.isOtherSolid = true;
            return hitinfo;
        }
        if (checkIsGroundedOnSemiSolids(semis, &groundCollider)) {
            HitInfo hitinfo(Vector2i(0, -1));
            hitinfo.other = groundCollider->getEntity();
            hitinfo.otherMaterial = groundCollider->getMaterial();
            hitinfo.isOtherSemiSolid = true;
            return hitinfo;
        }

        return std::nullopt;
    };

    if (toMove == 0) {
        if (isGroundedCheckNeeded) {
            return groundedCheck(amount.y(), solids, semiSolids);
        }
        return std::nullopt;
    }

    mYRemainder -= toMove;
    const s32 moveSign = sign(toMove);
    const auto moveNormal = Vector2i(0, moveSign);
    while (toMove != 0) {
        auto nextPos = mCollider.getPosition() + moveNormal;
        auto hitInfo = checkCollisionSolids(solids, nextPos, moveNormal);
        if (!hitInfo) {
            hitInfo = checkCollisionSemiSolids(semiSolids, nextPos);
        }

        if (!hitInfo) {
            mCollider.setPosition(nextPos);
            toMove -= moveSign;
        } else {
            if (moveSign == 1 && ((hitInfo->isOtherSolid && (tryCornerCorrection(solids, nextPos, amount.x(), moveNormal))) ||
                                  (hitInfo->isOtherSemiSolid && (tryCornerCorrectionSemiSolids(semiSolids, nextPos, amount.x()))))) {
                continue;  // avoided collision
            }
            if (callback != nullptr) {
                callback(getEntity(), hitInfo->other, this, nullptr, moveNormal);  // TODO nullptr shouldn't be here
            }
            return hitInfo;
        }
    }

    if (isGroundedCheckNeeded) {
        return groundedCheck(amount.y(), solids, semiSolids);
    }
    return std::nullopt;
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

bool ActorCollider::checkIsGroundedOnSemiSolids(const std::vector<SemiSolidCollider*>& semis, IUseCollision** groundCollider) {
    for (auto& semi : semis) {
        if (semi->isCollidable() && isRiding(semi)) {
            *groundCollider = semi;
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

std::optional<HitInfo> ActorCollider::checkCollisionSemiSolids(const std::vector<SemiSolidCollider*>& semis, const Vector2i position) const {
    auto movedCollider = AABB(position, mCollider.half);
    for (auto& collider : semis) {
        if (!collider->isCollidable()) {
            continue;
        }

        std::optional<HitInfo> hitInfo = movedCollider.collide(collider->getCollider());
        if (hitInfo != std::nullopt) {
            hitInfo->other = collider->getEntity();
            hitInfo->isOtherSemiSolid = true;
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

bool ActorCollider::tryCornerCorrectionSemiSolids(const std::vector<SemiSolidCollider*>& semis, Vector2i nextPosition, s32 moveSignX) {
    // if we are on a half texel x coord, start at 0.5 texels of movement
    if (moveSignX >= 0) {
        for (s32 i = SPIXELS_PER_TEXEL - nextPosition.x() % SPIXELS_PER_TEXEL; i <= CORNERCORRECTIONWIGGLE; i += SPIXELS_PER_TEXEL) {
            Vector2i nextPos = nextPosition + Vector2i(i, 0);
            if (!checkCollisionSemiSolids(semis, nextPos)) {
                mCollider.setPosition(nextPos);
                return true;
            }
        }
    }
    if (moveSignX <= 0) {
        for (s32 i = SPIXELS_PER_TEXEL - nextPosition.x() % SPIXELS_PER_TEXEL; i <= CORNERCORRECTIONWIGGLE; i += SPIXELS_PER_TEXEL) {
            Vector2i nextPos = nextPosition + Vector2i(-i, 0);
            if (!checkCollisionSemiSolids(semis, nextPos)) {
                mCollider.setPosition(nextPos);
                return true;
            }
        }
    }

    return false;
}

SolidCollider::SolidCollider(Transform transform, Vector2i half, Material material, CollisionCallback onCollisionEnter_, CollisionDir collisionDir)
    : IUseCollision(AABB(transform, half), material, onCollisionEnter_), mCollisionDir(collisionDir) {}

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
    auto ridingSemiSolids = getRidingSemiSolids();

    // turn off collision so actors moved by the solid don't get stuck on it
    bool wasCollidable = mIsCollidable;
    mIsCollidable = false;
    if (toMoveX > 0) {
        mCollider.setPosition(mCollider.getPosition() + Vector2i(toMoveX, 0));
        moveActors(toMoveX, x, true, mCollider.right(), &AABB::left, riding, isManualMove);
        moveSemiSolids(true, toMoveX, mCollider.right(), &AABB::left, ridingSemiSolids, isManualMove);

    } else if (toMoveX < 0) {
        mCollider.setPosition(mCollider.getPosition() + Vector2i(toMoveX, 0));
        moveActors(toMoveX, x, true, mCollider.left(), &AABB::right, riding, isManualMove);
        moveSemiSolids(true, toMoveX, mCollider.left(), &AABB::right, ridingSemiSolids, isManualMove);
    }

    if (toMoveY > 0) {
        mCollider.setPosition(mCollider.getPosition() + Vector2i(0, toMoveY));
        moveActors(toMoveY, y, false, mCollider.top(), &AABB::bottom, riding, isManualMove);
        moveSemiSolids(false, toMoveY, mCollider.top(), &AABB::bottom, ridingSemiSolids, isManualMove);

    } else if (toMoveY < 0) {
        mCollider.setPosition(mCollider.getPosition() + Vector2i(0, toMoveY));
        moveActors(toMoveY, y, false, mCollider.bottom(), &AABB::top, riding, isManualMove);
        moveSemiSolids(false, toMoveY, mCollider.bottom(), &AABB::top, ridingSemiSolids, isManualMove);
    }

    mIsCollidable = wasCollidable;
}

void SolidCollider::moveActors(s32 toMoveRounded, f32 toMoveUnrounded, bool isXDirection, s32 solidEdge, EdgeGetter edgeFunc,
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
            s32 actorEdge = (actor->getCollider().*edgeFunc)();
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
                f32 momentum = static_cast<f32>(toMoveRounded) / dt;
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

void SolidCollider::moveSemiSolids(bool isXDirection, s32 toMoveRounded, s32 solidEdge, EdgeGetter edgeFunc, std::vector<SemiSolidCollider*>& riding,
                                   bool isManualMove) {
    Vector2i moveNormal;
    if (isXDirection) {
        moveNormal = {sign(toMoveRounded), 0};
    } else {
        moveNormal = {0, sign(toMoveRounded)};
    }
    for (auto& semiSolid : SemiSolidsManager::getInstance()->getAllSemiSolids()) {
        if (!semiSolid->isCollidable()) {
            continue;
        }
        // push takes priority over carry
        if (mCollider.isOverlapping(semiSolid->getCollider()) &&
            checkDirectionalCollision(semiSolid->getCollider(), getCollider(), moveNormal, getCollisionDir())) {
            s32 actorEdge = (semiSolid->getCollider().*edgeFunc)();
            toMoveRounded = solidEdge - actorEdge;
            auto ridingActors = semiSolid->getRidingActors();
            auto ridingSemis = semiSolid->getRidingSemiSolids();
            if (isXDirection) {
                semiSolid->moveX(toMoveRounded, &defaultSquish, ridingActors, ridingSemis);
            } else {
                semiSolid->moveY(toMoveRounded, &defaultSquish, ridingActors, ridingSemis);
            }
        } else if (std::find(riding.begin(), riding.end(), semiSolid) != riding.end()) {
            auto ridingActors = semiSolid->getRidingActors();
            auto ridingSemis = semiSolid->getRidingSemiSolids();
            // I might change this for solids moving down faster than gravity RESEARCH
            if (isXDirection) {
                semiSolid->moveX(toMoveRounded, nullptr, ridingActors, ridingSemis);
            } else {
                semiSolid->moveY(toMoveRounded, nullptr, ridingActors, ridingSemis);
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

std::vector<SemiSolidCollider*> SolidCollider::getRidingSemiSolids() const {
    // RESEARCH should a destination vector be passed here and ^ to avoid copying?
    std::vector<SemiSolidCollider*> riding;
    for (auto& semi : SemiSolidsManager::getInstance()->getAllSemiSolids()) {
        if (semi->isRiding(this)) {
            riding.push_back(semi);
        }
    }
    return riding;
}

void SolidCollider::setCollisionCallback(CollisionCallback callback) {
    mOnCollisionEnter = callback;
    SolidsManager::getInstance()->setUpdateNeeded();
}

bool SolidCollider::isGround() const {
    return mCollisionDir == CollisionDir::ALL || mCollisionDir == CollisionDir::UP;
}

SemiSolidCollider::SemiSolidCollider(Transform transform, Vector2i half, Material material, CollisionCallback onCollisionEnter_)
    : SolidCollider(transform, half, material, onCollisionEnter_) {}

std::optional<HitInfo> SemiSolidCollider::moveX(const f32 amount, const CollisionCallback callback, std::vector<ActorCollider*>& ridingActors,
                                                std::vector<SemiSolidCollider*>& ridingSemis, bool isManualMove) {
    mXRemainder += amount;
    s32 toMove = std::round(mXRemainder);
    if (toMove == 0) {
        return std::nullopt;
    }
    mXRemainder -= toMove;
    const s32 moveSign = sign(toMove);
    const auto moveNormal = Vector2i(moveSign, 0);

    // move one pixel at a time, checking for collision with solids
    auto const& solids = SolidsManager::getInstance()->getAllSolids();
    std::optional<HitInfo> hitInfo = std::nullopt;
    bool neverMoved = true;
    s32 toMoveOriginal = toMove;
    while (toMove != 0) {
        auto nextPos = mCollider.getPosition() + moveNormal;
        hitInfo = checkCollisionSolids(solids, nextPos, moveNormal);
        if (!hitInfo) {
            mCollider.setPosition(nextPos);
            toMove -= moveSign;
            neverMoved = false;
        } else {
            if (callback != nullptr) {
                callback(getEntity(), hitInfo->other, this, nullptr, moveNormal);  // TODO nullptr shouldn't be here
            }
            break;
        }
    }

    // done w/ movement. now check if there are overlapping actors
    // if we never moved, can skip this step
    if (neverMoved) {
        return hitInfo;
    }

    // turn off collision so stuff moved by us don't get stuck
    bool wasCollidable = mIsCollidable;
    mIsCollidable = false;

    if (toMoveOriginal > 0) {
        moveActors(toMoveOriginal, amount, true, mCollider.right(), &AABB::left, ridingActors, isManualMove);
        moveSemiSolids(true, toMoveOriginal, mCollider.right(), &AABB::left, ridingSemis, isManualMove);
    } else {
        moveActors(toMoveOriginal, amount, true, mCollider.left(), &AABB::right, ridingActors, isManualMove);
        moveSemiSolids(true, toMoveOriginal, mCollider.left(), &AABB::right, ridingSemis, isManualMove);
    }

    mIsCollidable = wasCollidable;
    return hitInfo;
}

std::optional<HitInfo> SemiSolidCollider::moveY(const f32 amount, const CollisionCallback callback, std::vector<ActorCollider*>& ridingActors,
                                                std::vector<SemiSolidCollider*>& ridingSemis, bool isManualMove, bool isGroundedCheckNeeded) {
    mYRemainder += amount;
    s32 toMove = std::round(mYRemainder);
    auto const& solids = SolidsManager::getInstance()->getAllSolids();
    auto const& semis = SemiSolidsManager::getInstance()->getAllSemiSolids();

    auto groundedCheck = [this](const f32 amount, const std::vector<SolidCollider*>& solids,
                                const std::vector<SemiSolidCollider*>& semis) -> std::optional<HitInfo> {
        IUseCollision* groundCollider = nullptr;
        if (amount > 0) {
            return std::nullopt;
        }

        if (checkIsGrounded(solids, &groundCollider)) {
            HitInfo hitinfo(Vector2i(0, -1));
            hitinfo.other = groundCollider->getEntity();
            hitinfo.otherMaterial = groundCollider->getMaterial();
            hitinfo.isOtherSolid = true;
            return hitinfo;
        }
        if (checkIsGroundedOnSemiSolids(semis, &groundCollider)) {
            HitInfo hitinfo(Vector2i(0, -1));
            hitinfo.other = groundCollider->getEntity();
            hitinfo.otherMaterial = groundCollider->getMaterial();
            hitinfo.isOtherSemiSolid = true;
            return hitinfo;
        }

        return std::nullopt;
    };

    if (toMove == 0) {
        if (isGroundedCheckNeeded) {
            return groundedCheck(amount, solids, semis);
        }
        return std::nullopt;
    }

    mYRemainder -= toMove;
    const s32 moveSign = sign(toMove);
    const auto moveNormal = Vector2i(0, moveSign);

    // move one pixel at a time, checking for collision with solids
    std::optional<HitInfo> hitInfo = std::nullopt;
    bool neverMoved = true;
    s32 toMoveOriginal = toMove;
    while (toMove != 0) {
        auto nextPos = mCollider.getPosition() + moveNormal;
        hitInfo = checkCollisionSolids(solids, nextPos, moveNormal);
        if (!hitInfo) {
            mCollider.setPosition(nextPos);
            toMove -= moveSign;
            neverMoved = false;
        } else {
            if (callback != nullptr) {
                callback(getEntity(), hitInfo->other, this, nullptr, moveNormal);  // TODO nullptr shouldn't be here
            }
            break;
        }
    }

    // done w/ movement. now check if there are overlapping actors/semisolids
    // if we never moved (immediately hit a solid), can skip this step
    if (neverMoved) {
        return hitInfo;
    }

    // turn off collision so stuff moved by us don't get stuck
    bool wasCollidable = mIsCollidable;
    mIsCollidable = false;

    if (toMoveOriginal > 0) {
        moveActors(toMoveOriginal, amount, false, mCollider.top(), &AABB::bottom, ridingActors, isManualMove);
        moveSemiSolids(false, toMoveOriginal, mCollider.top(), &AABB::bottom, ridingSemis, isManualMove);
    } else {
        moveActors(toMoveOriginal, amount, false, mCollider.bottom(), &AABB::top, ridingActors, isManualMove);
        moveSemiSolids(false, toMoveOriginal, mCollider.bottom(), &AABB::top, ridingSemis, isManualMove);
    }

    mIsCollidable = wasCollidable;
    if (!hitInfo) {
        if (isGroundedCheckNeeded) {
            return groundedCheck(amount, solids, semis);
        }
        return std::nullopt;
    }
    return hitInfo;
}

std::optional<HitInfo> SemiSolidCollider::checkCollisionSolids(const std::vector<SolidCollider*>& solids, const Vector2i position,
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

std::optional<HitInfo> SemiSolidCollider::checkCollisionSemiSolids(const std::vector<SemiSolidCollider*>& semisolids, const Vector2i position) const {
    auto movedCollider = AABB(position, mCollider.half);
    for (auto& semi : semisolids) {
        if (!semi->isCollidable() || this == semi) {
            continue;
        }

        std::optional<HitInfo> hitInfo = movedCollider.collide(semi->getCollider());
        if (hitInfo != std::nullopt) {
            hitInfo->other = semi->getEntity();
            hitInfo->isOtherSemiSolid = true;
            hitInfo->otherMaterial = semi->getMaterial();
            return hitInfo;
        }
    }

    return std::nullopt;
}

bool SemiSolidCollider::checkIsGrounded(const std::vector<SolidCollider*>& solids, IUseCollision** groundCollider) {
    for (auto& solid : solids) {
        if (solid->isCollidable() && solid->isGround() && isRiding(solid)) {
            *groundCollider = solid;
            return true;
        }
    }
    return false;
}

bool SemiSolidCollider::checkIsGroundedOnSemiSolids(const std::vector<SemiSolidCollider*>& semis, IUseCollision** groundCollider) {
    for (auto& semi : semis) {
        if (semi != this && semi->isCollidable() && isRiding(semi)) {
            *groundCollider = semi;
            return true;
        }
    }
    return false;
}

bool SemiSolidCollider::isRiding(const SolidCollider* solid) const {
    // check for collision 1 unit down
    // (making sure to use the unmoved collider for the directional collision check so the edges are properly aligned)
    auto movedCollider = AABB(mCollider.center + Vector2i::unitDown, mCollider.half);
    if (solid != this && movedCollider.isOverlapping(solid->getCollider()) &&
        (solid->getCollisionDir() == CollisionDir::ALL ||
         checkDirectionalCollision(getCollider(), solid->getCollider(), {0, -1}, solid->getCollisionDir()))) {
        return true;
    }
    return false;
}

void SemiSolidCollider::setCollisionCallback(CollisionCallback callback) {
    mOnCollisionEnter = callback;
    SemiSolidsManager::getInstance()->setUpdateNeeded();
}

void SemiSolidCollider::moveSemiSolids(bool isXDirection, s32 toMoveRounded, s32 solidEdge, EdgeGetter edgeFunc,
                                       std::vector<SemiSolidCollider*>& riding, bool isManualMove) {
    Vector2i moveNormal;
    if (isXDirection) {
        moveNormal = {sign(toMoveRounded), 0};
    } else {
        moveNormal = {0, sign(toMoveRounded)};
    }
    for (auto& semiSolid : SemiSolidsManager::getInstance()->getAllSemiSolids()) {
        if (!semiSolid->isCollidable()) {
            continue;
        }
        // push takes priority over carry
        if (mCollider.isOverlapping(semiSolid->getCollider()) &&
            checkDirectionalCollision(semiSolid->getCollider(), getCollider(), moveNormal, getCollisionDir())) {
            s32 actorEdge = (semiSolid->getCollider().*edgeFunc)();
            toMoveRounded = solidEdge - actorEdge;
            auto ridingActors = semiSolid->getRidingActors();
            auto ridingSemis = semiSolid->getRidingSemiSolids();
            if (isXDirection) {
                semiSolid->moveX(toMoveRounded, nullptr, ridingActors, ridingSemis);
            } else {
                semiSolid->moveY(toMoveRounded, nullptr, ridingActors, ridingSemis);
            }
        } else if (std::find(riding.begin(), riding.end(), semiSolid) != riding.end()) {
            auto ridingActors = semiSolid->getRidingActors();
            auto ridingSemis = semiSolid->getRidingSemiSolids();
            // I might change this for solids moving down faster than gravity RESEARCH
            if (isXDirection) {
                semiSolid->moveX(toMoveRounded, nullptr, ridingActors, ridingSemis);
            } else {
                semiSolid->moveY(toMoveRounded, nullptr, ridingActors, ridingSemis);
            }
        }
    }
}

}  // namespace whal
