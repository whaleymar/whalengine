#include "DrawManager.h"

#include "ECS/Components/Draw.h"

namespace whal {

void SpriteManager::onRemove(ecs::Entity entity) {
    entity.get<Sprite>().vao.free();
    entity.get<Sprite>().vbo.free();
}

void DrawManager::onRemove(ecs::Entity entity) {
    entity.get<Draw>().vao.free();
    entity.get<Draw>().vbo.free();
}

}  // namespace whal
