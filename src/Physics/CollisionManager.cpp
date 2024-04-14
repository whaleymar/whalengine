#include "CollisionManager.h"

#include <algorithm>
#include <glad/gl.h>

#include "Gfx/GLResourceManager.h"
#include "Gfx/GfxUtil.h"
#include "Gfx/Shader.h"
#include "Physics/Actor.h"
#include "Physics/IUseCollision.h"
#include "Physics/Solid.h"

namespace whal {

void CollisionManager::add(ActorCollider* actor) {
    mActors.push_back(actor);
}

void CollisionManager::add(SolidCollider* solid) {
    mSolids.push_back(solid);
}

void CollisionManager::remove(ActorCollider* actor) {
    auto ix = std::find(mActors.begin(), mActors.end(), actor);
    if (ix != mActors.end()) {
        mActors.erase(ix);
    }
}

void CollisionManager::remove(SolidCollider* solid) {
    auto ix = std::find(mSolids.begin(), mSolids.end(), solid);
    if (ix != mSolids.end()) {
        mSolids.erase(ix);
    }
}

void CollisionManager::clearActors() {
    mActors.clear();
}

void CollisionManager::clearSolids() {
    mSolids.clear();
}

#ifndef NDEBUG
void drawCollider(ShaderProgram program, const IUseCollision* collider) {
    const AABB& aabb = collider->getCollider();

    Vector2f floatPos(aabb.left(), aabb.top());
    glUniform2fv(program.drawOffsetUniform, 1, floatPos.e);

    aabb.vao.bind();
    auto vertices = MakeRectVerticesRGBUV(aabb.half.x() * 2, aabb.half.y() * 2, Depth::Debug, Color::RED);
    aabb.vbo.buffer(vertices.data(), vertices.size() * sizeof(float));

    updateShaderVars(program);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, N_VERTS_RECT);
}

void drawColliders() {
    auto program = GLResourceManager::getInstance().getProgram(SHNAME_DEBUG);
    program.useProgram();

    for (const IUseCollision* collider : CollisionManager::getInstance().getAllActors()) {
        drawCollider(program, collider);
    }
    for (const IUseCollision* collider : CollisionManager::getInstance().getAllSolids()) {
        drawCollider(program, collider);
    }
}
#endif

}  // namespace whal
