#include "ECS/Components/Blaster.h"

#include "ECS/Components/Transform.h"
#include "ECS/Components/Velocity.h"
#include "ECS/Entities/Projectile.h"
#include "Gfx/GfxUtil.h"
#include "Systems/Event.h"
#include "Util/Vector.h"

void onBlasterFired(whal::Vector2i target) {
    using namespace whal;
    for (auto& [entityid, entity] : ProjectileSystem::getEntitiesRef()) {
        Transform trans = entity.get<Transform>();

        // change where the projectile starts (relative to shooting entity)
        Vector2i offset = {0, PIXELS_PER_TILE};
        Vector2i shotOrigin = trans.position + offset;
        Vector2f moveNormal = toFloatVec(target - shotOrigin).norm();

        Vector2f velocity;
        Blaster& blaster = entity.get<Blaster>();
        velocity = moveNormal * blaster.projectileSpeed;

        // auto totalVel = velocity + entity.get<Velocity>().total;
        auto totalVel = velocity;
        makeProjectile(shotOrigin, totalVel, blaster.projectileLifetimeSeconds, blaster.explosionRadius);

        // push shooter in opposite direction of projectile
        if (auto velOpt = entity.tryGet<Velocity>(); velOpt) {
            velOpt.value()->stable += moveNormal * -1 * blaster.shotKnockback;
        }

        System::audio.play(Sfx::SHOTFIRED, 0.2);
    }
}

// TODO this shouldn't run in a loop. Make the logic a function that takes an entity & call it from player control system or AI control system
// void onBlasterFired(whal::Vector2i moveNormal) {
//     using namespace whal;
//     for (auto& [entityid, entity] : ProjectileSystem::getEntitiesRef()) {
//         Transform trans = entity.get<Transform>();
//
//         s32 shotDirectionX;
//         if (moveNormal.x() != 0) {
//             shotDirectionX = moveNormal.x();
//         } else {
//             shotDirectionX = trans.facing == Facing::Left ? -1 : 1;
//         }
//
//         Vector2f shotDirection = Vector2f(shotDirectionX, moveNormal.y());
//
//         // change where the projectile starts (relative to shooting entity)
//         Vector2i offset = {shotDirectionX, 3 * PIXELS_PER_TILE / 2};
//         if (moveNormal.y() > 0) {
//             offset.e[1] += PIXELS_PER_TILE / 2;
//         } else if (moveNormal.y() < 0) {
//             offset.e[1] -= 2 * PIXELS_PER_TILE;
//         }
//
//         Vector2f velocity;
//         Vector2i shotNormal;
//         Blaster& blaster = entity.get<Blaster>();
//         if (moveNormal.x() == 0 && moveNormal.y() == 0) {
//             velocity = Vector2f(shotDirectionX, 0) * blaster.projectileSpeed;
//             shotNormal = {shotDirectionX, 0};
//         } else {
//             velocity = toFloatVec(moveNormal).norm() * blaster.projectileSpeed;
//             shotNormal = moveNormal;
//         }
//
//         auto shooterVelocity = entity.get<Velocity>().total;
//         makeProjectile(trans.position + offset, velocity + shooterVelocity, blaster.projectileLifetimeSeconds, blaster.explosionRadius);
//
//         // push shooter in opposite direction of projectile
//         if (auto velOpt = entity.tryGet<Velocity>(); velOpt) {
//             velOpt.value()->stable += toFloatVec(shotNormal * -1) * blaster.shotKnockback;
//         }
//
//         System::audio.play(Sfx::SHOTFIRED, 0.2);
//     }
// }

ProjectileSystem::ProjectileSystem() : mBlasterEventListener(whal::EventListener<whal::Vector2i>(&onBlasterFired)) {
    whal::System::eventMgr.registerListener(whal::Event::SHOOT_EVENT, mBlasterEventListener);
}
