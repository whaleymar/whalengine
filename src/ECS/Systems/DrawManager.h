#pragma once

#include "ECS/Lib/ECS.h"

namespace whal {

struct Sprite;
struct Draw;

class SpriteManager : public ecs::ISystem<Sprite> {
public:
    void onRemove(ecs::Entity entity) override;
};

class DrawManager : public ecs::ISystem<Draw> {
public:
    void onRemove(ecs::Entity entity) override;
};

}  // namespace whal
