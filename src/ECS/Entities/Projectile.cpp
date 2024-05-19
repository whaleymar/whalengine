#include "Projectile.h"

#include "ECS/Components/Collision.h"
#include "ECS/Components/Draw.h"
#include "ECS/Components/Lifetime.h"
#include "ECS/Components/Name.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Velocity.h"
#include "ECS/Entities/Explosion.h"
#include "ECS/Lib/ECS.h"
#include "Gfx/GfxUtil.h"
#include "Physics/IUseCollision.h"

constexpr f32 PROJECTILE_LIFETIME_SECONDS = 10.0f;
constexpr s32 PROJECTILE_EXPLOSION_RADIUS = whal::PIXELS_PER_TILE * 1.5;

namespace whal {

void Explode(ecs::Entity self, ecs::Entity other, IUseCollision* selfCollider, IUseCollision* otherCollider, Vector2i moveNormal) {
    // TODO
    // sfx
    // particles

    // make 2 knockback zones. One centered at the point of collision, and one with its far edge aligned with the collision edge
    // makeExplosionZone(selfCollider->getCollider().getPositionEdge(moveNormal) - moveNormal * PROJECTILE_EXPLOSION_RADIUS,
    //                   PROJECTILE_EXPLOSION_RADIUS);

    // nvm just make one big zone
    makeExplosionZone(selfCollider->getCollider().getPositionEdge(moveNormal), PROJECTILE_EXPLOSION_RADIUS);
    self.kill();
}

Expected<ecs::Entity> makeProjectile(Vector2i position, Vector2f velocity) {
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
    entity.add(ActorCollider(trans, {halflenPixels, halflenPixels}, Material::None, &Explode, true));
    entity.add(Name("PROJECTILE"));
    entity.add(Lifetime(PROJECTILE_LIFETIME_SECONDS));

    return entity;
}

}  // namespace whal
