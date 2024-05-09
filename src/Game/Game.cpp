#include "Game.h"

#include <glad/gl.h>

#include <SDL2/SDL.h>

#include "ECS/Components/Transform.h"
#include "ECS/Entities/Camera.h"
#include "ECS/Systems/CallbackSystem.h"
#include "ECS/Systems/RelationshipManager.h"
#include "ECS/Systems/TagTrackers.h"
#include "ECS/Systems/TriggerSystem.h"
#include "Game/Events.h"
#include "Gfx/GLResourceManager.h"
#include "Gfx/GfxUtil.h"
#include "Gfx/Shader.h"
#include "Gfx/Window.h"
#include "Systems/Event.h"
#include "Systems/InputHandler.h"
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
#include "Util/Vector.h"

constexpr f32 MAX_LOAD_DISTANCE_TEXELS = WINDOW_WIDTH_TEXELS * 3;
static const char* TILED_PROJECT_FILE = "project.tiled-project";

Game::Game() : mEntityDeathListener(EventListener<ecs::Entity>(&removeEntityFromLevel)) {
    System::eventMgr.registerListener(Event::DEATH_EVENT, mEntityDeathListener);
}

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

    err = parseMapProject(TILED_PROJECT_FILE);
    if (err) {
        print("Failed to parse map project file. Got error:\n", *err);
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
    ecs.registerSystem<MovingActorTracker>();  // dependency of TriggerSystem
    auto triggerSystem = ecs.registerSystem<TriggerSystem>();
    auto frameEndSystem = ecs.registerSystem<OnFrameEndSystem>();

    // single-component systems for running psuedo-destructors / updating some global var
    auto actorsMgr = ActorsManager::getInstance();
    auto solidsMgr = SolidsManager::getInstance();
    auto semiSolidsMgr = SemiSolidsManager::getInstance();
    auto followMgr = ecs.registerSystem<FollowSystem>();
    // these don't have update methods:
    auto spriteMgr = ecs.registerSystem<SpriteManager>();
    auto drawMgr = ecs.registerSystem<DrawManager>();
    auto playerMgr = PlayerSystem::instance();
    auto cameraMgr = CameraSystem::instance();
    auto childMgr = EntityChildSystem::instance();

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

    bool isTimeNormal = true;
    mWindow->setFocus();
    actorsMgr->update();
    solidsMgr->update();
    semiSolidsMgr->update();
    while (true) {
        pollEvents();
        if (System::input.isQuit()) {
            System::audio.end();
            System::schedule.end();
            break;
        }

        if (System::frame.getFrame() == 0) {
            Vector2f cameraPos = toFloatVec(getCameraPosition());
            updateLoadedLevels(cameraPos);
            // print(cameraPos);
        }

        // randomFallingTile(1);

        // update systems
        System::dt.update();
        System::schedule.tick(System::dt());
        mWindow->updateFPS(System::frame.update());
        controlSystemRB->update();
        controlSystemFree->update();
        pathSystem->update();
        followMgr->update();
        physicsSystem->update();
        triggerSystem->update();

        lifetimeSystem->update();

        // Update Scene
        updateLevelCamera();

        // Only rendering remains, so we can do "end of frame" stuff now
        frameEndSystem->update();
        ecs.killEntities();
        actorsMgr->update();
        solidsMgr->update();
        semiSolidsMgr->update();

        // Render
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        animationSystem->update();

        drawSystem->drawEntities();
        spriteSystem->drawEntities();
#ifndef NDEBUG
        if (System::input.isMusicDebug()) {
            System::input.reset(InputType::MUSICTEST);
            // System::audio.stopMusic();
            System::audio.play(music);
        }
        if (System::input.isReloadScene()) {
            System::input.reset(InputType::RELOADSCENE);
            reloadScene();
        }
        if (System::input.isDebug()) {
            drawColliders();
        }
        if (System::input.isTimeDebug()) {
            System::input.reset(InputType::TIMETEST);
            if (isTimeNormal) {
                System::dt.setMultiplier(0.0);
                isTimeNormal = false;
            } else {
                System::dt.setMultiplier(1.0);
                isTimeNormal = true;
            }
        }
        if (System::input.isKillPlayer()) {
            System::input.reset(InputType::KILLPLAYER);
            for (auto [entityid, entity] : PlayerSystem::instance()->getEntitiesRef()) {
                entity.kill();
            }
        }
#endif

        mWindow->swapBuffers();
    }
}

void Game::end() {
    System::audio.await();
    System::schedule.await();
    killListeners();
}

