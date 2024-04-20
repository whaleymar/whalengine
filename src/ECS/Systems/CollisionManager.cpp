#include "CollisionManager.h"

#include "ECS/Components/Collision.h"
#include "Util/Print.h"

#include <glad/gl.h>

#include "Gfx/GLResourceManager.h"

namespace whal {

void ActorsManager::update() {
    if (mIsUpdateNeeded == true) {
        print("Updating actors list");
    }
    std::vector<ActorCollider*> newActorList;

    bool isUpdateNeeded = mIsUpdateNeeded;
    mIsUpdateNeeded = false;  // might change if we kill entities

    for (auto& [entityid, entity] : getEntities()) {
        auto pCollider = &entity.get<ActorCollider>();
        if (!pCollider->isAlive()) {
            entity.kill();
        } else if (isUpdateNeeded) {
            newActorList.push_back(pCollider);
        }
    }

    if (isUpdateNeeded) {
        mActors = newActorList;
    }
}

void ActorsManager::onAdd(ecs::Entity entity) {
    mIsUpdateNeeded = true;
}

void ActorsManager::onRemove(ecs::Entity entity) {
    mIsUpdateNeeded = true;
}

const std::vector<ActorCollider*>& ActorsManager::getAllActors() const {
    return mActors;
}

void SolidsManager::update() {
    if (!mIsUpdateNeeded) {
        return;
    }
    print("updating solids list");
    std::vector<SolidCollider*> newSolidList;
    for (auto& [entityid, entity] : getEntities()) {
        auto pCollider = &entity.get<SolidCollider>();
        newSolidList.push_back(pCollider);
    }
    mSolids = newSolidList;
    mIsUpdateNeeded = false;
}

void SolidsManager::onAdd(ecs::Entity entity) {
    mIsUpdateNeeded = true;
}

void SolidsManager::onRemove(ecs::Entity entity) {
    mIsUpdateNeeded = true;
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

    for (const IUseCollision* collider : ActorsManager::getInstance()->getAllActors()) {
        drawCollider(program, collider);
    }
    for (const IUseCollision* collider : SolidsManager::getInstance()->getAllSolids()) {
        drawCollider(program, collider);
    }
}
#endif

}  // namespace whal
