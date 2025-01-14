#include <glad/gl.h>

#include <SDL2/SDL.h>

#include "ECS/Lib/ECS.h"
#include "ECS/Systems/Animation.h"
#include "ECS/Systems/CollisionManager.h"
#include "ECS/Systems/Controller.h"
#include "ECS/Systems/DrawManager.h"
#include "ECS/Systems/Gfx.h"
#include "ECS/Systems/Physics.h"
#include "ECS/Systems/Rails.h"

#include "Gfx/GLResourceManager.h"
#include "Gfx/GfxUtil.h"
#include "Gfx/Window.h"

#include "Systems/System.h"

#include "Util/Print.h"

#include "Game/DebugScene.h"

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

    // SDL_Event event;
    while (true) {
        // check inputs

        // glfwPollEvents();
        pollEvents();
        // while (SDL_PollEvent(&event) != 0) {
        //     print("got event");
        //     if (event.type == SDL_QUIT) {
        //         System::input.set(InputType::QUIT);
        //         break;
        //     }
        //     keyCallback(event);
        // }
        if (System::input.isQuit()) {
            break;
        }

        // update systems
        System::dt.update();
        System::frame.update();
        controlSystemRB->update();
        controlSystemFree->update();
        pathSystem->update();
        physicsSystem->update();

        // these don't need to run every frame:
        // they sync the collision manager
        // and kill crushed actors
        actorsMgr->update();
        solidsMgr->update();

        // Render
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        animationSystem->update();

        // draw opaque objects, then objects with transparency (sprites) in order from farthest to closest
        drawSystem->drawEntities();
        spriteSystem->drawEntities();
#ifndef NDEBUG
        if (System::input.isDebug()) {
            drawColliders();
        }
#endif

        window.swapBuffers();
    }
}

int main() {
    auto window = Window(WINDOW_TITLE);
    if (!window.init(0, 0, WINDOW_WIDTH_PIXELS_ACTUAL, WINDOW_HEIGHT_PIXELS_ACTUAL)) {
        return -1;
    }

    // doesn't work
    // auto prevState = SDL_EventState(SDL_KEYDOWN, SDL_ENABLE);
    // if (prevState == 0) {
    //     print("was false");
    // }

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

    // glViewport(0, 0, WINDOW_WIDTH_PIXELS, WINDOW_HEIGHT_PIXELS); idk what this does
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    // glEnable(GL_DEPTH_TEST);  // not working right
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    MainLoop(window);

    // clear resources
    // glfwTerminate();
    return 0;
}
