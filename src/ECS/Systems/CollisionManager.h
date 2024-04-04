#pragma once

#include "ECS/Lib/ECS.h"

namespace whal {

struct RigidBody;
struct SolidBody;

class RigidBodyManager : public ecs::ISystem<RigidBody> {
public:
    void update() override;
    void onAdd(ecs::Entity entity) override;
    void onRemove(ecs::Entity entity) override;
};

class SolidBodyManager : public ecs::ISystem<SolidBody> {
public:
    void onAdd(ecs::Entity entity) override;
    void onRemove(ecs::Entity entity) override;
};

}  // namespace whal
