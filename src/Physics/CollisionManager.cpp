#include "CollisionManager.h"

#include <glad/gl.h>

#include "ECS/Components/Collision.h"
#include "Gfx/GLResourceManager.h"
#include "Gfx/GfxUtil.h"
#include "Gfx/Shader.h"
#include "Physics/IUseCollision.h"

namespace whal {

void CollisionManager::init(std::shared_ptr<RigidBodyManager> actorMgr, std::shared_ptr<SolidBodyManager> solidMgr) {
    mActorMgr = actorMgr;
    mSolidMgr = solidMgr;
#ifndef NDEBUG
    if (mActorMgr && mSolidMgr) {
        mIsValid = true;
    }
#endif  // !NDEBUG
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
    auto program = GLResourceManager::getInstance().getProgram(ShaderType::Debug);
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
