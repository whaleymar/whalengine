#include "ECS/Systems/Gfx.h"

#include "Gfx/GfxUtil.h"
#include "Gfx/Shader.h"
#include "Util/Vector.h"

#include "ECS/Components/Draw.h"
#include "ECS/Components/Position.h"

// clang-format off
#include <glad/gl.h>
#include <GLFW/glfw3.h>  // always include after glad
// clang-format on

namespace whal {

void GraphicsSystem::update(){};

void GraphicsSystem::drawEntities(ShaderProgram program) {
    // this puts (0,0) at the bottom left of the screen
    static const Vector2i OFFSET = Vector2i(WINDOW_WIDTH_PIXELS / 2, WINDOW_HEIGHT_PIXELS / 2);

    program.useProgram();
    // TODO sort by depth
    for (auto const& [entityid, entity] : getEntities()) {
        Position& pos = entity.get<Position>();
        Draw& draw = entity.get<Draw>();

        // position is the center, but openGL expects position of the top left corner
        Vector2f drawOffset = toFloatVec(draw.frameSizeTexels) * Vector2f(-0.5, 0.5);
        Vector2f floatPos = (toFloatVec(pos.e - OFFSET) + drawOffset);
        glUniform2fv(program.drawOffsetUniform, 1, floatPos.e);

        draw.vao.bind();
        auto vertices = draw.getVertices();
        draw.vbo.buffer(vertices.data(), vertices.size() * sizeof(float));

        updateShaderVars(program);
        glDrawArrays(GL_TRIANGLES, 0, draw.nVertices);
    }
}

}  // namespace whal
