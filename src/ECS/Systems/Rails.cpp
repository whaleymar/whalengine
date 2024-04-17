#include "Rails.h"

#include "ECS/Components/Position.h"
#include "ECS/Components/RailsControl.h"
#include "ECS/Components/RigidBody.h"
#include "ECS/Components/SolidBody.h"
#include "ECS/Components/Velocity.h"

#include "Systems/Deltatime.h"
#include "Util/Vector.h"

namespace whal {

constexpr f32 CHECKPOINT_DISTANCE_THRESHOLD = 4;  // in pixels

void RailsSystem::update() {
    f32 dt = Deltatime::getInstance().get();
    f32 inv_dt = 1 / dt;
    for (auto& [entityid, entity] : getEntities()) {
        auto& rails = entity.get<RailsControl>();
        if (!rails.isValid()) {
            continue;
        }
        auto& position = entity.get<Position>();

        Vector2f delta = toFloatVec(rails.getTarget().position - position.e);
        f32 distance = delta.len();

        // scale checkpoint threshold with speed
        f32 speed = [entity]() -> f32 {
            std::optional<Velocity*> vel = entity.tryGet<Velocity>();
            if (!vel) {
                return 0;
            }
            return vel.value()->stable.len();
        }();
        f32 epsilon = speed > 1 ? CHECKPOINT_DISTANCE_THRESHOLD * speed * 0.1 : CHECKPOINT_DISTANCE_THRESHOLD;

        if (rails.isWaiting) {
            // waiting at checkpoint
            if (rails.curTarget != 0 || rails.isCycle) {
                if (rails.curActionTime >= rails.waitTime) {
                    rails.step();
                    rails.isWaiting = false;
                } else {
                    rails.curActionTime += dt;
                }
            }

        } else if (!rails.isMoving) {
            // start moving to next checkpoint

            // TODO should be able to move this to isWaiting block, but getting bugs
            rails.isVelocityUpdateNeeded = true;
            rails.isMoving = true;
            rails.curActionTime = 0;
            entity.add<Velocity>(Velocity(delta.norm() * rails.getSpeed(position.e, inv_dt)));

        } else if (distance < epsilon) {
            // got to checkpoint, clamp to exact position
            if (std::optional<SolidBody*> sb = entity.tryGet<SolidBody>(); sb) {
                sb.value()->collider.move(delta.x(), delta.y(), true);
                entity.set(Position(sb.value()->collider.getCollider().getPositionEdge(Vector::unitiDown)));
            } else if (std::optional<RigidBody*> rb = entity.tryGet<RigidBody>(); rb) {
                rb.value()->collider.moveDirection(true, delta.x(), nullptr);
                rb.value()->collider.moveDirection(false, delta.y(), nullptr);
                entity.set(Position(rb.value()->collider.getCollider().getPositionEdge(Vector::unitiDown)));
            } else {
                entity.set(Position(rails.getTarget().position));
            }

            entity.remove<Velocity>();
            rails.isWaiting = true;
            rails.isMoving = false;

        } else {
            // moving to next checkpoint
            if (rails.isVelocityUpdateNeeded) {
                f32 speed = rails.getSpeed(position.e, inv_dt);
                entity.set(Velocity(delta.norm() * speed));
            }
            rails.curActionTime += dt;
        }
    }
}

}  // namespace whal
