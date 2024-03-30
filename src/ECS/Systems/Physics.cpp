#include "Physics.h"

#include "ECS/Components/Position.h"
#include "ECS/Components/RigidBody.h"
#include "ECS/Components/SolidBody.h"

#include "ECS/Components/Velocity.h"
#include "Gfx/GfxUtil.h"
#include "Systems/Deltatime.h"
#include "Util/Print.h"

namespace whal {

constexpr f32 GRAVITY = -2.5;
constexpr f32 FRICTION = 0.5;
constexpr f32 TERMINAL_VELOCITY_Y = -15;
constexpr f32 JUMP_VELOCITY = 5.5;
constexpr f32 JUMP_GRAVITY_MULT = 0.5;

void applyGravity(Vector2f& velocity, bool isJumping) {
    f32 multiplier = 1 - static_cast<f32>(isJumping) * (1 - JUMP_GRAVITY_MULT);
    f32 newVelY = velocity.y() + GRAVITY * multiplier;
    if (newVelY < TERMINAL_VELOCITY_Y) {
        newVelY = TERMINAL_VELOCITY_Y;
    }
    velocity.e[1] = newVelY;
}

void applyFriction(Vector2f& velocity) {
    velocity.e[0] *= FRICTION;
}

void PhysicsSystem::update() {
    f32 dt = Deltatime::getInstance().get();
    std::vector<ecs::Entity> needsPositionUpdateRB;
    for (auto& [entityid, entity] : getEntities()) {
        Position& pos = entity.get<Position>();
        Velocity& vel = entity.get<Velocity>();
        std::optional<RigidBody*> rb = entity.tryGet<RigidBody>();

        if (rb && rb.value()->isJumping) {
            if (rb.value()->collider.isGrounded()) {
                rb.value()->inputVelocity += Vector2f(0, JUMP_VELOCITY);
                rb.value()->jumpSecondsRemaining = rb.value()->jumpSecondsMax;
            } else {
                f32 secondsRemaining = rb.value()->jumpSecondsRemaining - dt;
                if (secondsRemaining < 0) {
                    rb.value()->jumpSecondsRemaining = 0;
                    rb.value()->isJumping = false;
                    rb.value()->inputVelocity = Vector2f();
                } else {
                    rb.value()->jumpSecondsRemaining = secondsRemaining;
                    f32 multiplier = secondsRemaining / rb.value()->jumpSecondsMax;
                    multiplier *= multiplier;
                    rb.value()->inputVelocity.e[1] *= multiplier;
                }
            }
            // maybe use input velocity for x too and add to vel outside of jump check?
            vel.e += rb.value()->inputVelocity;
        } else if (rb) {
            rb.value()->inputVelocity = Vector2f();
        }

        // TODO? update collider position to match position component before moving?

        // velocity currently in pixels per second
        f32 moveX = vel.e.x() * dt * PIXELS_PER_TEXEL * TEXELS_PER_TILE;
        f32 moveY = vel.e.y() * dt * PIXELS_PER_TEXEL * TEXELS_PER_TILE;

        // I can split this into separate systems if this gets slow
        if (rb) {
            rb.value()->collider.setGrounded(false);

            // no callback needed when actor moves into solid
            rb.value()->collider.moveDirection(true, moveX, nullptr);
            rb.value()->collider.moveDirection(false, moveY, nullptr);
            needsPositionUpdateRB.push_back(entity);
            if (!rb.value()->collider.isGrounded()) {
                applyGravity(vel.e, rb.value()->isJumping);
            } else {
                if (vel.e.y() < 0 && !rb.value()->isJumping) {
                    // zero gravity when grounded and not trying to jump, otherwise entity falls at terminal velocity after walking off platform
                    vel.e.e[1] = 0;
                }
                if (vel.e.x()) {
                    applyFriction(vel.e);
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
        pos.e = rb.collider.getCollider().getPosition();
    }
}

}  // namespace whal
