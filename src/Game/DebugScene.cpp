#include "Game/DebugScene.h"

#include "ECS/Components/Collision.h"
#include "ECS/Components/Draw.h"
#include "ECS/Components/RailsControl.h"
#include "ECS/Components/Transform.h"
#include "ECS/Entities/Block.h"
#include "ECS/Entities/Player.h"
#include "ECS/Lib/ECS.h"
#include "Gfx/Color.h"
#include "Gfx/GLResourceManager.h"
#include "Gfx/GfxUtil.h"

std::optional<Error> loadDebugScene() {
    using namespace whal;

    // auto player = createPlayer().value();
    // player.remove<PlayerControlRB>();

    auto player = createPlayer();

    // auto playerCopyExpected = createPlayer();
    // // player.value().kill();
    // if (playerCopyExpected.isExpected()) {
    //     auto playerCopy = playerCopyExpected.value();
    //     playerCopy.set(Transform::tiles(20, 10));
    //     playerCopy.get<Sprite>().setColor(Color::EMERALD);
    // } else {
    //     return playerCopyExpected.error();
    // }

    for (s32 i = 0; i < 50; i++) {
        createBlock(Transform::tiles(i, 1));
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
            Transform::tiles(i, y),
            Sprite(Depth::Player, GLResourceManager::getInstance().getTexture(TEXNAME_SPRITE).getFrame("tile/dirtblock").value(), Color::MAGENTA));
    }

    for (s32 i = 10; i < 15; i++) {
        Depth d = i % 2 == 0 ? Depth::Foreground : Depth::BackgroundNoParallax;
        auto invisBlock =
            createBlock(Transform::tiles(i, 2),
                        Sprite(d, GLResourceManager::getInstance().getTexture(TEXNAME_SPRITE).getFrame("tile/dirtblock").value(), Color::EMERALD))
                .value();
        invisBlock.remove<SolidCollider>();
        auto invisBlock2 = createBlock(Transform::tiles(i - 5, 2), Draw(Color::EMERALD, {8, 8}, d)).value();
        invisBlock2.remove<SolidCollider>();
    }

    // createBlock(Transform::tiles(6, 15), Draw(Color::RED, {8, 8}));
    // createBlock(Transform::tiles(6, 16), Draw(Color::RED, {8, 8}));
    // auto myBlock = createBlock(Transform::tiles(6, 15), Draw(Color::RED, {8, 8})).value();
    //
    // myBlock.add<RigidBody>();
    // myBlock.add<Velocity>();
    //
    // myBlock.add<PlayerControlFree>();
    // myBlock.add<Velocity>();

    auto platform = createBlock(Transform::tiles(5, 1)).value();
    auto pathControl = RailsControl(14,
                                    {
                                        {Transform::tiles(5, 1).position, RailsControl::Movement::LINEAR},
                                        // {Transform::tiles(5, 15).position, RailsControl::Movement::EASEIO_BEZIER},
                                        {Transform::tiles(15, 15).position, RailsControl::Movement::EASEI_CUBE},
                                    },
                                    2, true);
    platform.add<RailsControl>(pathControl);

    auto rightPlatform = createBlock(Transform::tiles(36, 1)).value();
    rightPlatform.add(RailsControl(4,
                                   {
                                       {Transform::tiles(36, 1).position, RailsControl::Movement::LINEAR},
                                       {Transform::tiles(36, 7).position, RailsControl::Movement::LINEAR},
                                   },
                                   2));

    return std::nullopt;
}
