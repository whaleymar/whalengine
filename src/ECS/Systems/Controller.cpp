#include "Controller.h"

#include "ECS/Components/Draw.h"
#include "ECS/Components/PlayerControl.h"
#include "ECS/Components/RigidBody.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Velocity.h"

#include "Systems/Deltatime.h"
#include "Systems/InputHandler.h"
#include "Util/MathUtil.h"

namespace whal {

constexpr f32 APPROACH_SPEED_X = 10.0;  // 5 frames to max speed

void ControllerSystemRB::update() {
    auto& input = Input::getInstance();
    f32 dt = Deltatime::getInstance().get();

    bool isJumpAvailable = input.isJumpAvailable();
    input.useJump();

    for (auto& [entityid, entity] : getEntities()) {
        Velocity& vel = entity.get<Velocity>();
        RigidBody& rb = entity.get<RigidBody>();
        PlayerControlRB& control = entity.get<PlayerControlRB>();

#ifndef NDEBUG
        auto sprite = entity.tryGet<Sprite>();
        if (sprite) {
            bool changed = false;
            if (input.isShrinkX()) {
                sprite.value()->scale.e[0] -= 0.1;
                changed = true;
            }
            if (input.isShrinkY()) {
                sprite.value()->scale.e[1] -= 0.1;
                changed = true;
            }
            if (input.isGrowX()) {
                sprite.value()->scale.e[0] += 0.1;
                changed = true;
            }
            if (input.isGrowY()) {
                sprite.value()->scale.e[1] += 0.1;
                changed = true;
            }
            if (changed) {
                sprite.value()->isVertsUpdateNeeded = true;
            }
        }
#endif

        f32 impulseX = 0;
        if (input.isLeft()) {
            impulseX -= 1;
        }
        if (input.isRight()) {
            impulseX += 1;
        }
        impulseX *= control.moveSpeed;
        const f32 approachSpeed = APPROACH_SPEED_X * dt * control.moveSpeed;
        if (impulseX != 0) {
            f32 approachFrom;
            if (sign(impulseX) == sign(vel.total.x())) {
                approachFrom = vel.total.x();
            } else {
                approachFrom = 0;
            }
            impulseX = approach(approachFrom, impulseX, approachSpeed);
        }
        // if (abs(impulseX) == control.moveSpeed) {
        //     print("hit max speed");
        // } else if (impulseX) {
        //     print("frame");
        // }

        f32 impulseY = 0;
        if (isJumpAvailable) {
            control.jumpBuffer.buffer();
        }

        if (input.isJump()) {
            if ((rb.collider.isGrounded() || rb.coyoteSecondsRemaining > 0) && control.canJump()) {
                control.jumpBuffer.consume();
                rb.isJumping = true;

                if (vel.stable.y() < 0) {
                    vel.stable.e[1] = 0;
                }

                impulseY += rb.jumpInitialVelocity;
                rb.jumpSecondsRemaining = rb.jumpSecondsMax;
            } else if (control.isJumping()) {
                f32 damping = rb.jumpSecondsRemaining / rb.jumpSecondsMax;
                damping *= damping;  // TODO play around with this
                impulseY += rb.jumpInitialVelocity * damping;
                rb.jumpSecondsRemaining -= dt;
                if (rb.jumpSecondsRemaining < 0) {
                    rb.jumpSecondsRemaining = 0;
                    control.jumpBuffer.reset();
                    rb.isJumping = false;
                }
            } else if (control.canJump()) {
                control.jumpBuffer.notUsed();
            }
        } else {
            control.jumpBuffer.reset();
            rb.isJumping = false;
        }

        vel.impulse += Vector2f(impulseX, impulseY);

        auto& trans = entity.get<Transform>();
        if (impulseX > 0 && trans.facing != Facing::Right) {
            trans.facing = Facing::Right;
            trans.isDirectionChanged = true;
        } else if (impulseX < 0 && trans.facing != Facing::Left) {
            trans.facing = Facing::Left;
            trans.isDirectionChanged = true;
        }
    }
}

void ControllerSystemFree::update() {
    auto& input = Input::getInstance();
    for (auto& [entityid, entity] : getEntities()) {
        Vector2f delta;
        if (input.isLeft()) {
            delta += Vector2f::unitLeft;
        }
        if (input.isRight()) {
            delta += Vector2f::unitRight;
        }
        if (input.isUp()) {
            delta += Vector2f::unitUp;
        }
        if (input.isDown()) {
            delta += Vector2f::unitDown;
        }

        PlayerControlFree& control = entity.get<PlayerControlFree>();
        delta *= control.moveSpeed;

        Velocity newVel = Velocity(delta);
        entity.set(newVel);
    }
}

}  // namespace whal
