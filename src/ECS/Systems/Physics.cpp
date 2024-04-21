#include "Physics.h"

#include <cmath>

#include "ECS/Components/Collision.h"
#include "ECS/Components/RigidBody.h"
#include "ECS/Components/Transform.h"

#include "ECS/Components/Velocity.h"
#include "Gfx/GfxUtil.h"
#include "Systems/System.h"
#include "Util/MathUtil.h"
#include "Util/Vector.h"

namespace whal {

constexpr f32 GRAVITY = 35;

constexpr f32 FRICTION_GROUND = 30.0;
constexpr f32 FRICTION_AIR = 25.0;
constexpr f32 MOVE_EPSILON = 0.1;

constexpr f32 JUMP_PEAK_GRAVITY_MULT = 0.5;
constexpr f32 JUMP_PEAK_SPEED_MAX = -3.5;  // once Y velocity is below this, no longer considered "jumping"

void applyGravity(Velocity& velocity, f32 dt, bool isJumping) {
    bool isInJumpPeak = isJumping && isBetween(velocity.total.y(), JUMP_PEAK_SPEED_MAX, 0.0f);
    f32 peakMultiplier = 1 - static_cast<f32>(isInJumpPeak) * (1 - JUMP_PEAK_GRAVITY_MULT);
    velocity.stable.e[1] = approach(velocity.stable.y(), TERMINAL_VELOCITY_Y, GRAVITY * peakMultiplier * dt);
}

void applyFriction(Vector2f& velocity, f32 frictionMultiplier) {
    velocity.e[0] = approach(velocity.x(), 0, frictionMultiplier);
}

void PhysicsSystem::update() {
    const f32 dt = System::dt();
    const f32 frictionStepGround = dt * FRICTION_GROUND;
    const f32 frictionStepAir = dt * FRICTION_AIR;
    const f32 gravityStep = dt * GRAVITY * 3;
    std::vector<ecs::Entity> allActors;

    // sync collider in case position changed in another system
    // is a little inefficient to do it this way (vs separating the systems)
    for (auto& [entityid, entity] : getEntities()) {
        Transform& trans = entity.get<Transform>();
        if (!trans.isManuallyMoved) {
            continue;
        }

        trans.isManuallyMoved = false;
        if (std::optional<ActorCollider*> actor = entity.tryGet<ActorCollider>(); actor) {
            actor.value()->setPositionFromBottom(trans.position);
        } else if (std::optional<SolidCollider*> solid = entity.tryGet<SolidCollider>(); solid) {
            solid.value()->setPositionFromBottom(trans.position);
        }
    }

    for (auto& [entityid, entity] : getEntities()) {
        Transform& trans = entity.get<Transform>();
        Velocity& vel = entity.get<Velocity>();

        // if impulse ends, use residual
        Vector2f impulse = vel.impulse;
        if (!impulse.x() && abs(vel.residualImpulse.x()) >= MOVE_EPSILON) {
            impulse.e[0] += vel.residualImpulse.x();
        }
        if (!impulse.y() && abs(vel.residualImpulse.y()) >= MOVE_EPSILON) {
            impulse.e[1] += vel.residualImpulse.y();
        }

        const Vector2f totalVelocity = vel.stable + impulse;
        const Vector2f move = {totalVelocity.x() * dt * TEXELS_PER_TILE * PIXELS_PER_TEXEL,
                               totalVelocity.y() * dt * TEXELS_PER_TILE * PIXELS_PER_TEXEL};

        vel.residualImpulse = {approach(impulse.x(), 0, frictionStepGround), approach(impulse.y(), 0, gravityStep)};
        vel.impulse = {0, 0};
        vel.total = totalVelocity;

        std::optional<RigidBody*> rb = entity.tryGet<RigidBody>();
        std::optional<ActorCollider*> actor = entity.tryGet<ActorCollider>();
        if (rb && actor) {
            // MOVEMENT + GROUNDED CHECKS
            //// only actors can be grounded (because they're the only ones that interact with the ground)
            //// no callback needed when actor moves into solid
            const bool wasGrounded = rb.value()->isGrounded;
            if (actor.value()->moveY(move, nullptr)) {
                if (move.y() <= 0) {
                    rb.value()->isGrounded = true;
                } else {
                    rb.value()->isGrounded = false;
                }
                rb.value()->isJumping = false;
                vel.residualImpulse.e[1] = 0;
            } else {
                rb.value()->isGrounded = false;
            }
            actor.value()->moveX(move, nullptr);
            allActors.push_back(entity);

            // RIGIDBODY FLAGS, MOMENTUM, AND COYOTE TIME
            bool isMomentumStored = actor.value()->isMomentumStored();
            if (rb.value()->isGrounded) {
                rb.value()->isLanding = !wasGrounded;

                if (isMomentumStored) {
                    actor.value()->momentumNotUsed();
                }

            } else {
                if (wasGrounded && !rb.value()->isJumping) {
                    rb.value()->coyoteSecondsRemaining = rb.value()->coyoteTimeSecondsMax;
                } else if (rb.value()->coyoteSecondsRemaining > 0) {
                    rb.value()->coyoteSecondsRemaining -= dt;
                }

                if (isMomentumStored) {
                    vel.stable += actor.value()->getMomentum() * dt;  // TODO scaling by dt feels wrong -- is possibly just bad naming?
                    actor.value()->resetMomentum();
                }
            }

        } else if (actor) {
            actor.value()->moveX(move, nullptr);
            actor.value()->moveY(move, nullptr);
            allActors.push_back(entity);

        } else if (std::optional<SolidCollider*> solid = entity.tryGet<SolidCollider>(); solid) {
            solid.value()->move(move.x(), move.y());
            trans.position = solid.value()->getCollider().getPositionEdge(Vector2i::unitDown);

        } else {
            trans.position += Vector2i(std::round(move.x()), std::round(move.y()));
        }

        // This could be made a lot faster if I have separate functions for actors/non-actors
        if (rb) {
            // friction
            if (vel.stable.x()) {
                if (rb.value()->isGrounded) {
                    applyFriction(vel.stable, frictionStepGround);
                } else {
                    applyFriction(vel.stable, frictionStepAir);
                    vel.residualImpulse.e[0] = approach(impulse.x(), 0, frictionStepAir);  // recalced
                }
            }

            // gravity
            if (!rb.value()->isGrounded) {
                if (totalVelocity.y() < JUMP_PEAK_SPEED_MAX) {
                    // falling == not jumping
                    // a little lower than 0 while applying reduced gravity
                    rb.value()->isJumping = false;
                }

                applyGravity(vel, dt, rb.value()->isJumping);

                rb.value()->isLanding = false;

            } else {
                if (totalVelocity.y() < 0 && !rb.value()->isJumping) {
                    // zero y velocity when grounded and not trying to jump, otherwise entity falls at terminal velocity after walking off platform
                    vel.stable.e[1] = 0;
                }
            }
        }
    }

    // actors can be moved by other colliders, so wait until all collisions are processed to update position
    for (auto& entity : allActors) {
        Transform& trans = entity.get<Transform>();
        ActorCollider& actor = entity.get<ActorCollider>();

        // position is bottom-middle of collider
        trans.position = actor.getCollider().getPositionEdge(Vector2i::unitDown);
    }
}

}  // namespace whal
