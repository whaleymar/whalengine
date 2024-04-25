#pragma once

#include "ECS/Components/Collision.h"
#include "ECS/Components/Draw.h"
#include "ECS/Components/Lifetime.h"
#include "ECS/Components/RigidBody.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Velocity.h"
#include "ECS/Entities/Block.h"
#include "ECS/Lib/ECS.h"
#include "Gfx/Color.h"
#include "Gfx/GfxUtil.h"
#include "Systems/System.h"
#include "Util/Print.h"

namespace whal {

void killEntity(ecs::Entity entity) {
    entity.kill();
}

void randomFallingTile(s32 i = 1) {
    if (i == 0) {
        return;
    }
    f32 r = System::rng.uniform();
    f32 x = r * WINDOW_WIDTH_TEXELS;
    static RGB colorOpts[4] = {Color::RED, Color::WHITE, Color::EMERALD, Color::MAGENTA};
    RGB color = colorOpts[static_cast<s32>(r * 100) % 3];
    auto trans = Transform::texels(x, WINDOW_HEIGHT_TEXELS / 2);

    bool isActor = System::rng.uniform() > 0.9;
    if (isActor) {
        color = Color::MAGENTA;
    }

    auto draw = Draw(color);
    Expected<ecs::Entity> eBlock = createBlock(trans, draw);
    if (!eBlock.isExpected()) {
        print("Got error: ", eBlock.error());
    } else {
        auto block = eBlock.value();
        block.add<RigidBody>();
        f32 yVel = System::rng.uniform();
        if (!isActor) {
            block.add(Lifetime(1.5));
            block.add<Velocity>(Velocity({0, yVel * 50}));
        } else {
            block.remove<SolidCollider>();
            block.add<ActorCollider>();
            block.add<Velocity>(Velocity({0, -20}));
        }
        // System::schedule.after(&killEntity, 1.5, block);
        // System::schedule.after(&ecs::Entity::kill, 1.5, &block);
        // System::schedule.after(&ecs::ECS::kill, 1.5, ecs::ECS::getInstance(), block);
    }
    randomFallingTile(i - 1);
}

}  // namespace whal