std::optional<Error> Game::loadScene(const char* filename) {
    if (mIsSceneLoaded) {
        unloadScene();
    }

    auto errOpt = parseWorld(filename, mActiveScene);
    if (errOpt) {
        mIsSceneLoaded = false;
        return errOpt;
    }
    Vector2f cameraPos = toFloatVec(getCameraPosition());
    updateLoadedLevels(cameraPos);
    // TODO spawn player at start pos?

    mIsSceneLoaded = true;
    return std::nullopt;
}

void Game::unloadScene() {
    while (!mActiveScene.loadedLevels.empty()) {
        auto& lvl = mActiveScene.loadedLevels.back();
        unloadLevel(lvl);
        mActiveScene.loadedLevels.pop_back();
    }
    std::set<ecs::Entity> toKill = std::move(mActiveScene.childEntities);
    for (auto entity : toKill) {
        entity.kill();
    }
    mIsSceneLoaded = false;
}

std::optional<Error> Game::reloadScene() {
    auto errOpt = loadScene(mActiveScene.name.c_str());
    if (!errOpt) {
        updateLevelCamera(true);
    }
    return errOpt;
}

Scene& Game::getScene() {
    return mActiveScene;
}

void Game::updateLoadedLevels(Vector2f cameraWorldPosPixels) {
    Vector2f cameraWorldPosTexels = cameraWorldPosPixels * TEXELS_PER_PIXEL;
    for (auto lvl : mActiveScene.allLevels) {
        Vector2f lvlCenterPos = lvl.worldPosOriginTexels - lvl.sizeTexels * Vector2f(-0.5, 0.5);
        const bool shouldLoad = (lvlCenterPos - cameraWorldPosTexels).len() <= MAX_LOAD_DISTANCE_TEXELS;
        auto it = std::find(mActiveScene.loadedLevels.begin(), mActiveScene.loadedLevels.end(), lvl);
        const bool isLevelLoaded = it != mActiveScene.loadedLevels.end();

        if (shouldLoad && !isLevelLoaded) {
            std::optional<Error> errOpt = loadLevel(lvl);
            if (errOpt) {
                print(*errOpt);
                continue;
            }
        } else if (!shouldLoad && isLevelLoaded) {
            unloadAndRemoveLevel(*it);
        }
    }
}

void Game::updateLevelCamera(bool overrideCache) {
    if (PlayerSystem::instance()->getEntitiesRef().empty() || !mIsSceneLoaded || CameraSystem::instance()->getEntitiesRef().empty()) {
        return;
    }
    static std::string lastLevel = "default";
    std::string curLevel;
    ecs::Entity player = PlayerSystem::instance()->first();
    ecs::Entity camera = CameraSystem::instance()->first();
    Vector2f playerPosTexels = toFloatVec(player.get<Transform>().position) * TEXELS_PER_PIXEL;
    auto levelOpt = mActiveScene.getLevelAt(playerPosTexels);

    bool doDefaultCamera = false;
    if (!levelOpt) {
        // print("Not within any level boundaries");
        doDefaultCamera = true;
    } else {
        curLevel = levelOpt.value().filepath;
        if (!overrideCache && curLevel == lastLevel) {
            return;
        }
        print("not skipping");
        lastLevel = curLevel;
        Expected<ActiveLevel*> activeOpt = mActiveScene.getLoadedLevel(levelOpt.value());
        if (!activeOpt.isExpected()) {
            print("Couldn't load level. Got error:", activeOpt.error());
            doDefaultCamera = true;
        } else {
            if (activeOpt.value()->cameraFollow) {
                Follow follow = activeOpt.value()->cameraFollow.value();
                follow.targetEntity = player;
                if (camera.has<Follow>()) {
                    camera.set(follow);
                } else {
                    camera.add(follow);
                }
                return;
            } else {
                Vector2i focalPoint = activeOpt.value()->cameraFocalPoint;
                if (camera.has<Follow>()) {
                    camera.remove<Follow>();
                }
                camera.add(createCameraMoveController(camera.get<Transform>().position, focalPoint));
                System::dt.setMultiplier(0.0);
                return;
            }
        }
    }
    if (doDefaultCamera) {
        curLevel = "default";
        if (!overrideCache && curLevel == lastLevel) {
            return;
        }
        lastLevel = curLevel;
        Follow follow = Follow(player);
        if (camera.has<Follow>()) {
            camera.set(follow);
        } else {
            camera.add(follow);
        }
    }
}
