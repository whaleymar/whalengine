// #include <iostream>
// #include "Util/MathUtil.h"
// #include "Util/Vector.h"
//
// using namespace whal;
//
// int main(int argc, char* argv[]) {
//     std::cout << "running..." << std::endl;
//
//     Vector2f myVec1(5.0, 5.0);
//     Vector2f myVec2(-5.0, -5.0);
//     // f32 dot = vec1.dot(vec2);
//     f32 out = abs(myVec1.dot(myVec2));
//     // f32 out = vec1.dot(vec2);
//     // f32 out = vec1.len();
//     // f32 out = abs(-1 * Vector::zero2f.len());
//     std::cout << out << std::endl;
//
//     return 0;
// }

// clang-format off
#include <glad/gl.h>
#include <GLFW/glfw3.h>  // always include after glad
// clang-format on

#include <iostream>
#include "Gfx/Shader.h"
#include "Gfx/VertexObject.h"

#include "ECS/Lib/ECS.h"
#include "Util/Vector.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MainLoop(GLFWwindow* window, whal::ShaderProgram program);

// Window dimensions
const GLuint WIDTH = 1280, HEIGHT = 720;

// The MAIN function, from here we start the application and run the game loop
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
    auto program = whal::InitShaders();
    if (!program.isExpected()) {
        std::cout << "Failed to initialize shaders. Got error:\n" << program.error() << std::endl;
    }

    glViewport(0, 0, WIDTH, HEIGHT);
    MainLoop(window, *program);

    // clear resources
    glfwTerminate();
    return 0;
}

void MainLoop(GLFWwindow* window, whal::ShaderProgram program) {
    auto verts = whal::MakeRectVertices(100, 100);
    auto vao = whal::Vao();
    auto vbo = whal::Vbo();
    u32 nVertices = 6;

    struct Position {
        whal::Vector2i pos;
    };
    Position pos({5, 5});

    struct Velocity {
        whal::Vector2i v;
    };
    Velocity vel({0, 0});
    // Position pos = Position(whal::Vector2i(5, 5));

    auto& ecs = whal::ecs::ECS::getInstance();
    auto entity = ecs.entity().value();
    ecs.add<Position>(entity, pos);
    ecs.add<Velocity>(entity, vel);

    auto entity2 = ecs.entity().value();
    ecs.add<Position>(entity2, pos);
    // entity2.add<Position>(pos);
    ecs.add<Velocity>(entity2, vel);

    std::optional<Position> _pos = ecs.getComponent<Position>(entity);
    if (_pos) {
        std::cout << _pos.value().pos.len() << std::endl;
    } else {
        std::cout << "null position for entity" << std::endl;
    }

    while (!glfwWindowShouldClose(window)) {
        // check inputs
        glfwPollEvents();
        // std::__shared_ptr<whal::ecs::IComponentArray, (__gnu_cxx::_Lock_policy)2>::get (this=0xfffdaaaaaa0efcf0) at
        // /usr/bin/../lib64/gcc/x86_64-pc-linux-gnu/13.2.1/../../../../include/c++/13.2.1/bits/shared_ptr_base.h:1666 1666	      { return _M_ptr; }

        // Render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // gfx.UpdateShaderVars(program)

        vao.bind();
        // vbo.bind();  // dont think i need to bind. binding vao implicitly does it for me
        vbo.buffer(verts.data(), verts.size() * 4);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        program.useProgram();
        glDrawArrays(GL_TRIANGLES, 0, nVertices);

        // Swap the screen buffers
        glfwSwapBuffers(window);
    }
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}
