// clang-format off
#include <glad/gl.h>
#include <GLFW/glfw3.h>  // always include after glad
// clang-format on

#include <iostream>

#include "ECS/Components/SolidBody.h"
#include "ECS/Components/Velocity.h"
#include "ECS/Lib/ECS.h"
#include "ECS/Systems/CollisionManagerSystem.h"
#include "ECS/Systems/Controller.h"
#include "ECS/Systems/Gfx.h"
#include "ECS/Systems/Physics.h"

#include "Gfx/Shader.h"
// #include "Gfx/Texture.h"
#include "Gfx/GfxUtil.h"

#include "Systems/Deltatime.h"
#include "Systems/Frametracker.h"
#include "Systems/InputHandler.h"

#include "Util/Vector.h"

using namespace whal;

void MainLoop(GLFWwindow* window, ShaderProgram program) {
    // Texture mainTexture;
    // mainTexture.loadAtlas("data/sprites.png");

    auto& ecs = ecs::ECS::getInstance();
    auto controlSystem = ecs.registerSystem<ControllerSystem>();
    auto physicsSystem = ecs.registerSystem<PhysicsSystem>();
    auto graphicsSystem = ecs.registerSystem<GraphicsSystem>();

    // single-component systems for syncing the global collision manager:
    auto rigidBodyMgr = ecs.registerSystem<RigidBodyManager>();
    auto solidBodyMgr = ecs.registerSystem<SolidBodyManager>();

    auto entity = ecs.entity().value();
    entity.add<Position>(Position({0, 0}));
    entity.add<Velocity>();
    entity.add<Draw>();
    // entity.add<PlayerControl>();

    s32 halflen = entity.get<Draw>().frameSizeTexels.x() * static_cast<s32>(PIXELS_PER_TEXEL) / 2;
    entity.add<RigidBody>(RigidBody(toFloatVec(entity.get<Position>().e), halflen, halflen));

    auto entity2 = ecs.entity().value();
    entity2.add<Position>(Position({150, -15}));
    entity2.add<Velocity>();
    entity2.add<Draw>();
    entity2.add<PlayerControl>();
    entity2.add<SolidBody>(SolidBody(toFloatVec(entity2.get<Position>().e), halflen, halflen));

    while (!glfwWindowShouldClose(window)) {
        // check inputs
        glfwPollEvents();
        if (Input::getInstance().isPause()) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }

        // update systems
        Deltatime::getInstance().update();
        Frametracker::getInstance().update();
        controlSystem->update();
        physicsSystem->update();
        rigidBodyMgr->update();

        // std::cout << Deltatime::getInstance().get() << std::endl;
        // if (Frametracker::getInstance().getFrame() == 0)
        //     std::cout << Frametracker::getInstance().getFPS() << std::endl;

        // Render
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // mainTexture.bind();

        graphicsSystem->drawEntities(program);

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
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwSetKeyCallback(window, key_callback);

    int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0) {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
        return -1;
    }

    std::cout << "Loaded OpenGL " << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version) << std::endl;

    // Load Shaders
    auto program = InitShaders();
    if (!program.isExpected()) {
        std::cout << "Failed to initialize shaders. Got error:\n" << program.error() << std::endl;
        return -1;
    }

    // glViewport(0, 0, WINDOW_WIDTH_PIXELS, WINDOW_HEIGHT_PIXELS); idk what this does
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    MainLoop(window, *program);

    // clear resources
    glfwTerminate();
    return 0;
}
