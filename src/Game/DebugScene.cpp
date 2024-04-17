#include "Game/DebugScene.h"

#include "ECS/Components/Draw.h"
#include "ECS/Components/Position.h"
#include "ECS/Components/RailsControl.h"
#include "ECS/Components/SolidBody.h"
#include "ECS/Components/Velocity.h"
#include "ECS/Entities/Block.h"
#include "ECS/Entities/Player.h"
#include "ECS/Lib/ECS.h"
#include "Gfx/Color.h"
#include "Gfx/GLResourceManager.h"
#include "Gfx/GfxUtil.h"

std::optional<Error> loadDebugScene() {
    using namespace whal;

    auto player = createPlayer();

    // auto playerCopyExpected = createPlayer();
    // player.value().kill();  // deleting here makes the collision box wrong! see bug in readme
    // if (playerCopyExpected.isExpected()) {
    //     auto playerCopy = playerCopyExpected.value();
    //     playerCopy.set(Position::tiles(20, 10));
    //     playerCopy.get<Sprite>().setColor(Color::EMERALD);
    // } else {
    //     return playerCopyExpected.error();
    // }

    for (s32 i = 0; i < 50; i++) {
        createBlock(Position::tiles(i, 1));
    }

    // auto tmp = createBlock(Position::tiles(5, 15));
    // if (tmp.isExpected()) {
    //     auto tmpBlock = tmp.value();
    //     tmpBlock.add<PlayerControlFree>();
    //     tmpBlock.add<Velocity>();
    // } else {
    //     print(tmp.error());
    // }

    for (s32 i = 21; i < 50; i++) {
        s32 y = 4;
        if (i < 26) {
            y = i - 19;
        } else if (i % 7 < 4) {
            continue;
        }
        createBlock(
            Position::tiles(i, y),
            Sprite(Depth::Player, GLResourceManager::getInstance().getTexture(TEXNAME_SPRITE).getFrame("tile/dirtblock").value(), Color::MAGENTA));
    }

    for (s32 i = 10; i < 15; i++) {
        Depth d = i % 2 == 0 ? Depth::Foreground : Depth::BackgroundNoParallax;
        auto invisBlock =
            createBlock(Position::tiles(i, 2),
                        Sprite(d, GLResourceManager::getInstance().getTexture(TEXNAME_SPRITE).getFrame("tile/dirtblock").value(), Color::EMERALD))
                .value();
        invisBlock.remove<SolidBody>();
        auto invisBlock2 = createBlock(Position::tiles(i - 5, 2), Draw(d, Color::EMERALD)).value();
        // invisBlock.remove<SolidBody>(); // TODO after collision manager rework, check that deleting here actually removes the solidBody
        invisBlock2.remove<SolidBody>();
    }

    createBlock(Position::tiles(6, 15), Draw(Depth::Player, Color::RED));
    createBlock(Position::tiles(6, 16), Draw(Depth::Player, Color::RED));

    auto platform = createBlock(Position::tiles(5, 1)).value();
    auto pathControl = RailsControl(400,
                                    {
                                        {Position::tiles(5, 1).e, RailsControl::Movement::LINEAR},
                                        {Position::tiles(5, 15).e, RailsControl::Movement::LINEAR},
                                    },
                                    2, true);
    platform.add<RailsControl>(pathControl);

    return std::nullopt;
}
