#include "CollisionManager.h"

#include "ECS/Components/Collision.h"
#include "ECS/Systems/TagTrackers.h"
#include "Game/Events.h"

#include <glad/gl.h>

#include "Gfx/GLResourceManager.h"

namespace whal {

void ActorsManager::update() {
    if (!mIsUpdateNeeded) {
        return;
    }
    std::vector<std::pair<ecs::Entity, ActorCollider*>> newActorList;

    for (auto& [entityid, entity] : getEntitiesRef()) {
        auto pCollider = &entity.get<ActorCollider>();
        newActorList.push_back({entity, pCollider});
    }

    mActors = std::move(newActorList);
    mIsUpdateNeeded = false;
}

void ActorsManager::onAdd(ecs::Entity entity) {
    mActors.push_back({entity, &entity.get<ActorCollider>()});
    entity.get<ActorCollider>().setEntity(entity);
#ifndef NDEBUG
    entity.get<ActorCollider>().getCollider().vao.initArray();
    entity.get<ActorCollider>().getCollider().vbo.initBuffer();
#endif
}

void ActorsManager::onRemove(ecs::Entity entity) {
    mIsUpdateNeeded = true;
#ifndef NDEBUG
    entity.get<ActorCollider>().getCollider().vao.freeArray();
    entity.get<ActorCollider>().getCollider().vbo.freeBuffer();
#endif
}

void SolidsManager::update() {
    if (!mIsUpdateNeeded) {
        return;
    }
    // put colliders with callbacks first so they get priority in collision checks
    std::vector<std::pair<ecs::Entity, SolidCollider*>> newSolids;
    std::vector<std::pair<ecs::Entity, SolidCollider*>> newSolidsWithCallbacks;
    for (auto& [entityid, entity] : getEntitiesRef()) {
        auto pCollider = &entity.get<SolidCollider>();
        if (pCollider->getOnCollisionEnter() == nullptr) {
            newSolids.push_back({entity, pCollider});
        } else {
            newSolidsWithCallbacks.push_back({entity, pCollider});
        }
    }
    // mSolids = std::move(newSolids);
    mSolids.clear();
    mSolids.reserve(newSolidsWithCallbacks.size() + newSolids.size());
    mSolids.insert(mSolids.end(), newSolidsWithCallbacks.begin(), newSolidsWithCallbacks.end());
    mSolids.insert(mSolids.end(), newSolids.begin(), newSolids.end());

    mIsUpdateNeeded = false;
}

void SolidsManager::onAdd(ecs::Entity entity) {
    SolidCollider* pCollider = &entity.get<SolidCollider>();
    mSolids.push_back({entity, pCollider});
    pCollider->setEntity(entity);
    if (pCollider->getOnCollisionEnter() != nullptr) {
        mIsUpdateNeeded = true;
    }
#ifndef NDEBUG
    entity.get<SolidCollider>().getCollider().vao.initArray();
    entity.get<SolidCollider>().getCollider().vbo.initBuffer();
#endif
}

void SolidsManager::onRemove(ecs::Entity entity) {
    mIsUpdateNeeded = true;
#ifndef NDEBUG
    entity.get<SolidCollider>().getCollider().vao.freeArray();
    entity.get<SolidCollider>().getCollider().vbo.freeBuffer();
#endif
}

#ifndef NDEBUG

void drawCollider(ShaderProgram program, const IUseCollision* collider, const RGB color) {
    const AABB& aabb = collider->getCollider();

    Vector2f floatPos(aabb.left(), aabb.top());
    glUniform2fv(program.drawOffsetUniform, 1, floatPos.e);
    Vector2f size = Vector2f(aabb.half.x(), aabb.half.y()) * 2;
    glUniform2fv(program.sizeUniform, 1, size.e);

    aabb.vao.bind();
    auto vertices = MakeRectVerticesRGBUV(aabb.half.x() * 2, aabb.half.y() * 2, Depth::Debug, color);
    aabb.vbo.buffer(vertices.data(), vertices.size() * sizeof(float));

    updateShaderVars(program);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, N_VERTS_RECT);
}

void drawColliders() {
    auto program = GLResourceManager::getInstance().getProgram(ShaderType::Debug);
    program.useProgram();

    auto cameraPosF = toFloatVec(getCameraPosition());
    glUniform2fv(program.cameraPositionUniform, 1, cameraPosF.e);

    for (const auto& [entity, collider] : ActorsManager::getInstance()->getAllActors()) {
        drawCollider(program, collider, Color::MAGENTA);
    }
    for (const auto& [entity, collider] : SolidsManager::getInstance()->getAllSolids()) {
        drawCollider(program, collider, Color::RED);
    }
}
#endif

}  // namespace whal
