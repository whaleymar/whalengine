#include "Explosion.h"

#include "ECS/Components/Collision.h"
#include "ECS/Components/Lifetime.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/TriggerZone.h"
#include "ECS/Components/Velocity.h"
#include "ECS/Lib/ECS.h"
#include "Util/Vector.h"

Expected<whal::ecs::Entity> makeExplosionZone(whal::Vector2i center, s32 halflen) {
    using namespace whal;

    auto eEntity = ecs::ECS::getInstance().entity();
    if (!eEntity.isExpected()) {
        return eEntity;
    }

    auto entity = eEntity.value();

    Transform trans = Transform(center - Vector2i(halflen, 0));
    entity.add(trans);

    TriggerCallback pushEntityAway = [](ecs::Entity self, ecs::Entity other) {
        Vector2i center = self.get<TriggerZone>().center;
        auto normVec = toFloatVec(other.get<ActorCollider>().getCollider().center - center).norm();

        // TODO scale with distance
        constexpr f32 pushStrength = 100.0f;

        Velocity& vel = other.get<Velocity>();
        vel.stable += normVec * pushStrength;
    };
    auto trigger = TriggerZone(trans, {halflen, halflen}, pushEntityAway);
    entity.add(trigger);
    entity.add(Lifetime(0.5));

    return entity;
}
