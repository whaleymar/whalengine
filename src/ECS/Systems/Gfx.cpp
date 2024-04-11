#include "ECS/Systems/Gfx.h"

#include "Gfx/GLResourceManager.h"
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

void SpriteSystem::update(){};

void SpriteSystem::drawEntities() {
    auto program = GLResourceManager::getInstance().getProgram(SHNAME_SPRITE_RGB);
    GLResourceManager::getInstance().getTexture(TEXNAME_SPRITE).bind();
    program.useProgram();
    // TODO sort by depth
    for (auto const& [entityid, entity] : getEntities()) {
        Position& pos = entity.get<Position>();
        Sprite& sprite = entity.get<Sprite>();
        if (sprite.isVertsUpdateNeeded) {
            sprite.updateVertices(pos.facing != Facing::Left);
        }

        // position is the center, but openGL expects position of the top left corner
        // TODO clamp to texel grid
        Vector2f drawOffset = toFloatVec(sprite.frameSizeTexels * PIXELS_PER_TEXEL) * Vector2f(-0.5, 0.5);
        Vector2f floatPos = (toFloatVec(pos.e) + drawOffset);
        glUniform2fv(program.drawOffsetUniform, 1, floatPos.e);

        sprite.vao.bind();
        auto vertices = sprite.getVertices();
        sprite.vbo.buffer(vertices.data(), vertices.size() * sizeof(float));

        updateShaderVars(program);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, sprite.nVertices);
    }
}

void DrawSystem::update(){};

void DrawSystem::drawEntities() {
    auto program = GLResourceManager::getInstance().getProgram(SHNAME_COLOR);
    program.useProgram();
    // TODO sort by depth
    for (auto const& [entityid, entity] : getEntities()) {
        // TODO abstract this loop
        Position& pos = entity.get<Position>();
        Draw& draw = entity.get<Draw>();

        // position is the center, but openGL expects position of the top left corner
        // TODO clamp to texel grid
        Vector2f drawOffset = toFloatVec(draw.frameSizeTexels * PIXELS_PER_TEXEL) * Vector2f(-0.5, 0.5);
        Vector2f floatPos = (toFloatVec(pos.e) + drawOffset);
        glUniform2fv(program.drawOffsetUniform, 1, floatPos.e);

        draw.vao.bind();
        auto vertices = draw.getVertices();
        draw.vbo.buffer(vertices.data(), vertices.size() * sizeof(float));

        updateShaderVars(program);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, draw.nVertices);
    }
}

}  // namespace whal
