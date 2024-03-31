#include "Physics.h"

#include "ECS/Components/Position.h"
#include "ECS/Components/RigidBody.h"
#include "ECS/Components/SolidBody.h"

#include "ECS/Components/Velocity.h"
#include "Gfx/GfxUtil.h"
#include "Systems/Deltatime.h"
#include "Util/Print.h"

namespace whal {

constexpr f32 GRAVITY = -35;
constexpr f32 FRICTION = 0.5;
constexpr f32 TERMINAL_VELOCITY_Y = -15;
constexpr f32 JUMP_GRAVITY_MULT = 0.8;

void applyGravity(Vector2f& velocity, f32 dt, bool isJumping) {
    f32 multiplier = 1 - static_cast<f32>(isJumping) * (1 - JUMP_GRAVITY_MULT);
    f32 newVelY = velocity.y() + GRAVITY * multiplier * dt;
    if (newVelY < TERMINAL_VELOCITY_Y) {
        newVelY = TERMINAL_VELOCITY_Y;
    }
    velocity.e[1] = newVelY;
}

void applyFriction(Vector2f& velocity) {
    // TODO dt
    velocity.e[0] *= FRICTION;
}

void PhysicsSystem::update() {
    f32 dt = Deltatime::getInstance().get();
    std::vector<ecs::Entity> needsPositionUpdateRB;
    for (auto& [entityid, entity] : getEntities()) {
        Position& pos = entity.get<Position>();
        Velocity& vel = entity.get<Velocity>();
        std::optional<RigidBody*> rb = entity.tryGet<RigidBody>();

        // TODO? update collider position to match position component before moving?
        // only necessary if i change position outside of PhysicsSystem

        // velocity currently in pixels per second
        if (rb) {
            print(rb.value()->collider.getMomentum().y());
        }
        Vector2f totalVelocity = vel.vel + vel.impulse;
        f32 moveX = totalVelocity.x() * dt * PIXELS_PER_TEXEL * TEXELS_PER_TILE;
        f32 moveY = totalVelocity.y() * dt * PIXELS_PER_TEXEL * TEXELS_PER_TILE;
        vel.impulse = {0, 0};

        // I can split this into separate systems if this gets slow
        if (rb) {
            rb.value()->collider.setGrounded(false);

            // no callback needed when actor moves into solid
            rb.value()->collider.moveDirection(true, moveX, nullptr);
            rb.value()->collider.moveDirection(false, moveY, nullptr);
            needsPositionUpdateRB.push_back(entity);
            if (!rb.value()->collider.isGrounded()) {
                if (totalVelocity.y() < 0) {
                    // falling == not jumping
                    rb.value()->isJumping = false;
                }
                applyGravity(vel.vel, dt, rb.value()->isJumping);
            } else {
                if (totalVelocity.y() < 0 && !rb.value()->isJumping) {
                    // zero gravity when grounded and not trying to jump, otherwise entity falls at terminal velocity after walking off platform
                    vel.vel.e[1] = 0;
                }
                if (vel.vel.x()) {
                    applyFriction(vel.vel);
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
