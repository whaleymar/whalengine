#include "Controller.h"

#include "ECS/Components/PlayerControl.h"
#include "ECS/Components/RigidBody.h"
#include "ECS/Components/Velocity.h"

#include "Systems/Deltatime.h"
#include "Systems/InputHandler.h"

namespace whal {

void ControllerSystemRB::update() {
    auto& input = Input::getInstance();
    f32 dt = Deltatime::getInstance().get();
    for (auto& [entityid, entity] : getEntities()) {
        Velocity& vel = entity.get<Velocity>();
        RigidBody& rb = entity.get<RigidBody>();

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
                input.useJump();

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
}

void ControllerSystemFree::update() {
    auto& input = Input::getInstance();
    for (auto& [entityid, entity] : getEntities()) {
        Vector2f delta;
        if (input.isLeft()) {
            delta += Vector::unitfLeft;
        }
        if (input.isRight()) {
            delta += Vector::unitfRight;
        }
        if (input.isUp()) {
            delta += Vector::unitfUp;
        }
        if (input.isDown()) {
            delta += Vector::unitfDown;
        }

        PlayerControlFree& control = entity.get<PlayerControlFree>();
        delta *= control.moveSpeed;

        Velocity newVel = Velocity(delta);
        entity.set(newVel);
    }
}

}  // namespace whal
