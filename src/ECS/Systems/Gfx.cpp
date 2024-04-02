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
    static const Vector2i OFFSET = Vector2i(WINDOW_WIDTH_TEXELS / 2, WINDOW_HEIGHT_TEXELS / 2);  // puts (0,0) at bottom left of screen
    static const Vector2f HALF_TILE_OFFSET = Vector2f(TILE_LEN_PIXELS / 2, -TILE_LEN_PIXELS / 2);
    program.useProgram();
    // TODO sort by depth
    for (auto const& [entityid, entity] : getEntities()) {
        Position& pos = entity.get<Position>();
        Draw& draw = entity.get<Draw>();

        Vector2f floatPos = toFloatVec(pos.e - OFFSET) * PIXELS_PER_TEXEL - HALF_TILE_OFFSET;
        glUniform2fv(program.drawOffsetUniform, 1, floatPos.e);

        draw.vao.bind();
        auto vertices = draw.getVertices();
        draw.vbo.buffer(vertices.data(), vertices.size() * sizeof(float));

        updateShaderVars(program);
        glDrawArrays(GL_TRIANGLES, 0, draw.nVertices);
    }
}

}  // namespace whal
