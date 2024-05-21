#include "Explosion.h"

#include "ECS/Components/AnimUtil.h"
#include "ECS/Components/Collision.h"
#include "ECS/Components/Draw.h"
#include "ECS/Components/Lifetime.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/TriggerZone.h"
#include "ECS/Components/Velocity.h"
#include "ECS/Lib/ECS.h"
#include "ECS/Systems/TagTrackers.h"
#include "Gfx/GfxUtil.h"
#include "Util/MathUtil.h"
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
        const Vector2f pushStrengthMax = {150, 100};

        Velocity& vel = other.get<Velocity>();
        vel.stable += unitDelta * pushStrengthMax * Vector2f(multX, multY);
    };
    auto trigger = TriggerZone(trans, {halflen, halflen}, pushEntityAway);
    entity.add(trigger);

    constexpr f32 lifetime = 0.5;
    constexpr s32 nFrames = 6;
    constexpr f32 frameTime = lifetime / static_cast<f32>(nFrames);
    static const AnimInfo animInfo = {{"effect/explosion", 0, nFrames, frameTime}};
    Animator animator;
    loadAnimations(animator, animInfo);
    animator.setLooping(false);
    entity.add(animator);
    entity.add(Sprite(Depth::Foreground1, animator.getFrame()));

    entity.add(Lifetime(lifetime));

    // scale volume with distance from camera
    f32 distance = toFloatVec(getCameraPosition() - trans.position).len();
    f32 maxVolume = 0.2f;
    f32 maxDistance = 1500.0f;
    f32 volume = easeOutQuad(maxVolume, 0.0f, distance / maxDistance);
    System::audio.play(Sfx::EXPLOSION, volume);

    return entity;
}
