#include "Controller.h"

#include "ECS/Components/Draw.h"
#include "ECS/Components/PlayerControl.h"
#include "ECS/Components/RigidBody.h"
#include "ECS/Components/Velocity.h"

#include "Systems/Deltatime.h"
#include "Systems/InputHandler.h"
#include "Util/Print.h"

namespace whal {

void ControllerSystemRB::update() {
    auto& input = Input::getInstance();
    f32 dt = Deltatime::getInstance().get();
    bool isJumpUsed = false;
    for (auto& [entityid, entity] : getEntities()) {
        Velocity& vel = entity.get<Velocity>();
        RigidBody& rb = entity.get<RigidBody>();

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

        f32 impulseY = 0;
        if (input.isJump()) {
            if ((rb.collider.isGrounded() || rb.coyoteSecondsRemaining > 0) && !input.isJumpUsed()) {
                rb.isJumping = true;
                isJumpUsed = true;

                if (vel.stable.y() < 0) {
                    vel.stable.e[1] = 0;
                }

                impulseY += rb.jumpInitialVelocity;
                rb.jumpSecondsRemaining = rb.jumpSecondsMax;
            } else if (rb.isJumping) {
                f32 damping = rb.jumpSecondsRemaining / rb.jumpSecondsMax;
                damping *= damping;  // TODO play around with this
                impulseY += rb.jumpInitialVelocity * damping;
                rb.jumpSecondsRemaining -= dt;
                if (rb.jumpSecondsRemaining < 0) {
                    rb.jumpSecondsRemaining = 0;
                    rb.isJumping = false;
                }
            }
        } else {
            rb.isJumping = false;
        }

        PlayerControlRB& control = entity.get<PlayerControlRB>();
        impulseX *= control.moveSpeed;

        vel.impulse += Vector2f(impulseX, impulseY);
    }
    if (isJumpUsed) {
        // use jump after all entities have processed inputs
        // so multiple entities can jump
        input.useJump();
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
