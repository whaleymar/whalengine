#include "ECS/Systems/Gfx.h"

#include "Gfx/GLResourceManager.h"
#include "Gfx/GfxUtil.h"
#include "Gfx/Shader.h"
#include "Gfx/VertexObject.h"
#include "Util/Vector.h"

#include "ECS/Components/Draw.h"
#include "ECS/Components/Position.h"

// clang-format off
#include <glad/gl.h>
#include <GLFW/glfw3.h>  // always include after glad
// clang-format on

namespace whal {

void SpriteSystem::onAdd(const ecs::Entity entity) {
    f32 fDepth = depthToFloat(entity.get<Sprite>().depth);
    auto prev = mSorted.before_begin();
    for (auto it = mSorted.begin(); it != mSorted.end(); ++it) {
        f32 fDepthNew = depthToFloat(it->get<Sprite>().depth);
        if (fDepthNew >= fDepth) {
            mSorted.insert_after(prev, entity);
            return;
        }
        prev = it;
    }
    mSorted.insert_after(prev, entity);
}

void SpriteSystem::onRemove(const ecs::Entity entity) {
    mSorted.remove_if([entity](const ecs::Entity& e) { return e.id() == entity.id(); });
}

void SpriteSystem::drawEntities() {
    auto program = GLResourceManager::getInstance().getProgram(SHNAME_SPRITE_RGB);
    GLResourceManager::getInstance().getTexture(TEXNAME_SPRITE).bind();
    program.useProgram();
    for (auto const& entity : mSorted) {
        Position& pos = entity.get<Position>();
        Sprite& sprite = entity.get<Sprite>();
        if (sprite.isVertsUpdateNeeded) {
            sprite.updateVertices(pos.facing != Facing::Left);
        }

        // position is the bottom-middle of the vao, but openGL expects position of the top left corner
        // TODO clamp to texel grid
        // f32 height = static_cast<f32>(sprite.frameSizeTexels.x() * PIXELS_PER_TEXEL) * sprite.scale.y();
        Vector2f drawOffset = toFloatVec(sprite.frameSizeTexels * PIXELS_PER_TEXEL) * sprite.scale * Vector2f(-0.5, 1.0);
        Vector2f floatPos = (toFloatVec(pos.e) + drawOffset);
        glUniform2fv(program.drawOffsetUniform, 1, floatPos.e);

        sprite.vao.bind();
        auto vertices = sprite.getVertices();
        sprite.vbo.buffer(vertices.data(), vertices.size() * sizeof(float));

        updateShaderVars(program);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, sprite.nVertices);
    }
}

void DrawSystem::drawEntities() {
    auto program = GLResourceManager::getInstance().getProgram(SHNAME_COLOR);
    program.useProgram();
    // sorting not required since Draw components don't have transparency
    for (auto const& [entityid, entity] : getEntities()) {
        // TODO abstract this loop
        Position& pos = entity.get<Position>();
        Draw& draw = entity.get<Draw>();

        // position is the bottom-middle of the vao, but openGL expects position of the top left corner
        // TODO clamp to texel grid
        Vector2f drawOffset = toFloatVec(draw.frameSizeTexels * PIXELS_PER_TEXEL) * Vector2f(-0.5, 1.0);
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
