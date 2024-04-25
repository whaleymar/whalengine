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

void randomFallingTile() {
    f32 r = System::rng.uniform();
    f32 x = r * WINDOW_WIDTH_TEXELS;
    static RGB colorOpts[4] = {Color::RED, Color::WHITE, Color::EMERALD, Color::MAGENTA};
    RGB color = colorOpts[static_cast<s32>(4.99 * r)];
    auto trans = Transform::texels(x, WINDOW_HEIGHT_TEXELS);
    auto draw = Draw(color);
    Expected<ecs::Entity> eBlock = createBlock(trans, draw);
    if (!eBlock.isExpected()) {
        print("Got error: ", eBlock.error());
    } else {
        auto block = eBlock.value();
        block.add<RigidBody>();
        block.add<Velocity>();
        // block.remove<SolidCollider>();
        block.add(Lifetime(1.5));
        // System::schedule.after(&killEntity, 1.5, block);
        // System::schedule.after(&ecs::Entity::kill, 1.5, &block);
        // System::schedule.after(&ecs::ECS::kill, 1.5, ecs::ECS::getInstance(), block);
    }
}

}  // namespace whal
