#include "Explosion.h"

#include "ECS/Components/Collision.h"
#include "ECS/Components/Lifetime.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/TriggerZone.h"
#include "ECS/Components/Velocity.h"
#include "ECS/Lib/ECS.h"
#include "Gfx/GfxUtil.h"
#include "Util/MathUtil.h"
#include "Util/Print.h"
#include "Util/Vector.h"

Expected<whal::ecs::Entity> makeExplosionZone(whal::Vector2i center, s32 halflen) {
    using namespace whal;

    auto eEntity = ecs::ECS::getInstance().entity();
    if (!eEntity.isExpected()) {
        return eEntity;
    }

    auto entity = eEntity.value();

    Transform trans = Transform(center - Vector2i(0, halflen));
    entity.add(trans);

    TriggerCallback pushEntityAway = [](ecs::Entity self, ecs::Entity other) {
        const AABB& otherCollider = other.get<ActorCollider>().getCollider();

        TriggerZone& trigger = self.get<TriggerZone>();
        Vector2i center = trigger.center;
        Vector2f delta = toFloatVec(otherCollider.center - center);
        auto unitDelta = delta.norm();

        // slight knockback falloff based on distance
        Vector2f distanceTexels = (delta * TEXELS_PER_PIXEL).absolute();
        Vector2f halflenTexels = toFloatVec(trigger.half) * TEXELS_PER_PIXEL;
        f32 multX = std::sqrt(1.0f - clamp(distanceTexels.x() / halflenTexels.x(), 0.0f, 1.0f));
        f32 multY = std::sqrt(1.0f - clamp(distanceTexels.y() / halflenTexels.y(), 0.0f, 1.0f));

        // print("distance2D:", distanceTexels);
        // print("halflen:", halflenTexels);
        // print("mult", multX, multY, "\n");
        constexpr f32 pushStrengthMax = 100.0f;

        Velocity& vel = other.get<Velocity>();
        vel.stable += unitDelta * pushStrengthMax * Vector2f(multX, multY);
    };
    auto trigger = TriggerZone(trans, {halflen, halflen}, pushEntityAway);
    entity.add(trigger);
    entity.add(Lifetime(0.5));

    return entity;
}
