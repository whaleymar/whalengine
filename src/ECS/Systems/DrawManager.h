#pragma once

#include "ECS/Lib/ECS.h"

namespace whal {

struct Sprite;
struct Draw;

class SpriteManager : public ecs::ISystem<Sprite> {
public:
    void onAdd(ecs::Entity entity) override;
    void onRemove(ecs::Entity entity) override;
};

class DrawManager : public ecs::ISystem<Draw> {
public:
    void onAdd(ecs::Entity entity) override;
    void onRemove(ecs::Entity entity) override;
};

}  // namespace whal
