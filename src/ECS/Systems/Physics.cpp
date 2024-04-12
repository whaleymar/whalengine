#include "Physics.h"

#include <cmath>

#include "ECS/Components/Position.h"
#include "ECS/Components/RigidBody.h"
#include "ECS/Components/SolidBody.h"

#include "ECS/Components/Velocity.h"
#include "Gfx/GfxUtil.h"
#include "Systems/Deltatime.h"
#include "Util/MathUtil.h"

namespace whal {

constexpr f32 GRAVITY = -35;
constexpr f32 TERMINAL_VELOCITY_Y = -15;

constexpr f32 FRICTION_GROUND = 5.0;
constexpr f32 FRICTION_AIR = 2.0;
constexpr f32 FRICTION_IMPULSE = 10.0;
constexpr f32 MOVE_EPSILON = 0.1;

constexpr f32 JUMP_PEAK_GRAVITY_MULT = 0.5;
constexpr f32 JUMP_PEAK_SPEED_MAX = -3.5;  // once Y velocity is below this, no longer considered "jumping"

void applyGravity(Velocity& velocity, f32 dt, bool isJumping) {
    bool isInJumpPeak = isJumping && isBetween(velocity.total.y(), JUMP_PEAK_SPEED_MAX, 0.0f);
    f32 peakMultiplier = 1 - static_cast<f32>(isInJumpPeak) * (1 - JUMP_PEAK_GRAVITY_MULT);
    f32 newVelY = velocity.stable.y() + GRAVITY * peakMultiplier * dt;
    if (newVelY < TERMINAL_VELOCITY_Y) {
        newVelY = TERMINAL_VELOCITY_Y;
    }
    velocity.stable.e[1] = newVelY;
}

void applyFriction(Vector2f& velocity, f32 frictionMultiplier) {
    if (abs(velocity.e[0]) < MOVE_EPSILON) {
        velocity.e[0] = 0.0;
    } else {
        velocity.e[0] *= frictionMultiplier;
    }
}

void PhysicsSystem::update() {
    f32 dt = Deltatime::getInstance().get();
    f32 frictionRatioGround = 1.0 / (1.0 + (FRICTION_GROUND * dt));
    f32 frictionRatioAir = 1.0 / (1.0 + (FRICTION_AIR * dt));
    f32 frictionRatioImpulse = 1.0 / (1.0 + (FRICTION_IMPULSE * dt));
    std::vector<ecs::Entity> needsPositionUpdateRB;
    for (auto& [entityid, entity] : getEntities()) {
        Position& pos = entity.get<Position>();
        Velocity& vel = entity.get<Velocity>();
        std::optional<RigidBody*> rb = entity.tryGet<RigidBody>();

        // TODO? update collider position to match position component before moving?
        // only necessary if i change position outside of PhysicsSystem
        //
        // -> I think i should do it. teleporting entities will be a pain otherwise

        // if impulse ends, use residual
        Vector2f impulse = vel.impulse;
        if (!impulse.x() && abs(vel.residualImpulse.x()) >= MOVE_EPSILON) {
            impulse.e[0] += vel.residualImpulse.x();
        }
        if (!impulse.y() && abs(vel.residualImpulse.y()) >= MOVE_EPSILON) {
            impulse.e[1] += vel.residualImpulse.y();
        }

        Vector2f totalVelocity = vel.stable + impulse;
        f32 moveX = totalVelocity.x() * dt * TEXELS_PER_TILE * PIXELS_PER_TEXEL;
        f32 moveY = totalVelocity.y() * dt * TEXELS_PER_TILE * PIXELS_PER_TEXEL;
        vel.residualImpulse = impulse * frictionRatioImpulse;
        vel.impulse = {0, 0};
        vel.total = totalVelocity;

        if (moveX > 0 && pos.facing != Facing::Right) {
            pos.facing = Facing::Right;
            pos.isDirectionChanged = true;
        } else if (moveX < 0 && pos.facing != Facing::Left) {
            pos.facing = Facing::Left;
            pos.isDirectionChanged = true;
        }

        // I can split this into separate systems if this gets slow
        if (rb) {
            bool wasGrounded = rb.value()->collider.isGrounded();
            rb.value()->collider.setGrounded(false);

            // no callback needed when actor moves into solid
            rb.value()->collider.moveDirection(true, moveX, nullptr);
            rb.value()->collider.moveDirection(false, moveY, nullptr);
            needsPositionUpdateRB.push_back(entity);

            // friction
            if (vel.stable.x()) {
                if (rb.value()->collider.isGrounded()) {
                    applyFriction(vel.stable, frictionRatioGround);
                } else {
                    applyFriction(vel.stable, frictionRatioAir);
                }
            }

            // gravity and momentum
            bool isMomentumStored = rb.value()->collider.isMomentumStored();
            if (!rb.value()->collider.isGrounded()) {
                if (totalVelocity.y() < JUMP_PEAK_SPEED_MAX) {
                    // falling == not jumping
                    // a little lower than 0 while applying reduced gravity
                    rb.value()->isJumping = false;
                }

                if (wasGrounded && !rb.value()->isJumping) {
                    rb.value()->coyoteSecondsRemaining = rb.value()->coyoteTimeSecondsMax;
                } else if (rb.value()->coyoteSecondsRemaining > 0) {
                    rb.value()->coyoteSecondsRemaining -= dt;
                }

                applyGravity(vel, dt, rb.value()->isJumping);

                if (isMomentumStored) {
                    vel.stable += rb.value()->collider.getMomentum() * dt;  // TODO scaling by dt feels wrong
                    rb.value()->collider.resetMomentum();
                }

            } else {
                if (totalVelocity.y() < 0 && !rb.value()->isJumping) {
                    // zero gravity when grounded and not trying to jump, otherwise entity falls at terminal velocity after walking off platform
                    vel.stable.e[1] = 0;
                }
                if (isMomentumStored) {
                    rb.value()->collider.momentumNotUsed();
                }
            }

        } else if (std::optional<SolidBody*> sb = entity.tryGet<SolidBody>(); sb) {
            sb.value()->collider.move(moveX, moveY);
            pos.e = sb.value()->collider.getCollider().getPosition();

        } else {
            pos.e += Vector2i(std::round(moveX), std::round(moveY));
        }
    }

    // actors can be moved by other colliders, so wait until all collisions are processed to update position
    for (auto& entity : needsPositionUpdateRB) {
        Position& pos = entity.get<Position>();
        RigidBody& rb = entity.get<RigidBody>();
        pos.e = {rb.collider.getCollider().getPosition().x(), static_cast<s32>(rb.collider.getCollider().bottom())};
    }
}

}  // namespace whal
