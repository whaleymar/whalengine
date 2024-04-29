#pragma once

#include "ECS/Components/Tags.h"
#include "ECS/Lib/ECS.h"
namespace whal {

class PlayerSystem : public ecs::ISystem<Player> {
public:
    static std::shared_ptr<PlayerSystem> instance() {
        static std::shared_ptr<PlayerSystem> instance_ = ecs::ECS::getInstance().registerSystem<PlayerSystem>();
        return instance_;
    }
};

class CameraSystem : public ecs::ISystem<Camera> {
public:
    static std::shared_ptr<CameraSystem> instance() {
        static std::shared_ptr<CameraSystem> instance_ = ecs::ECS::getInstance().registerSystem<CameraSystem>();
        return instance_;
    }
};

std::optional<ecs::Entity> getCamera();

}  // namespace whal
