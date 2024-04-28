#include "Game.h"

#include <glad/gl.h>

#include <SDL2/SDL.h>

#include "Gfx/GLResourceManager.h"
#include "Gfx/GfxUtil.h"
#include "Gfx/Shader.h"
#include "Gfx/Window.h"
#include "Systems/Event.h"
#include "Util/Print.h"

#include "ECS/Lib/ECS.h"
#include "ECS/Systems/Animation.h"
#include "ECS/Systems/CollisionManager.h"
#include "ECS/Systems/Controller.h"
#include "ECS/Systems/DrawManager.h"
#include "ECS/Systems/Gfx.h"
#include "ECS/Systems/Lifetime.h"
#include "ECS/Systems/Physics.h"
#include "ECS/Systems/Rails.h"

#include "Game/DebugScene.h"
#include "Game/EventListeners.h"

#include "Map/Level.h"
#include "Map/Tiled.h"

#include "Systems/Audio.h"
#include "Systems/System.h"

constexpr f32 MAX_LOAD_DISTANCE_TEXELS = WINDOW_WIDTH_TEXELS * 3;

Game::Game() : mEntityDeathListener(EventListener<ecs::Entity>(&removeEntityFromLevel)) {}

bool Game::startup() {
    mWindow = std::make_unique<Window>(WINDOW_TITLE);
    if (!mWindow->init(SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH_PIXELS_ACTUAL, WINDOW_HEIGHT_PIXELS_ACTUAL)) {
        return true;
    }

    // Load Shaders
    auto err = createAndRegisterShader(VERTEX_RGBUV_SHADER_PATH, FRAG_SPRITE_RGB_SHADER_PATH, ShaderType::SpriteRGB, VertexInfo::RGBUV);
    if (err) {
        print("Failed to register shader. Got error:\n", *err);
        return true;
    }

    err = createAndRegisterShader(VERTEX_UV_SHADER_PATH, FRAG_SPRITE_SHADER_PATH, ShaderType::SpritePlain, VertexInfo::UV);
    if (err) {
        print("Failed to register shader. Got error:\n", *err);
        return true;
    }

    err = createAndRegisterShader(VERTEX_RGBUV_SHADER_PATH, FRAG_OUTLINE_SHADER_PATH, ShaderType::Debug, VertexInfo::RGBUV);
    if (err) {
        print("Failed to register shader. Got error:\n", *err);
        return true;
    }

    err = createAndRegisterShader(VERTEX_RGB_SHADER_PATH, FRAG_COLOR_SHADER_PATH, ShaderType::RGBonly, VertexInfo::RGB);
    if (err) {
        print("Failed to register shader. Got error:\n", *err);
        return true;
    }

    err = createAndRegisterTexture(SPRITE_TEXTURE_PATH, TEXNAME_SPRITE);
    if (err) {
        print("Failed to register", TEXNAME_SPRITE, "texture. Got error:\n", *err);
        return true;
    }

    // init systems
    if (!System::audio.isValid()) {
        print("Error initializing audio manager");
        return true;
    }
    System::audio.start();
    System::schedule.start();
    startListeners();

    return false;
}

void Game::mainloop() {
    auto& ecs = ecs::ECS::getInstance();
    auto controlSystemRB = ecs.registerSystem<ControllerSystemRB>();
    auto controlSystemFree = ecs.registerSystem<ControllerSystemFree>();
    auto pathSystem = ecs.registerSystem<RailsSystem>();
    auto physicsSystem = ecs.registerSystem<PhysicsSystem>();
    auto spriteSystem = ecs.registerSystem<SpriteSystem>();
    auto drawSystem = ecs.registerSystem<DrawSystem>();
    auto animationSystem = ecs.registerSystem<AnimationSystem>();
    auto lifetimeSystem = ecs.registerSystem<LifetimeSystem>();

    // single-component systems for running psuedo-destructors / updating some global var
    auto actorsMgr = ActorsManager::getInstance();
    auto solidsMgr = SolidsManager::getInstance();
    // these don't have update methods:
    auto spriteMgr = ecs.registerSystem<SpriteManager>();
    auto drawMgr = ecs.registerSystem<DrawManager>();

    // load scene // TODO separate function
    auto err = loadTestMap();
    if (err) {
        print("Error loading debug scene: ", err.value());
        return;
    }

    std::optional<Error> errOpt;
    Music music;
    errOpt = music.load("data/provingGroundsTheme.mp3");
    if (errOpt) {
        print(errOpt.value());
        return;
    }
    // System::audio.play(music);

    mWindow->setFocus();
    while (true) {
        pollEvents();
        if (System::input.isQuit()) {
            System::audio.end();
            System::schedule.end();
            break;
        }

        if (System::frame.getFrame() == 0) {
            // updateLoadedLevels(ecs.query<Player>()[0]); // TODO need to test this, but don't have a way to get player pos
            //     print(ecs.getEntityCount());
        }

        // randomFallingTile(1);

        // update systems
        System::dt.update();
        System::schedule.tick(System::dt());
        mWindow->updateFPS(System::frame.update());
        controlSystemRB->update();
        controlSystemFree->update();
        pathSystem->update();
        physicsSystem->update();

        // these sync the collision manager
        if (System::frame.getFrame() % 10 == 0) {
            actorsMgr->update();
            solidsMgr->update();
        }

        // Render
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        animationSystem->update();

        // draw opaque objects, then objects with transparency (sprites) in order from farthest to closest
        drawSystem->drawEntities();
        spriteSystem->drawEntities();
#ifndef NDEBUG
        if (System::input.isMusicDebug()) {
            System::input.reset(InputType::MUSICTEST);
            // System::audio.stopMusic();
            System::audio.play(music);
        }
        if (System::input.isDebug()) {
            drawColliders();
        }
#endif

        lifetimeSystem->update();
        mWindow->swapBuffers();
    }
}

void Game::end() {
    System::audio.await();
    System::schedule.await();
    killListeners();
}

std::optional<Error> Game::loadScene(const char* filename) {
    auto errOpt = parseWorld(filename, mActiveScene);
    if (errOpt) {
        mIsSceneLoaded = false;
        return errOpt;
    }
    updateLoadedLevels({0, 0});  // TODO stubbed arg
    // TODO spawn player at start pos?

    mIsSceneLoaded = true;
    return std::nullopt;
}

Scene& Game::getScene() {
    return mActiveScene;
}

void Game::updateLoadedLevels(Vector2f cameraWorldPosTexels) {
    for (auto lvl : mActiveScene.allLevels) {
        const bool shouldLoad = (lvl.worldPosOrigin - cameraWorldPosTexels).len() <= MAX_LOAD_DISTANCE_TEXELS;
        auto it = std::find(mActiveScene.loadedLevels.begin(), mActiveScene.loadedLevels.end(), lvl);
        const bool isLevelLoaded = it != mActiveScene.loadedLevels.end();

        if (shouldLoad && !isLevelLoaded) {
            std::optional<Error> errOpt = loadLevel(lvl);
            if (errOpt) {
                print(*errOpt);
                continue;
            }
        } else if (!shouldLoad && isLevelLoaded) {
            unloadLevel(*it);
        }
    }
}
