#include "Block.h"

#include "ECS/Components/Draw.h"
#include "ECS/Components/Position.h"
#include "ECS/Components/SolidBody.h"
#include "ECS/Lib/ECS.h"
#include "Gfx/GfxUtil.h"
#include "Util/Vector.h"

namespace whal {

Expected<ecs::Entity> createBlock(Position position) {
    auto& ecs = ecs::ECS::getInstance();

    auto expected = ecs.entity();
    if (!expected.isExpected()) {
        return expected;
    }
    auto block = expected.value();

    block.add(position);
    block.add<Draw>();

    const s32 widthTileHL = PIXELS_PER_TEXEL * 8 / 2;
    const s32 heightTileHL = PIXELS_PER_TEXEL * 8 / 2;
    block.add(SolidBody(toFloatVec(position.e) + Vector2f(0, heightTileHL), widthTileHL, heightTileHL));

    return block;
}

Expected<ecs::Entity> createBlock(Position position, Draw draw) {
    auto& ecs = ecs::ECS::getInstance();

    auto expected = ecs.entity();
    if (!expected.isExpected()) {
        return expected;
    }
    auto block = expected.value();

    block.add(position);
    block.add(draw);

    const s32 widthTileHL = PIXELS_PER_TEXEL * 8 / 2;
    const s32 heightTileHL = PIXELS_PER_TEXEL * 8 / 2;
    block.add(SolidBody(toFloatVec(position.e) + Vector2f(0, heightTileHL), widthTileHL, heightTileHL));

    return block;
}

Expected<ecs::Entity> createBlock(Position position, Sprite sprite) {
    auto& ecs = ecs::ECS::getInstance();

    auto expected = ecs.entity();
    if (!expected.isExpected()) {
        return expected;
    }
    auto block = expected.value();

    block.add(position);
    block.add(sprite);

    const s32 widthTileHL = PIXELS_PER_TEXEL * 8 / 2;
    const s32 heightTileHL = PIXELS_PER_TEXEL * 8 / 2;
    block.add(SolidBody(toFloatVec(position.e) + Vector2f(0, heightTileHL), widthTileHL, heightTileHL));

    return block;
}

}  // namespace whal
