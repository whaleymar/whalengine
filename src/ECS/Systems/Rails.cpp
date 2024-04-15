#include "Rails.h"

#include "ECS/Components/Position.h"
#include "ECS/Components/RailsControl.h"
// #include "ECS/Components/RigidBody.h"
#include "ECS/Components/SolidBody.h"
#include "ECS/Components/Velocity.h"

#include "Systems/Deltatime.h"
#include "Util/MathUtil.h"
#include "Util/Vector.h"

namespace whal {

constexpr f32 CHECKPOINT_DISTANCE_THRESHOLD = 4;  // in pixels

void clampSolidToTarget(ecs::Entity entity, SolidBody* sb, Vector2f move) {
    sb->collider.move(move.x(), move.y(), true);
    entity.set(Position(sb->collider.getCollider().getPositionEdge(Vector::unitiDown)));
}

void RailsSystem::update() {
    f32 dt = Deltatime::getInstance().get();
    for (auto& [entityid, entity] : getEntities()) {
        auto& rails = entity.get<RailsControl>();
        auto& position = entity.get<Position>();

        Vector2f delta = toFloatVec(rails.getTarget().e - position.e);
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
            // if (rails.isVelocityUpdateNeeded) {
            //     entity.remove<Velocity>();
            //     rails.isVelocityUpdateNeeded = false;
            // }
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

            rails.invSegmentDistance = 1 / distance;
            // TODO check for move type before setting init speed
            rails.isVelocityUpdateNeeded = true;
            entity.add<Velocity>(Velocity(delta.norm() * rails.maxSpeed * 0.5));
            rails.isMoving = true;
            rails.curActionTime = 0;
        } else if (distance < epsilon) {
            // got to checkpoint, clamp to exact position
            if (std::optional<SolidBody*> sb = entity.tryGet<SolidBody>(); sb) {
                clampSolidToTarget(entity, *sb, delta);
            } else {
                entity.set(rails.getTarget());  // TODO setting position here puts platform inside of actors without calling move
            }

            entity.set(Velocity());
            rails.isWaiting = true;
            rails.isMoving = false;
            // rails.isVelocityUpdateNeeded = true;  // need to remove velocity after next physics update
        } else {
            // moving to next checkpoint
            if (rails.isVelocityUpdateNeeded) {
                f32 t = 1 - distance * rails.invSegmentDistance;
                entity.set(Velocity(delta.norm() * easeInCubic(0.5 * rails.maxSpeed, rails.maxSpeed, t)));
            }
            rails.curActionTime += dt;
        }
    }
}

}  // namespace whal
