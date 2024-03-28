#include "Physics.h"

#include "Gfx/GfxUtil.h"
#include "Systems/Deltatime.h"

namespace whal {

void PhysicsSystem::update() {
    f32 dt = Deltatime::getInstance().get();
    for (auto& [entityid, entity] : getEntities()) {
        Position& pos = entity.get<Position>();
        Velocity& vel = entity.get<Velocity>();
        RigidBody& rgb = entity.get<RigidBody>();

        // velocity currently in pixels per second
        f32 moveX = vel.e.x() * dt * PIXELS_PER_TEXEL * TEXELS_PER_TILE;
        f32 moveY = vel.e.y() * dt * PIXELS_PER_TEXEL * TEXELS_PER_TILE;

        // no callback needed when actor moves into solid
        rgb.collider.moveDirection(true, moveX, nullptr);
        rgb.collider.moveDirection(false, moveY, nullptr);

        pos.e = rgb.collider.getCollider().getPosition();
    }
}

}  // namespace whal
