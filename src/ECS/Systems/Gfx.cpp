#include "ECS/Systems/Gfx.h"

#include "Gfx/GLResourceManager.h"
#include "Gfx/GfxUtil.h"
#include "Gfx/Shader.h"
#include "Gfx/VertexObject.h"
#include "Util/Vector.h"

#include "ECS/Components/Draw.h"
#include "ECS/Components/Transform.h"

#include <glad/gl.h>

namespace whal {

constexpr f32 TEXELS_PER_PIXEL = 1.0f / PIXELS_PER_TEXEL;

Vector2f clampToTexelGrid(Vector2f vec) {
    return Vector2f(std::roundf(vec.x() * TEXELS_PER_PIXEL), std::roundf(vec.y() * TEXELS_PER_PIXEL)) * PIXELS_PER_TEXEL;
}

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

void drawEntity(ShaderProgram& program, Transform& trans, IDraw& draw, f32* pVertices, s32 nVertices) {
    // position is the bottom-middle of the vao, but openGL expects position of the top left corner
    Vector2f drawOffset = toFloatVec(draw.getFrameSizeTexels() * PIXELS_PER_TEXEL) * draw.scale * Vector2f(-0.5, 1.0);
    Vector2f floatPos = (toFloatVec(trans.position) + drawOffset);

    // RESEARCH
    // Vector2f floatPos = clampToTexelGrid(toFloatVec(trans.position) + drawOffset);
    glUniform2fv(program.drawOffsetUniform, 1, floatPos.e);

    draw.vao.bind();
    draw.vbo.buffer(pVertices, nVertices * sizeof(float));

    updateShaderVars(program);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, draw.nVertices);
}

void SpriteSystem::drawEntities() {
    auto program = GLResourceManager::getInstance().getProgram(ShaderType::SpriteRGB);
    GLResourceManager::getInstance().getTexture(TEXNAME_SPRITE).bind();
    program.useProgram();
    for (auto const& entity : mSorted) {
        Transform& trans = entity.get<Transform>();
        Sprite& sprite = entity.get<Sprite>();
        if (sprite.isVertsUpdateNeeded) {
            sprite.updateVertices(trans.facing != Facing::Left);
        }
        drawEntity(program, trans, sprite, sprite.getVertices().data(), sprite.getVertices().size());
    }
}

void DrawSystem::drawEntities() {
    auto program = GLResourceManager::getInstance().getProgram(ShaderType::RGBonly);
    program.useProgram();
    // sorting not required since Draw components don't have transparency
    for (auto const& [entityid, entity] : getEntities()) {
        Transform& trans = entity.get<Transform>();
        Draw& draw = entity.get<Draw>();
        drawEntity(program, trans, draw, draw.getVertices().data(), draw.getVertices().size());
    }
}

}  // namespace whal
