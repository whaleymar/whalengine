#include "Projectile.h"

#include "ECS/Components/Collision.h"
#include "ECS/Components/Draw.h"
#include "ECS/Components/Lifetime.h"
#include "ECS/Components/Name.h"
#include "ECS/Components/Radius.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Velocity.h"
#include "ECS/Entities/Explosion.h"
#include "ECS/Lib/ECS.h"
#include "Gfx/GfxUtil.h"
#include "Physics/IUseCollision.h"

namespace whal {

void makeDefaultExplosion(ecs::Entity self) {
    // lifetime's onDeath callback
    f32 explosionRadius = self.get<Radius>().r;
    if (explosionRadius > 0) {
        Vector2i pos = self.get<Transform>().position;
        makeExplosionZone(pos, explosionRadius);
    }
}

void Explode(ecs::Entity self, ecs::Entity other, IUseCollision* selfCollider, IUseCollision* otherCollider, Vector2i moveNormal) {
    // make 2 knockback zones. One centered at the point of collision, and one with its far edge aligned with the collision edge
    // makeExplosionZone(selfCollider->getCollider().getPositionEdge(moveNormal) - moveNormal * PROJECTILE_EXPLOSION_RADIUS,
    //                   PROJECTILE_EXPLOSION_RADIUS);

    // nvm just make one big zone
    f32 explosionRadius = self.get<Radius>().r;
    if (explosionRadius > 0) {
        Vector2i pos = selfCollider->getCollider().getPositionEdge(moveNormal);
        makeExplosionZone(pos, explosionRadius);
    }
    self.kill();
}

void enableCollision(ecs::Entity entity) {
    entity.get<ActorCollider>().setIsCollidable(true);
}

Expected<ecs::Entity> makeProjectile(Vector2i position, Vector2f velocity, f32 lifetimeSeconds, f32 explosionRadius) {
    auto expected = ecs::ECS::getInstance().entity();
    if (!expected.isExpected()) {
        return expected.error();
    }

    auto entity = expected.value();
    Transform trans(position);
    Velocity vel(velocity);
    s32 len = 4;
    s32 halflenPixels = len / 2 * SPIXELS_PER_TEXEL;

    entity.add(trans);
    entity.add(vel);
    entity.add(Draw(RGB::fromInts(200, 0, 0), {len, len}));
    entity.add(Name("PROJECTILE"));
    entity.add(Lifetime(lifetimeSeconds, &makeDefaultExplosion));
    entity.add(Radius(explosionRadius));

    // add collider slightly after creation so it doesn't collide with shooter
    // TODO this still sucks, should use a layer mask or something so it can't collide with shooter
    auto collider = ActorCollider(trans, {halflenPixels, halflenPixels}, Material::None, &Explode, true);
    collider.setIsCollidable(false);
    entity.add<ActorCollider>(collider);
    System::schedule.after(&enableCollision, 0.075, entity);

    return entity;
}

}  // namespace whal