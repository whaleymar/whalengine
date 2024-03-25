// clang-format off
#include <glad/gl.h>
#include <GLFW/glfw3.h>  // always include after glad
// clang-format on

#include <iostream>
#include "ECS/Components/Draw.h"
#include "ECS/Systems/Gfx.h"
#include "Gfx/Shader.h"
// #include "Gfx/Texture.h"

// #include "ECS/Components/Velocity.h"
#include "ECS/Lib/ECS.h"
#include "ECS/Systems/Physics.h"
#include "Util/Vector.h"

using namespace whal;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MainLoop(GLFWwindow* window, ShaderProgram program);

// Window dimensions
const GLuint WIDTH = 1280, HEIGHT = 720;

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", NULL, NULL);
    glfwMakeContextCurrent(window);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Set the required callback functions
    glfwSetKeyCallback(window, key_callback);

    // Load OpenGL functions, gladLoadGL returns the loaded version, 0 on error.
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
    }

    glViewport(0, 0, WIDTH, HEIGHT);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    MainLoop(window, *program);

    // clear resources
    glfwTerminate();
    return 0;
}

void MainLoop(GLFWwindow* window, ShaderProgram program) {
    // Texture mainTexture;
    // mainTexture.loadAtlas("data/sprites.png");

    auto& ecs = ecs::ECS::getInstance();
    auto physicsSystem = ecs.registerSystem<PhysicsSystem>();
    auto graphicsSystem = ecs.registerSystem<GraphicsSystem>();

    auto entity = ecs.entity().value();
    entity.add<Position>(Position({0, 0}));
    entity.add<Draw>();

    // auto entity2 = ecs.entity().value();
    // entity2.add<Position>(pos);

    while (!glfwWindowShouldClose(window)) {
        // check inputs
        glfwPollEvents();

        // update systems
        // physicsSystem->update();

        // auto e1Position = ecs.tryGetComponent<Position>(entity);
        // if (e1Position) {
        //     std::cout << "position vec len: " << e1Position.value()->e.y() << std::endl;
        // } else {
        //     std::cout << "null position for entity" << std::endl;
        // }

        // Render
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // mainTexture.bind();

        graphicsSystem->drawEntities(program);

        // Swap the screen buffers
        glfwSwapBuffers(window);
    }
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}
