// clang-format off
#include <glad/gl.h>
#include <GLFW/glfw3.h>  // always include after glad
// clang-format on

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

#include "Physics/CollisionManager.h"
#include "Systems/Deltatime.h"
#include "Systems/Frametracker.h"
#include "Systems/InputHandler.h"

#include "Util/Print.h"

#include "Game/DebugScene.h"

using namespace whal;

void MainLoop(GLFWwindow* window) {
    auto& ecs = ecs::ECS::getInstance();
    auto controlSystemRB = ecs.registerSystem<ControllerSystemRB>();
    auto controlSystemFree = ecs.registerSystem<ControllerSystemFree>();
    auto pathSystem = ecs.registerSystem<RailsSystem>();
    auto physicsSystem = ecs.registerSystem<PhysicsSystem>();
    auto spriteSystem = ecs.registerSystem<SpriteSystem>();
    auto drawSystem = ecs.registerSystem<DrawSystem>();
    auto animationSystem = ecs.registerSystem<AnimationSystem>();

    // single-component systems for running psuedo-destructors:
    auto rigidBodyMgr = ecs.registerSystem<RigidBodyManager>();
    // these don't have update methods:
    auto solidBodyMgr = ecs.registerSystem<SolidBodyManager>();
    auto spriteMgr = ecs.registerSystem<SpriteManager>();
    auto drawMgr = ecs.registerSystem<DrawManager>();

    // load scene
    auto err = loadDebugScene();
    if (err) {
        print("Error loading debug scene: ", err.value());
        return;
    }

    auto& input = Input::getInstance();
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // GL_FILL to go back to normal
    while (!glfwWindowShouldClose(window)) {
        // check inputs

        glfwPollEvents();
        if (input.isPause()) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }

        // update systems
        Deltatime::getInstance().update();
        Frametracker::getInstance().update();
        controlSystemRB->update();
        controlSystemFree->update();
        pathSystem->update();
        physicsSystem->update();
        rigidBodyMgr->update();  // kill entities that are destroyed by the phyiscs sim. this doesn't need to run every frame

        // Render
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        animationSystem->update();

        // draw opaque objects, then objects with transparency (sprites) in order from farthest to closest
        drawSystem->drawEntities();
        spriteSystem->drawEntities();
#ifndef NDEBUG
        if (input.isDebug()) {
            drawColliders();
        }
#endif

        // Swap the screen buffers
        glfwSwapBuffers(window);
    }
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH_PIXELS, WINDOW_HEIGHT_PIXELS, WINDOW_TITLE, NULL, NULL);
    glfwMakeContextCurrent(window);
    if (window == NULL) {
        print("Failed to create GLFW window");
        glfwTerminate();
        return -1;
    }

    glfwSetKeyCallback(window, key_callback);

    int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0) {
        print("Failed to initialize OpenGL context");
        return -1;
    }

    print<Format({"", "\n"})>("Loaded OpenGL ", GLAD_VERSION_MAJOR(version), ".", GLAD_VERSION_MINOR(version));

    // Load Shaders
    auto err = createAndRegisterShader(VERTEX_RGBUV_SHADER_PATH, FRAG_SPRITE_RGB_SHADER_PATH, SHNAME_SPRITE_RGB, VertexInfo::RGBUV);
    if (err) {
        print("Failed to register shader. Got error:\n", *err);
        return -1;
    }

    err = createAndRegisterShader(VERTEX_UV_SHADER_PATH, FRAG_SPRITE_SHADER_PATH, SHNAME_SPRITE, VertexInfo::UV);
    if (err) {
        print("Failed to register shader. Got error:\n", *err);
        return -1;
    }

    err = createAndRegisterShader(VERTEX_RGBUV_SHADER_PATH, FRAG_OUTLINE_SHADER_PATH, SHNAME_DEBUG, VertexInfo::RGBUV);
    if (err) {
        print("Failed to register shader. Got error:\n", *err);
        return -1;
    }

    err = createAndRegisterShader(VERTEX_RGB_SHADER_PATH, FRAG_COLOR_SHADER_PATH, SHNAME_COLOR, VertexInfo::RGB);
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
    glfwTerminate();
    return 0;
}
