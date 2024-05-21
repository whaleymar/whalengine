#include "Game/DebugScene.h"

#include "ECS/Components/Callback.h"
#include "ECS/Components/Collision.h"
#include "ECS/Components/Draw.h"
#include "ECS/Components/Name.h"
#include "ECS/Components/RailsControl.h"
#include "ECS/Components/RigidBody.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/TriggerZone.h"
#include "ECS/Components/Velocity.h"
#include "ECS/Entities/Block.h"
#include "ECS/Entities/Camera.h"
#include "ECS/Entities/Player.h"
#include "ECS/Lib/ECS.h"
#include "Game/Game.h"
#include "Gfx/Color.h"
#include "Gfx/GLResourceManager.h"
#include "Gfx/GfxUtil.h"
#include "Map/Level.h"

void createTestPlatform();
void createTestTrigger();
void createTestSemiSolid();
void createDepthTest();
void createTestMouseTracker();

std::optional<Error> loadMap() {
    using namespace whal;

    const char* scenefile = "testworld.world";
    return Game::instance().loadScene(scenefile);
}

std::optional<Error> loadTestMap() {
    auto player = whal::createPlayer();
    if (!player.isExpected()) {
        return player.error();
    }
    // createTestPlatform();
    auto err = loadMap();
    createTestPlatform();
    // createTestMouseTracker();
    // createTestTrigger();
    // createTestSemiSolid();
    // createDepthTest();
    return err;
}

std::optional<Error> loadDebugScene() {
    using namespace whal;

    // auto player = createPlayer().value();
    // player.remove<PlayerControlRB>();

    auto player = createPlayer();

    // TODO copied play gets different momentum sometimes?
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
        Depth d = i % 2 == 0 ? Depth::Foreground1 : Depth::Background1;
        auto invisBlock =
            createBlock(Transform::tiles(i, 2),
                        Sprite(d, GLResourceManager::getInstance().getTexture(TEXNAME_SPRITE).getFrame("tile/dirtblock").value(), Color::EMERALD))
                .value();
        invisBlock.remove<SolidCollider>();
        auto invisBlock2 = createBlock(Transform::tiles(i - 5, 2), Draw(Color::EMERALD, {8, 8}, d)).value();
        invisBlock2.remove<SolidCollider>();
    }

    auto platform = createBlock(Transform::tiles(5, 1)).value();
    auto pathControl = RailsControl(
        14,
        {
            {Transform::tiles(5, 1).position, RailsControl::Movement::LINEAR}, {Transform::tiles(5, 15).position, RailsControl::Movement::EASEI_CUBE},
            // {Transform::tiles(15, 15).position, RailsControl::Movement::EASEI_CUBE},
        },
        2, true);
    platform.add<RailsControl>(pathControl);

    auto platformClone = createBlock(Transform::tiles(6, 1)).value();
    auto pathControlClone = RailsControl(
        14,
        {
            {Transform::tiles(6, 1).position, RailsControl::Movement::LINEAR}, {Transform::tiles(6, 15).position, RailsControl::Movement::LINEAR},
            // {Transform::tiles(5, 15).position, RailsControl::Movement::EASEIO_BEZIER},
            // {Transform::tiles(15, 15).position, RailsControl::Movement::EASEI_CUBE},
        },
        2, true);
    platformClone.add<RailsControl>(pathControlClone);

    auto rightPlatform = createBlock(Transform::tiles(36, 1)).value();
    rightPlatform.add(RailsControl(4,
                                   {
                                       {Transform::tiles(36, 1).position, RailsControl::Movement::LINEAR},
                                       {Transform::tiles(36, 7).position, RailsControl::Movement::LINEAR},
                                   },
                                   2));

    return std::nullopt;
}

void startRailsMovement(ecs::Entity self, ecs::Entity other, IUseCollision* selfCollider, IUseCollision* otherCollider, Vector2i moveNormal) {
    auto& rails = self.get<RailsControl>();
    if (rails.isWaiting && rails.curTarget == 0) {
        rails.startManually();
    }
}

void killEntityCallback(ecs::Entity self, ecs::Entity other, IUseCollision* selfCollider, IUseCollision* otherCollider, Vector2i moveNormal) {
    other.kill();
}

