#include "ECS/Systems/Gfx.h"

#include "Gfx/Shader.h"
#include "Util/Vector.h"

// clang-format off
#include <glad/gl.h>
#include <GLFW/glfw3.h>  // always include after glad
// clang-format on

namespace whal {

void GraphicsSystem::update(){};

void GraphicsSystem::drawEntities(ShaderProgram program) {
    program.useProgram();
    // TODO sort by depth
    for (auto const& [entityid, entity] : getEntities()) {
        Position& pos = entity.get<Position>();
        Draw& draw = entity.get<Draw>();

        Vector2f floatPos = toFloatVec(pos.e);
        glUniform2fv(program.drawOffsetUniform, 1, floatPos.e);

        draw.vao.bind();
        draw.vbo.buffer(draw.vertices.data(), draw.vertices.size() * sizeof(float));

        updateShaderVars(program);
        glDrawArrays(GL_TRIANGLES, 0, draw.nVertices);
    }
}

// Vector2f getPixelPosition(const Vector2i position) {}

}  // namespace whal
