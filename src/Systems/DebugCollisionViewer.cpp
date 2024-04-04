#include "DebugCollisionViewer.h"
// #include "Physics/CollisionManager.h"

#include "ECS/Components/RigidBody.h"
#include "Gfx/Shader.h"
#include "Gfx/VertexObject.h"
#include "glad/gl.h"

namespace whal {

void DebugCollisionViewer::draw(ShaderProgram program, ecs::Entity entity) const {
    static const f32 zeroOffset[2] = {0.0, 0.0};
    glUniform2fv(program.drawOffsetUniform, 1, zeroOffset);

    // auto& collisionMgr = CollisionManager::getInstance();
    // TEMP using player entity to make sure this is working
    RigidBody& rb = entity.get<RigidBody>();
    f32 left = rb.collider.getCollider().left() - 5.0;
    // f32 right= rb.collider.getCollider().right();
    f32 bottom = rb.collider.getCollider().bottom();
    f32 top = rb.collider.getCollider().top();

    Vector2f floatPos = {left, top};
    glUniform2fv(program.drawOffsetUniform, 1, floatPos.e);

    auto leftVerts = MakeLineVertices({left, bottom}, {left, top}, 2);
    mVao.bind();
    mVbo.buffer(leftVerts.data(), leftVerts.size() * sizeof(float));
    updateShaderVars(program);
    glDrawArrays(GL_TRIANGLES, 0, N_VERTS_RECT);
}

}  // namespace whal
