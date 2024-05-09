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

constexpr f32 SPEED_DIVISOR = 1.0f / 40.0f;

void RailsSystem::update() {
    for (auto& [entityid, entity] : getEntitiesRef()) {
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
            if (!vel || (vel.value()->stable.x() == 0 && vel.value()->stable.y() == 0)) {
                return 0;
            }
            return vel.value()->stable.len();
        }();
        f32 epsilon = speed * SPEED_DIVISOR + 1;

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

        } else if (distance <= epsilon) {
            // got to checkpoint, clamp to exact position

            // if entity has collider, use its move function
            if (std::optional<SolidCollider*> sb = entity.tryGet<SolidCollider>(); sb) {
                sb.value()->move(delta.x(), delta.y(), true);
                entity.set(Transform(sb.value()->getCollider().getPositionEdge(Vector2i::unitDown)));
            } else if (std::optional<ActorCollider*> actor = entity.tryGet<ActorCollider>(); actor) {
                actor.value()->moveX(delta, nullptr);
                actor.value()->moveY(delta, nullptr);
                entity.set(Transform(actor.value()->getCollider().getPositionEdge(Vector2i::unitDown)));
            } else if (std::optional<SemiSolidCollider*> semi = entity.tryGet<SemiSolidCollider>(); semi) {
                auto ridingActors = semi.value()->getRidingActors();
                auto ridingSemis = semi.value()->getRidingSemiSolids();

                semi.value()->moveX(delta.x(), nullptr, ridingActors, ridingSemis, true);
                semi.value()->moveY(delta.y(), nullptr, ridingActors, ridingSemis, true);
                entity.set(Transform(semi.value()->getCollider().getPositionEdge(Vector2i::unitDown)));
            } else {
                entity.set(Transform(rails.getTarget().position));
            }

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
                entity.set(Velocity(delta.norm() * speed));
            }
            rails.curActionTime += dt;
        }
    }
}

}  // namespace whal
