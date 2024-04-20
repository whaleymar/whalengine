#include "Collision.h"

#include <algorithm>
#include <cmath>
#include <type_traits>

#include "Gfx/GfxUtil.h"
#include "Physics/Collision/HitInfo.h"
#include "Physics/CollisionManager.h"
#include "Physics/IUseCollision.h"
#include "Systems/Deltatime.h"
#include "Util/MathUtil.h"

namespace whal {

constexpr s32 MOMENTUM_LIFETIME_FRAMES = 10;
constexpr s32 SPIXELS_PER_TEXEL = static_cast<s32>(PIXELS_PER_TEXEL);
constexpr s32 CORNERCORRECTIONWIGGLE = 4 * SPIXELS_PER_TEXEL;

ActorCollider::ActorCollider(Vector2f position, Vector2i half) : IUseCollision(AABB(half)) {
    mCollider.setPosition(position);
}

void ActorCollider::moveDirection(const bool isXDirection, const f32 amount, const CollisionCallback callback) {
    // RESEARCH doesn't handle colliding with other actors
    s32 toMove = std::round(amount);
    auto& solids = CollisionManager::getInstance().getAllSolids();

    if (toMove == 0) {
        // manually check isGrounded (gravity may not move an actor by a full texel every frame)
        if (!isXDirection) {
            checkIsGrounded(solids);
        }
        return;
    }

    const s32 moveSign = sign(toMove);
    const auto moveVector = isXDirection ? Vector2i(moveSign, 0) : Vector2i(0, moveSign);
    while (toMove != 0) {
        auto nextPos = mCollider.getPosition() + moveVector;
        auto hitInfo = checkCollision(solids, nextPos);
        if (hitInfo == std::nullopt) {
            mCollider.setPosition(nextPos);
            toMove -= moveSign;
        } else {
            bool isStopped = true;
            if (!isXDirection && moveSign == -1) {
                // moving down
                setGrounded(true);
                // TODO set isJumping to false
            } else if (!isXDirection && moveSign == 1) {
                // moving up
                isStopped = !tryCornerCorrection(solids, nextPos, 0);  // todo pass X speed here in last arg
                if (isStopped) {
                    // set isJumping of parent collider to false... // TODO maybe collisioncallback should take an ENTITY arg...
                    // or MAYBE moveDirection can just return a struct with this shit
                } else {
                    continue;
                }
            }
            if (callback != nullptr) {
                ((*this).*callback)(hitInfo.value());
            }
            if (isStopped) {
                break;
            }
        }
    }
}

void ActorCollider::setMomentum(const f32 momentum, const bool isXDirection) {
    if (isXDirection) {
        mStoredMomentum.e[0] = momentum;
    } else {
        mStoredMomentum.e[1] = momentum;
    }
    mMomentumFramesLeft = MOMENTUM_LIFETIME_FRAMES;
}

void ActorCollider::addMomentum(const f32 momentum, const bool isXDirection) {
    if (isXDirection) {
        mStoredMomentum.e[0] += momentum;
    } else {
        mStoredMomentum.e[1] += momentum;
    }
    mMomentumFramesLeft = MOMENTUM_LIFETIME_FRAMES;
}

void ActorCollider::resetMomentum() {
    mStoredMomentum = {0, 0};
    mMomentumFramesLeft = 0;
}

void ActorCollider::momentumNotUsed() {
    mMomentumFramesLeft--;
    if (!mMomentumFramesLeft) {
        resetMomentum();
    }
}

void ActorCollider::checkIsGrounded(const std::vector<SolidCollider*>& solids) {
    for (auto solid : solids) {
        if (isRiding(solid)) {
            mIsGrounded = true;
            return;
        }
    }
}

template <typename T>
std::optional<HitInfo> ActorCollider::checkCollision(const std::vector<T*>& objects, const Vector2i position) const {
    static_assert(std::is_base_of_v<IUseCollision, T>, "collider must inherit IUseCollision");

    auto movedCollider = AABB(position, mCollider.half);
    for (auto& object : objects) {
        if (!object->isCollidable()) {
            continue;
        }
        std::optional<HitInfo> hitInfo = movedCollider.collide(object->getCollider());
        if (hitInfo != std::nullopt) {
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
    auto movedCollider = AABB(mCollider.center + Vector2i::unitDown, mCollider.half);
    if (movedCollider.collide(solid->getCollider()) != std::nullopt) {
        return true;
    }
    return false;
}

bool ActorCollider::tryCornerCorrection(const std::vector<SolidCollider*>& solids, Vector2i nextPosition, s32 moveSignX) {
    // try to wiggle out of collision if barely clipping another collider
    // returns true if successful

    // TODO if vel.total.x() >= 0 check right
    // if vel.total.x() <= 0 check left
    if (moveSignX >= 0) {
        // if we are on a half texel x coord, start at 0.5 texels of movement
        for (s32 i = SPIXELS_PER_TEXEL - nextPosition.x() % SPIXELS_PER_TEXEL; i <= CORNERCORRECTIONWIGGLE; i += SPIXELS_PER_TEXEL) {
            Vector2i nextPos = nextPosition + Vector2i(i, 0);
            if (!checkCollision(solids, nextPos)) {
                mCollider.setPosition(nextPos);
                return true;
            }
        }
    }
    if (moveSignX <= 0) {
        for (s32 i = SPIXELS_PER_TEXEL - nextPosition.x() % SPIXELS_PER_TEXEL; i <= CORNERCORRECTIONWIGGLE; i += SPIXELS_PER_TEXEL) {
            Vector2i nextPos = nextPosition + Vector2i(-i, 0);
            if (!checkCollision(solids, nextPos)) {
                mCollider.setPosition(nextPos);
                return true;
            }
        }
    }

    return false;
}

SolidCollider::SolidCollider(Vector2f position, Vector2i half) : IUseCollision(AABB(half)) {
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
    mIsCollidable = false;
    if (toMoveX > 0) {
        mCollider.setPosition(mCollider.getPosition() + Vector2i(toMoveX, 0));
        moveDirection(toMoveX, true, mCollider.right(), &AABB::left, riding, isManualMove);
    } else if (toMoveX < 0) {
        mCollider.setPosition(mCollider.getPosition() + Vector2i(toMoveX, 0));
        moveDirection(toMoveX, true, mCollider.left(), &AABB::right, riding, isManualMove);
    }

    if (toMoveY > 0) {
        mCollider.setPosition(mCollider.getPosition() + Vector2i(0, toMoveY));
        moveDirection(toMoveY, false, mCollider.top(), &AABB::bottom, riding, isManualMove);
    } else if (toMoveY < 0) {
        mCollider.setPosition(mCollider.getPosition() + Vector2i(0, toMoveY));
        moveDirection(toMoveY, false, mCollider.bottom(), &AABB::top, riding, isManualMove);
    }

    mIsCollidable = true;
}

void SolidCollider::moveDirection(f32 toMove, bool isXDirection, f32 solidEdge, EdgeGetter edgeFunc, std::vector<ActorCollider*>& riding,
                                  bool isManualMove) {
    f32 dt = Deltatime::getInstance().get();
    for (auto& actor : CollisionManager::getInstance().getAllActors()) {
        // push takes priority over carry
        if (mCollider.isOverlapping(actor->getCollider())) {
            f32 actorEdge = (actor->getCollider().*edgeFunc)();
            toMove = solidEdge - actorEdge;
            actor->moveDirection(isXDirection, toMove, &ActorCollider::squish);
            f32 momentum = toMove / dt;
            if (isManualMove) {
                actor->addMomentum(momentum, isXDirection);
            } else {
                actor->setMomentum(momentum, isXDirection);
            }
        } else if (std::find(riding.begin(), riding.end(), actor) != riding.end()) {
            // I might change this for solids moving down faster than gravity RESEARCH
            actor->moveDirection(isXDirection, toMove, nullptr);
            f32 momentum = toMove / dt;
            if (isManualMove) {
                actor->addMomentum(momentum, isXDirection);
            } else {
                actor->setMomentum(momentum, isXDirection);
            }
        }
    }
}

std::vector<ActorCollider*> SolidCollider::getRidingActors() const {
    std::vector<ActorCollider*> riding;
    for (auto& actor : CollisionManager::getInstance().getAllActors()) {
        if (actor->isRiding(this)) {
            riding.push_back(actor);
        }
    }
    return riding;
}

}  // namespace whal
