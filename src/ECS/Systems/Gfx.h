#pragma once

#include "ECS/Lib/ECS.h"

namespace whal {

struct Position;
struct Sprite;
struct Draw;

class SpriteSystem : public ecs::ISystem<Position, Sprite> {
public:
    void update() override;
    void drawEntities();
};

class DrawSystem : public ecs::ISystem<Position, Draw> {
public:
    void update() override;
    void drawEntities();
};

}  // namespace whal
