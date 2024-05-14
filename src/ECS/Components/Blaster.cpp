#include "ECS/Components/Blaster.h"

#include "ECS/Components/Transform.h"
#include "ECS/Entities/Projectile.h"
#include "Game/Events.h"
#include "Gfx/GfxUtil.h"
#include "Systems/Event.h"
#include "Util/Vector.h"

void onBlasterFired(whal::Vector2i moveNormal) {
    for (auto& [entityid, entity] : ProjectileSystem::getEntitiesRef()) {
        whal::Transform trans = entity.get<whal::Transform>();

        s32 direction;
        if (moveNormal.x() != 0) {
            direction = moveNormal.x();
        } else {
            direction = trans.facing == whal::Facing::Left ? -1 : 1;
        }
        whal::Vector2i offset;
        if (moveNormal.y() != -1) {
            offset.e[1] = 3 * whal::PIXELS_PER_TILE / 2;
        } else {
            offset.e[1] = whal::PIXELS_PER_TILE / 4;
        }

        whal::Vector2f velocity;
        if (moveNormal.x() == 0 && moveNormal.y() == 0) {
            velocity = whal::Vector2f(direction, 0) * entity.get<Blaster>().projectileSpeed;
        } else {
            velocity = whal::toFloatVec(moveNormal).norm() * entity.get<Blaster>().projectileSpeed;
        }

        makeProjectile(trans.position + offset, velocity);
    }
}

ProjectileSystem::ProjectileSystem() : mBlasterEventListener(whal::EventListener<whal::Vector2i>(&onBlasterFired)) {
    whal::System::eventMgr.registerListener(whal::Event::SHOOT_EVENT, mBlasterEventListener);
}
