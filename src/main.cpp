// clang-format off
#include <glad/gl.h>
#include <GLFW/glfw3.h>  // always include after glad
// clang-format on

#include "ECS/Components/Draw.h"
#include "ECS/Components/Position.h"
#include "ECS/Components/SolidBody.h"
#include "ECS/Components/Velocity.h"
#include "ECS/Entities/Player.h"
#include "ECS/Lib/ECS.h"
#include "ECS/Systems/Animation.h"
#include "ECS/Systems/CollisionManager.h"
#include "ECS/Systems/Controller.h"
#include "ECS/Systems/DrawManager.h"
#include "ECS/Systems/Gfx.h"
#include "ECS/Systems/PathController.h"
#include "ECS/Systems/Physics.h"

#include "Gfx/Color.h"
#include "Gfx/GLResourceManager.h"
#include "Gfx/GfxUtil.h"

#include "Physics/CollisionManager.h"
#include "Systems/Deltatime.h"
#include "Systems/Frametracker.h"
#include "Systems/InputHandler.h"

#include "Util/Print.h"
#include "Util/Vector.h"

using namespace whal;

void MainLoop(GLFWwindow* window) {
    auto& ecs = ecs::ECS::getInstance();
    auto controlSystemRB = ecs.registerSystem<ControllerSystemRB>();
    auto controlSystemFree = ecs.registerSystem<ControllerSystemFree>();
    auto pathSystem = ecs.registerSystem<PathControllerSystem>();
    auto physicsSystem = ecs.registerSystem<PhysicsSystem>();
    auto spriteSystem = ecs.registerSystem<SpriteSystem>();
    auto drawSystem = ecs.registerSystem<DrawSystem>();
    auto animationSystem = ecs.registerSystem<AnimationSystem>();

    // single-component systems for running psuedo-destructors:
    auto rigidBodyMgr = ecs.registerSystem<RigidBodyManager>();
    auto solidBodyMgr = ecs.registerSystem<SolidBodyManager>();
    auto spriteMgr = ecs.registerSystem<SpriteManager>();
    auto drawMgr = ecs.registerSystem<DrawManager>();

    auto player = createPlayerPrefab();
    if (!player.isExpected()) {
        print("failed to create player due to ", player.error());
    }
    auto playerCopyExpected = createPlayerPrefab();
    if (!playerCopyExpected.isExpected()) {
        print(playerCopyExpected.error());
    } else {
        auto playerCopy = playerCopyExpected.value();
        playerCopy.set(Velocity(Vector2f(5.0, 0.0)));
    }

    s32 widthTileHL = PIXELS_PER_TEXEL * 8 / 2;
    s32 heightTileHL = PIXELS_PER_TEXEL * 8 / 2;
    whal::ecs::Entity blockPrefab = ecs.entity().value();
    blockPrefab.add<Position>();
    blockPrefab.add<Velocity>();
    blockPrefab.add<Draw>(Draw(Depth::Player, Color::MAGENTA));
    blockPrefab.add<SolidBody>();

    whal::ecs::Entity block;
    for (s32 i = 0; i < 50; i++) {
        block = blockPrefab.copy().value();
        block.set(Position::tiles(i, 1));
        block.set(SolidBody(toFloatVec(block.get<Position>().e), widthTileHL, heightTileHL));
    }

    for (s32 i = 0; i < 50; i++) {
        if (i % 7 < 4) {
            continue;
        }
        block = blockPrefab.copy().value();
        block.set(Position::tiles(i, 4));
        block.set(SolidBody(toFloatVec(block.get<Position>().e), widthTileHL, heightTileHL));
    }

    auto entity2 = ecs.entity().value();
    entity2.add<Position>(Position::texels(0, 16));
    entity2.add<Velocity>();
    entity2.add<Draw>();
    // entity2.add<PlayerControlFree>();
    entity2.add<SolidBody>(SolidBody(toFloatVec(entity2.get<Position>().e), widthTileHL, heightTileHL));

    // auto pathControl = PathControl(10, {}, 2);
    // pathControl.checkpoints.push_back(Position::texels(0, 90));
    // pathControl.checkpoints.push_back(Position::texels(0, 5));
    // entity2.add<PathControl>(pathControl);

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
        rigidBodyMgr->update();  // this doesn't need to run every frame

        // Render
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        animationSystem->update();
        spriteSystem->drawEntities();
        drawSystem->drawEntities();
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
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    MainLoop(window);

    // clear resources
    glfwTerminate();
    return 0;
}
