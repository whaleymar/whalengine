#include "ECS/Systems/Gfx.h"

#include "Gfx/Shader.h"
#include "Util/Vector.h"

// clang-format off
#include <glad/gl.h>
#include <GLFW/glfw3.h>  // always include after glad
// clang-format on

// #include <iostream>

namespace whal {

void GraphicsSystem::update(){};

void GraphicsSystem::drawEntities(ShaderProgram program) {
    program.useProgram();
    // TODO sort by depth
    for (auto const& [entityid, entity] : getEntities()) {
        // std::cout << "drawing entity " << entityid << std::endl;
        Position& pos = entity.get<Position>();
        Draw& draw = entity.get<Draw>();

        // TODO convert to pixel location instead of cast
        Vector2f floatPos = toFloatVec(pos.e);
        glUniform2fv(program.drawOffsetUniform, 1, floatPos.e);

        draw.vao.bind();
        // draw.vbo.bind();
        draw.vbo.buffer(draw.vertices.data(), draw.vertices.size() * sizeof(float));

        updateShaderVars(program);
        glDrawArrays(GL_TRIANGLES, 0, draw.nVertices);
    }
}

}  // namespace whal
