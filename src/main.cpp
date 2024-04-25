#include <glad/gl.h>

#include <SDL2/SDL.h>

#include "ECS/Lib/ECS.h"
#include "ECS/Systems/Animation.h"
#include "ECS/Systems/CollisionManager.h"
#include "ECS/Systems/Controller.h"
#include "ECS/Systems/DrawManager.h"
#include "ECS/Systems/Gfx.h"
#include "ECS/Systems/Lifetime.h"
#include "ECS/Systems/Physics.h"
#include "ECS/Systems/Rails.h"

#include "Game/EventListeners.h"
#include "Gfx/GLResourceManager.h"
#include "Gfx/GfxUtil.h"
#include "Gfx/Window.h"

#include "Systems/Audio.h"
#include "Systems/System.h"

#include "Util/Print.h"

#include "Game/DebugScene.h"
#include "Game/StressTest.h"

using namespace whal;

void MainLoop(Window& window) {
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

    // load scene
    auto err = loadDebugScene();
    if (err) {
        print("Error loading debug scene: ", err.value());
        return;
    }

    window.setFocus();

    if (!System::audio.isValid()) {
        print("Error initializing audio manager");
        return;
    }
    System::audio.start();
    System::schedule.start();
    startListeners();

    // std::optional<Error> errOpt;
    // Music music;
    // errOpt = music.load("data/provingGroundsTheme.mp3");
    // if (errOpt) {
    //     print(errOpt.value());
    //     return;
    // }
    // System::audio.play(music);

    while (true) {
        pollEvents();
        if (System::input.isQuit()) {
            System::audio.end();
            System::schedule.end();
            break;
        }
        randomFallingTile();
        if (System::frame.getFrame() == 0) {
            print(ecs.getEntityCount());
        }

        // update systems
        System::dt.update();
        System::schedule.tick(System::dt());
        window.updateFPS(System::frame.update());
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
            // System::audio.play(music);
        }
        if (System::input.isDebug()) {
            drawColliders();
        }
#endif

        lifetimeSystem->update();
        window.swapBuffers();
    }
    System::audio.await();
    System::schedule.await();
    killListeners();
}

int main() {
    auto window = Window(WINDOW_TITLE);
    if (!window.init(SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH_PIXELS_ACTUAL, WINDOW_HEIGHT_PIXELS_ACTUAL)) {
        return -1;
    }

    // Load Shaders
    auto err = createAndRegisterShader(VERTEX_RGBUV_SHADER_PATH, FRAG_SPRITE_RGB_SHADER_PATH, ShaderType::SpriteRGB, VertexInfo::RGBUV);
    if (err) {
        print("Failed to register shader. Got error:\n", *err);
        return -1;
    }

    err = createAndRegisterShader(VERTEX_UV_SHADER_PATH, FRAG_SPRITE_SHADER_PATH, ShaderType::SpritePlain, VertexInfo::UV);
    if (err) {
        print("Failed to register shader. Got error:\n", *err);
        return -1;
    }

    err = createAndRegisterShader(VERTEX_RGBUV_SHADER_PATH, FRAG_OUTLINE_SHADER_PATH, ShaderType::Debug, VertexInfo::RGBUV);
    if (err) {
        print("Failed to register shader. Got error:\n", *err);
        return -1;
    }

    err = createAndRegisterShader(VERTEX_RGB_SHADER_PATH, FRAG_COLOR_SHADER_PATH, ShaderType::RGBonly, VertexInfo::RGB);
    if (err) {
        print("Failed to register shader. Got error:\n", *err);
        return -1;
    }

    err = createAndRegisterTexture(SPRITE_TEXTURE_PATH, TEXNAME_SPRITE);
    if (err) {
        print("Failed to register", TEXNAME_SPRITE, "texture. Got error:\n", *err);
        return -1;
    }

    // TODO move to window
    // glViewport(0, 0, WINDOW_WIDTH_PIXELS, WINDOW_HEIGHT_PIXELS); idk what this does
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    // glEnable(GL_DEPTH_TEST);  // not working right
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    MainLoop(window);

    return 0;
}
