#include "Physics.h"

#include "ECS/Components/RigidBody.h"
#include "ECS/Components/SolidBody.h"
#include "Gfx/GfxUtil.h"
#include "Systems/Deltatime.h"

namespace whal {

void PhysicsSystem::update() {
    f32 dt = Deltatime::getInstance().get();
    for (auto& [entityid, entity] : getEntities()) {
        Position& pos = entity.get<Position>();
        Velocity& vel = entity.get<Velocity>();

        // velocity currently in pixels per second
        f32 moveX = vel.e.x() * dt * PIXELS_PER_TEXEL * TEXELS_PER_TILE;
        f32 moveY = vel.e.y() * dt * PIXELS_PER_TEXEL * TEXELS_PER_TILE;

        // I can split this into separate systems if this gets slow
        std::optional<RigidBody*> rb = entity.tryGet<RigidBody>();
        if (rb) {
            // no callback needed when actor moves into solid
            rb.value()->collider.moveDirection(true, moveX, nullptr);
            rb.value()->collider.moveDirection(false, moveY, nullptr);

            pos.e = rb.value()->collider.getCollider().getPosition();
        } else if (std::optional<SolidBody*> sb = entity.tryGet<SolidBody>(); sb) {
            sb.value()->collider.move(moveX, moveY);
            pos.e = sb.value()->collider.getCollider().getPosition();
        } else {
            pos.e += Vector2i(std::round(moveX), std::round(moveY));
        }
    }
}

}  // namespace whal
