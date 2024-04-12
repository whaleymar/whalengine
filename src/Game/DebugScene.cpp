#include "Game/DebugScene.h"

#include "ECS/Components/Draw.h"
#include "ECS/Components/Position.h"
#include "ECS/Components/Velocity.h"
#include "ECS/Entities/Block.h"
#include "ECS/Entities/Player.h"
#include "ECS/Lib/ECS.h"
#include "Gfx/Color.h"
#include "Gfx/GLResourceManager.h"
#include "Gfx/GfxUtil.h"
#include "Util/Vector.h"

std::optional<Error> loadDebugScene() {
    using namespace whal;

    auto player = createPlayer();
    auto playerCopyExpected = createPlayer();

    // player.value().kill();  // deleting here makes the collision box wrong! see bug in readme
    if (playerCopyExpected.isExpected()) {
        auto playerCopy = playerCopyExpected.value();
        playerCopy.set(Velocity(Vector2f(5.0, 0.0)));
        playerCopy.get<Sprite>().setColor(Color::EMERALD);
    } else {
        return playerCopyExpected.error();
    }

    for (s32 i = 0; i < 50; i++) {
        createBlock(Position::tiles(i, 1));
    }

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

    // auto pathControl = PathControl(10, {}, 2);
    // pathControl.checkpoints.push_back(Position::texels(0, 90));
    // pathControl.checkpoints.push_back(Position::texels(0, 5));
    // entity2.add<PathControl>(pathControl);

    return std::nullopt;
}
