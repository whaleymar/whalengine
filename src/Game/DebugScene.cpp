#include "Game/DebugScene.h"

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
#include "Util/Print.h"

void createTestPlatform();
void createTestTrigger();
void createTestSemiSolid();

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
    createTestTrigger();
    createTestSemiSolid();
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

void startRailsMovement(ActorCollider* selfCollider, ecs::Entity actorEntity, HitInfo hitinfo) {
    print("called startRailsMovement");
    ecs::Entity solidEntity = hitinfo.other;
    auto& rails = solidEntity.get<RailsControl>();
    if (rails.isWaiting && rails.curTarget == 0) {
        rails.startManually();
    }
}

void killEntityCallback(ActorCollider* selfCollider, ecs::Entity actorEntity, HitInfo hitinfo) {
    actorEntity.kill();
}

void createTestPlatform() {
    for (s32 x = 2; x < 6; x += 3) {
        auto platform = createBlock(Transform::tiles(x, -15)).value();
        auto pathControl = RailsControl(14,
                                        {
                                            {Transform::tiles(x, -15).position, RailsControl::Movement::LINEAR},
                                            {Transform::tiles(x, -10).position, RailsControl::Movement::EASEI_CUBE},
                                        },
                                        2, false);
        platform.add<RailsControl>(pathControl);
        platform.add(Name("callback platform"));
        platform.get<SolidCollider>().setCollisionCallback(&startRailsMovement);
        // platform.get<SolidCollider>().setCollisionCallback(&killEntityCallback);
    }
}

void createTestTrigger() {
    // TriggerCallback callback = [](ecs::Entity entity) { System::audio.play(Sfx::ENEMY_CRY); };
    TriggerCallback callback = [](ecs::Entity entity) { entity.kill(); };

    // TriggerZone trigger = TriggerZone(Transform::tiles(5, -5), {4, 4}, callback);
    // TriggerZone trigger = TriggerZone(Transform::tiles(5, -9), {4, 4}, nullptr);
    TriggerZone trigger = TriggerZone(Transform::tiles(2, -9), {8, 8}, nullptr, callback);
    auto newEntity = ecs::ECS::getInstance().entity().value();
    newEntity.add(trigger);
}

void createTestSemiSolid() {
    auto newEntity = ecs::ECS::getInstance().entity().value();
    newEntity.add(Draw(RGB(0.5, 0.5, 0.5)));
    Transform trans = Transform::tiles(3, 10);
    newEntity.add(trans);
    newEntity.add<Velocity>();
    newEntity.add<RigidBody>();
    auto collider = SemiSolidCollider(toFloatVec(trans.position + Vector2i(0, 8)), Vector2i(8, 8), Material::None, nullptr);
    newEntity.add(collider);
}
