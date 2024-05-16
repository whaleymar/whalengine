#include "ECS/Components/Blaster.h"

#include "ECS/Components/Transform.h"
#include "ECS/Components/Velocity.h"
#include "ECS/Entities/Projectile.h"
#include "Game/Events.h"
#include "Gfx/GfxUtil.h"
#include "Systems/Event.h"
#include "Util/Vector.h"

// TODO this shouldn't run in a loop. Make the logic a function that takes an entity & call it from player control system or AI control system
void onBlasterFired(whal::Vector2i moveNormal) {
    using namespace whal;
    for (auto& [entityid, entity] : ProjectileSystem::getEntitiesRef()) {
        Transform trans = entity.get<Transform>();

        s32 shotDirectionX;
        if (moveNormal.x() != 0) {
            shotDirectionX = moveNormal.x();
        } else {
            shotDirectionX = trans.facing == Facing::Left ? -1 : 1;
        }
        Vector2i offset;
        if (moveNormal.y() != -1) {
            offset.e[1] = 3 * PIXELS_PER_TILE / 2;
        } else {
            offset.e[1] = PIXELS_PER_TILE / 4;
        }

        Vector2f velocity;
        Vector2i shotNormal;
        Blaster& blaster = entity.get<Blaster>();
        if (moveNormal.x() == 0 && moveNormal.y() == 0) {
            velocity = Vector2f(shotDirectionX, 0) * blaster.projectileSpeed;
            shotNormal = {shotDirectionX, 0};
        } else {
            velocity = toFloatVec(moveNormal).norm() * blaster.projectileSpeed;
            shotNormal = moveNormal;
        }

        makeProjectile(trans.position + offset, velocity);

        // push shooter in opposite direction of projectile
        if (auto velOpt = entity.tryGet<Velocity>(); velOpt) {
            velOpt.value()->stable += toFloatVec(shotNormal * -1) * blaster.shotKnockback;
        }
    }
}

ProjectileSystem::ProjectileSystem() : mBlasterEventListener(whal::EventListener<whal::Vector2i>(&onBlasterFired)) {
    whal::System::eventMgr.registerListener(whal::Event::SHOOT_EVENT, mBlasterEventListener);
}