void createTestPlatform() {
    for (s32 x = 2; x < 6; x += 3) {
        auto trans = Transform::tiles(x, -15);
        auto platform = createBlock(trans).value();
        auto pathControl = RailsControl(112,
                                        {
                                            {Transform::tiles(x, -15).position, RailsControl::Movement::LINEAR},
                                            {Transform::tiles(x, -10).position, RailsControl::Movement::EASEI_CUBE},
                                        },
                                        2, false);
        platform.add<RailsControl>(pathControl);
        platform.add(Name("callback platform"));
        platform.get<SolidCollider>().setCollisionCallback(&startRailsMovement);
        // platform.get<SolidCollider>().setCollisionCallback(&killEntityCallback);

        // platform.remove<SolidCollider>();
        // platform.add(SemiSolidCollider(trans, Vector2i(8, 8), Material::None, &startRailsMovement));
    }
}

void createTestTrigger() {
    // TriggerCallback callback = [](ecs::Entity entity) { System::audio.play(Sfx::ENEMY_CRY); };
    TriggerCallback callback = [](ecs::Entity self, ecs::Entity other) { other.kill(); };

    // TriggerZone trigger = TriggerZone(Transform::tiles(5, -5), {4, 4}, callback);
    // TriggerZone trigger = TriggerZone(Transform::tiles(5, -9), {4, 4}, nullptr);
    TriggerZone trigger = TriggerZone(Transform::tiles(2, -9), {8, 8}, nullptr, callback);
    auto newEntity = ecs::ECS::getInstance().entity().value();
    newEntity.add(trigger);
}

void createTestSemiSolid() {
    auto newEntity = ecs::ECS::getInstance().entity().value();
    newEntity.add(Draw(RGB(0.34, 0.5, 0.9)));
    Transform trans = Transform::tiles(18, 10);
    // Transform trans = Transform::tiles(10, -14);
    // auto pathControl = RailsControl(64,
    //                                 {
    //                                     {Transform::tiles(10, -14).position, RailsControl::Movement::LINEAR},
    //                                     {Transform::tiles(10, -10).position, RailsControl::Movement::LINEAR},
    //                                 },
    //                                 2, true);
    // auto pathControl = RailsControl(64,
    //                                 {
    //                                     {Transform::tiles(10, -14).position, RailsControl::Movement::LINEAR},
    //                                     {Transform::tiles(12, -14).position, RailsControl::Movement::LINEAR},
    //                                 },
    //                                 1, true);
    newEntity.add(trans);
    // newEntity.add(pathControl);
    newEntity.add<Velocity>();
    newEntity.add<RigidBody>();
    auto collider = SemiSolidCollider(trans, Vector2i(8, 8), Material::None, nullptr);
    newEntity.add(collider);

    newEntity = ecs::ECS::getInstance().entity().value();
    newEntity.add(Draw(RGB(1.0, 0.5, 0.9)));
    trans = Transform::tiles(18, 0);
    newEntity.add(trans);
    newEntity.add<Velocity>();
    newEntity.add<RigidBody>();
    newEntity.add(collider);
}

void createDepthTest() {
    auto newEntity = ecs::ECS::getInstance().entity().value();
    newEntity.add(Draw(RGB(0.2, 0.2, 1.0), {8, 8}, Depth::Background1));
    newEntity.add(Transform::tiles(7, -14));

    newEntity = ecs::ECS::getInstance().entity().value();
    newEntity.add(Draw(RGB(0.2, 0.5, .8), {8, 8}, Depth::Foreground1));
    newEntity.add(Transform::tiles(8, -14));
}

void createTestMouseTracker() {
    auto newEntity = ecs::ECS::getInstance().entity().value();
    newEntity.add<Transform>();
    newEntity.add(Draw(RGB(0.2, 0.5, .6), {8, 8}, Depth::Debug));

    auto setPositionToCamera = [](ecs::Entity entity) { entity.set(Transform(screenToWorldCoords(System::input.MousePosition))); };
    newEntity.add(OnFrameEnd(setPositionToCamera, false));
}
