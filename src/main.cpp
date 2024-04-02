// clang-format off
#include <glad/gl.h>
#include <GLFW/glfw3.h>  // always include after glad
// clang-format on

#include <iostream>

#include "ECS/Components/Draw.h"
#include "ECS/Components/PathControl.h"
#include "ECS/Components/PlayerControl.h"
#include "ECS/Components/Position.h"
#include "ECS/Components/RigidBody.h"
#include "ECS/Components/SolidBody.h"
#include "ECS/Components/Velocity.h"
#include "ECS/Lib/ECS.h"
#include "ECS/Systems/CollisionManagerSystem.h"
#include "ECS/Systems/Controller.h"
#include "ECS/Systems/Gfx.h"
#include "ECS/Systems/PathController.h"
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
    auto controlSystemRB = ecs.registerSystem<ControllerSystemRB>();
    auto controlSystemFree = ecs.registerSystem<ControllerSystemFree>();
    auto pathSystem = ecs.registerSystem<PathControllerSystem>();
    auto physicsSystem = ecs.registerSystem<PhysicsSystem>();
    auto graphicsSystem = ecs.registerSystem<GraphicsSystem>();

    // single-component systems for syncing the global collision manager:
    auto rigidBodyMgr = ecs.registerSystem<RigidBodyManager>();
    auto solidBodyMgr = ecs.registerSystem<SolidBodyManager>();

    auto s_TPT = static_cast<s32>(TEXELS_PER_TILE);
    auto entity = ecs.entity().value();
    entity.add<Position>(Position({16, 5 * s_TPT}));
    entity.add<Velocity>();
    entity.add<Draw>();
    entity.add<PlayerControlRB>();
    // entity.add<PlayerControlFree>(PlayerControlFree(1));

    s32 halflen = Draw().frameSizeTexels.x() / 2;
    entity.add<RigidBody>(RigidBody(toFloatVec(entity.get<Position>().e), halflen, halflen));

    whal::ecs::Entity block;
    for (s32 i = 0; i < 15; i++) {
        block = ecs.entity().value();
        s32 y = i == 0 ? 1 : 0;
        block.add<Position>(Position({s_TPT * i, s_TPT * (1 + y)}));
        block.add<Velocity>();
        block.add<Draw>();
        block.add<SolidBody>(SolidBody(toFloatVec(block.get<Position>().e), halflen, halflen));
    }

    // copy example:
    // auto dupeEntity = block.copy();
    // if (dupeEntity.isExpected()) {
    //     whal::ecs::Entity eVal = dupeEntity.value();
    //     eVal.add<PlayerControlFree>();
    // } else {
    //     print(dupeEntity.error());
    // }

    // auto entity2 = ecs.entity().value();
    // entity2.add<Position>(Position({0, 0}));
    // entity2.add<Velocity>();
    // entity2.add<Draw>();
    // entity2.add<PlayerControlFree>();
    // entity2.add<SolidBody>(SolidBody(toFloatVec(entity2.get<Position>().e), halflen, halflen));

    // auto pathControl = PathControl(1);
    // pathControl.checkpoints.push_back(Position({320, 0}));
    // pathControl.checkpoints.push_back(Position({320, 180}));
    // pathControl.checkpoints.push_back(Position({0, 180}));
    // pathControl.checkpoints.push_back(Position({0, 0}));
    // entity2.add<PathControl>(pathControl);

    auto& input = Input::getInstance();
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
        rigidBodyMgr->update();

        // std::cout << Deltatime::getInstance().get() << std::endl;
        // if (Frametracker::getInstance().getFrame() == 0) {
        // };  //     std::cout << Frametracker::getInstance().getFPS() << std::endl;

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
