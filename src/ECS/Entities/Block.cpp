#include "Block.h"

#include "ECS/Components/Collision.h"
#include "ECS/Components/Draw.h"
#include "ECS/Components/Transform.h"
#include "ECS/Lib/ECS.h"
#include "Gfx/GfxUtil.h"
#include "Util/Print.h"
#include "Util/Vector.h"

namespace whal {

Expected<ecs::Entity> createBlock(Transform transform) {
    auto& ecs = ecs::ECS::getInstance();

    auto expected = ecs.entity();
    if (!expected.isExpected()) {
        return expected;
    }
    auto block = expected.value();

    block.add(transform);
    block.add<Draw>();

    const s32 widthTileHL = PIXELS_PER_TEXEL * TEXELS_PER_TILE / 2;
    const s32 heightTileHL = PIXELS_PER_TEXEL * TEXELS_PER_TILE / 2;
    block.add(SolidCollider(toFloatVec(transform.position) + Vector2f(0, heightTileHL), Vector2i(widthTileHL, heightTileHL)));

    return block;
}

Expected<ecs::Entity> createBlock(Transform transform, Draw draw) {
    auto& ecs = ecs::ECS::getInstance();

    auto expected = ecs.entity();
    if (!expected.isExpected()) {
        return expected;
    }
    auto block = expected.value();

    block.add(transform);
    block.add(draw);

    const s32 widthTileHL = PIXELS_PER_TEXEL * TEXELS_PER_TILE / 2;
    const s32 heightTileHL = PIXELS_PER_TEXEL * TEXELS_PER_TILE / 2;
    block.add(SolidCollider(toFloatVec(transform.position) + Vector2f(0, heightTileHL), Vector2i(widthTileHL, heightTileHL)));

    return block;
}

Expected<ecs::Entity> createBlock(Transform transform, Sprite sprite) {
    auto& ecs = ecs::ECS::getInstance();

    auto expected = ecs.entity();
    if (!expected.isExpected()) {
        return expected;
    }
    auto block = expected.value();

    block.add(transform);
    block.add(sprite);

    const s32 widthTileHL = PIXELS_PER_TEXEL * TEXELS_PER_TILE / 2;
    const s32 heightTileHL = PIXELS_PER_TEXEL * TEXELS_PER_TILE / 2;
    block.add(SolidCollider(toFloatVec(transform.position) + Vector2f(0, heightTileHL), Vector2i(widthTileHL, heightTileHL)));

    return block;
}

Expected<ecs::Entity> createDecal(Transform transform, Sprite sprite) {
    auto& ecs = ecs::ECS::getInstance();

    auto expected = ecs.entity();
    if (!expected.isExpected()) {
        return expected;
    }
    auto decal = expected.value();

    decal.add(transform);
    decal.add(sprite);
    return decal;
}

}  // namespace whal
