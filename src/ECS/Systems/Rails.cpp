#include "Rails.h"

#include "ECS/Components/Collision.h"
#include "ECS/Components/RailsControl.h"
#include "ECS/Components/Tags.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Velocity.h"

#include "Systems/System.h"
#include "Util/Print.h"
#include "Util/Vector.h"

namespace whal {

constexpr f32 CHECKPOINT_DISTANCE_THRESHOLD = 4;  // in pixels

void RailsSystem::update() {
    for (auto& [entityid, entity] : getEntitiesCopy()) {
        f32 dt;
        if (entity.has<Camera>()) {
            dt = System::dt.getUnmodified();
        } else {
            dt = System::dt();
        }
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

                    Vector2i newDelta = rails.getTarget().position - transform.position;

                    // prevent divide by zero
                    if (newDelta.x() == 0 && newDelta.y() == 0) {
                        // we're already at target for some reason, so no need to wait again
                        entity.add<Velocity>();
                        rails.curActionTime = rails.waitTime;
                    } else {
                        Velocity velToAdd = Velocity(toFloatVec(newDelta).norm() * rails.getSpeed(transform.position));
                        entity.add<Velocity>(velToAdd);
                    }

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

            // if (entity.has<Name>()) {
            //     print("name:", entity.get<Name>());
            // }
            // print("got to destination in ", rails.curActionTime, "seconds");
            entity.remove<Velocity>();
            rails.isWaiting = true;
            rails.curActionTime = 0;
            rails.isVelocityUpdateNeeded = false;
            if (rails.arrivalCallback != nullptr) {
                rails.arrivalCallback(entity, rails);
            }

        } else {
            // moving to next checkpoint
            if (rails.isVelocityUpdateNeeded) {
                f32 speed = rails.getSpeed(transform.position);
                // f32 speed = rails.getSpeedNew();
                // print("Speed", speed);
                entity.set(Velocity(delta.norm() * speed));
            }
            rails.curActionTime += dt;
        }
    }
}

}  // namespace whal
