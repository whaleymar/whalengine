#include "Rails.h"

#include "ECS/Components/Collision.h"
#include "ECS/Components/RailsControl.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Velocity.h"

#include "Systems/System.h"
#include "Util/Print.h"
#include "Util/Vector.h"

namespace whal {

constexpr f32 CHECKPOINT_DISTANCE_THRESHOLD = 4;  // in pixels

void RailsSystem::update() {
    f32 dt = System::dt();
    f32 inv_dt = 1 / dt;
    for (auto& [entityid, entity] : getEntities()) {
        auto& rails = entity.get<RailsControl>();
        if (!rails.isValid()) {
            print("skipping invalid RailsControl component for entity", entityid);
            continue;
        }
        auto& transform = entity.get<Transform>();

        const Vector2f delta = toFloatVec(rails.getTarget().position - transform.position);
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
            if (rails.curTarget != 0 || rails.isCycle || rails.isVelocityUpdateNeeded) {
                if (rails.curActionTime >= rails.waitTime) {
                    // start moving
                    rails.step();
                    rails.isWaiting = false;

                    rails.isVelocityUpdateNeeded = true;
                    rails.curActionTime = 0;
                    entity.add<Velocity>(
                        Velocity(toFloatVec(rails.getTarget().position - transform.position).norm() * rails.getSpeed(transform.position, inv_dt)));
                } else {
                    rails.curActionTime += dt;
                }
            }

        } else if (distance < epsilon) {
            // got to checkpoint, clamp to exact position
            if (std::optional<SolidCollider*> sb = entity.tryGet<SolidCollider>(); sb) {
                sb.value()->move(delta.x(), delta.y(), true);
                entity.set(Transform(sb.value()->getCollider().getPositionEdge(Vector2i::unitDown)));
            } else if (std::optional<ActorCollider*> actor = entity.tryGet<ActorCollider>(); actor) {
                actor.value()->moveX(delta, nullptr);
                actor.value()->moveY(delta, nullptr);
                entity.set(Transform(actor.value()->getCollider().getPositionEdge(Vector2i::unitDown)));
            } else {
                entity.set(Transform(rails.getTarget().position));
            }

            entity.remove<Velocity>();
            rails.isWaiting = true;
            rails.isVelocityUpdateNeeded = false;

        } else {
            // moving to next checkpoint
            if (rails.isVelocityUpdateNeeded) {
                f32 speed = rails.getSpeed(transform.position, inv_dt);
                entity.set(Velocity(delta.norm() * speed));
            }
            rails.curActionTime += dt;
        }
    }
}

}  // namespace whal
