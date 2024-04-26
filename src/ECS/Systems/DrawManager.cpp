#include "DrawManager.h"

#include "ECS/Components/Draw.h"

namespace whal {

void SpriteManager::onAdd(ecs::Entity entity) {
    entity.get<Sprite>().vao.initArray();
    entity.get<Sprite>().vbo.initBuffer();
}

void SpriteManager::onRemove(ecs::Entity entity) {
    entity.get<Sprite>().vao.freeArray();
    entity.get<Sprite>().vbo.freeBuffer();
}

void DrawManager::onAdd(ecs::Entity entity) {
    entity.get<Draw>().vao.initArray();
    entity.get<Draw>().vbo.initBuffer();
}

void DrawManager::onRemove(ecs::Entity entity) {
    entity.get<Draw>().vao.freeArray();
    entity.get<Draw>().vbo.freeBuffer();
}

}  // namespace whal
