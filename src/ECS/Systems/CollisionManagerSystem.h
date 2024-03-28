#pragma once

#include "ECS/Components/RigidBody.h"
#include "ECS/Components/SolidBody.h"
#include "ECS/Lib/ECS.h"

namespace whal {

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
