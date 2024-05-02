#include "RelationshipManager.h"

#include "ECS/Components/Relationships.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Velocity.h"
#include "Game/Events.h"
#include "Gfx/GfxUtil.h"
#include "Util/MathUtil.h"
#include "Util/Print.h"
#include "Util/Vector.h"

namespace whal {

EntityChildSystem::EntityChildSystem() : mEntityDeathListener(&removeEntityFromChildList) {
    System::eventMgr.registerListener(Event::DEATH_EVENT, mEntityDeathListener);
}

void EntityChildSystem::onRemove(ecs::Entity entity) {
    std::vector<ecs::Entity> childrencopy = std::move(entity.get<Children>().entities);
    for (auto childEntity : childrencopy) {
        childEntity.kill();
    }
}

void FollowSystem::update() {
    for (auto [entityid, entity] : getEntities()) {
        Transform trans = entity.get<Transform>();
        auto& follow = entity.get<Follow>();
        if (!follow.isTargetInitialized) {
            follow.initTarget(entity);
        }
        Transform targetTrans = follow.targetEntity.get<Transform>();

        // consider target speed if it has the component and adjust lookahead to be smaller for low speeds
        f32 lookAheadX = follow.lookAheadTexels.x();
        f32 lookAheadY = follow.lookAheadTexels.y();
        bool isTargetMovingX = false;
        bool isTargetMovingY = false;
        if (auto velOpt = follow.targetEntity.tryGet<Velocity>(); velOpt) {
            f32 velx = velOpt.value()->total.x();
            f32 vely = velOpt.value()->total.y();
            lookAheadX *= clamp(abs(velx) * 0.1f, 0.0f, 1.0f);
            lookAheadY *= clamp(abs(vely) * 0.1f, 0.0f, 1.0f);
            isTargetMovingX = abs(velx) > 1;
            isTargetMovingY = vely != 0;
        }

        s32 direction = targetTrans.facing == Facing::Right ? 1 : -1;
        s32 target;
        if (follow.isMovingX && isTargetMovingX) {
            target = targetTrans.position.x() + direction * lookAheadX * SPIXELS_PER_TEXEL;
        } else {
            target = targetTrans.position.x();
        }
        s32 min = follow.boundsXTexels.e[0] * SPIXELS_PER_TEXEL;
        s32 max = follow.boundsXTexels.e[1] * SPIXELS_PER_TEXEL;
        s32 currentTarget = clamp(target, min, max);
        s32 distanceFromTarget = abs(currentTarget - trans.position.x()) * SPIXELS_PER_TEXEL;

        // if the target is not moving, we shouldn't move away from it
        if (follow.isMovingX && !isTargetMovingX &&
            ((targetTrans.position.x() <= trans.position.x() <= follow.currentTarget.x()) ||
             (targetTrans.position.x() >= trans.position.x() >= follow.currentTarget.x()))) {
            follow.currentTarget.e[0] = trans.position.x();
            follow.isMovingX = false;
        } else if (distanceFromTarget > follow.deadZoneTexels.x()) {
            follow.currentTarget.e[0] = currentTarget;
            follow.isMovingX = true;
        }

        if (follow.isMovingY && isTargetMovingY) {
            target = targetTrans.position.y() + direction * lookAheadY * SPIXELS_PER_TEXEL;
        } else {
            target = targetTrans.position.y();
        }
        min = follow.boundsYTexels.e[0] * SPIXELS_PER_TEXEL;
        max = follow.boundsYTexels.e[1] * SPIXELS_PER_TEXEL;
        currentTarget = clamp(target, min, max);
        distanceFromTarget = abs(currentTarget - trans.position.y()) * SPIXELS_PER_TEXEL;

        if (distanceFromTarget > follow.deadZoneTexels.y()) {
            follow.currentTarget.e[1] = currentTarget;
            follow.isMovingY = true;
        }

        Velocity& vel = entity.get<Velocity>();
        f32 targetSpeedX = static_cast<f32>(follow.currentTarget.x() - trans.position.x()) * TILES_PER_PIXEL;
        f32 targetSpeedY = static_cast<f32>(follow.currentTarget.y() - trans.position.y()) * TILES_PER_PIXEL;

        if (abs(targetSpeedX) > abs(vel.stable.x())) {
            targetSpeedX = lerp(vel.stable.x(), targetSpeedX, follow.damping.x());
        }
        if (abs(targetSpeedY) > abs(vel.stable.y())) {
            targetSpeedY = lerp(vel.stable.y(), targetSpeedY, follow.damping.y());
        }

        vel.stable = {targetSpeedX, targetSpeedY};
        vel.stable *= {2, 2};

        // don't go too slow
        f32 minspeed = 1.91;  // min speed for rounding to not zero at 60fps
        if (vel.stable.x() > 0 && vel.stable.x() < minspeed) {
            vel.stable.e[0] = minspeed;
        } else if (vel.stable.x() < 0 && vel.stable.x() > -minspeed) {
            vel.stable.e[0] = -minspeed;
        }

        if (vel.stable.x() == 0 || !isTargetMovingX) {
            follow.isMovingX = false;
        }

        if (vel.stable.y() > 0 && vel.stable.y() < minspeed) {
            vel.stable.e[1] = minspeed;
        } else if (vel.stable.y() < 0 && vel.stable.y() > -minspeed) {
            vel.stable.e[1] = -minspeed;
        } else if (vel.stable.y() == 0) {
            follow.isMovingY = false;
        }

        follow.debugTargetTracker.set(Transform(follow.currentTarget));
        follow.debugPositionTracker.set(trans);
    }
}

void removeEntityFromChildList(ecs::Entity entity) {
    for (auto [entityid, parent] : EntityChildSystem::instance()->getEntities()) {
        auto& children = parent.get<Children>();
        auto it = std::find(children.entities.begin(), children.entities.end(), entity);
        if (it != children.entities.end()) {
            children.entities.erase(it);
        }
    }
}

}  // namespace whal
