#pragma once

#include <forward_list>

#include "ECS/Lib/ECS.h"

namespace whal {

struct Position;
struct Sprite;
struct Draw;

class SpriteSystem : public ecs::ISystem<Position, Sprite> {
public:
    void onAdd(const ecs::Entity) override;
    void onRemove(const ecs::Entity) override;
    void drawEntities();

private:
    std::forward_list<ecs::Entity> mSorted;
};

class DrawSystem : public ecs::ISystem<Position, Draw> {
public:
    void drawEntities();
};

}  // namespace whal
