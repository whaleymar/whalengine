#pragma once

#include <unordered_map>

#include "json_fwd.hpp"

#include "Map/Level.h"
#include "Util/Factory.h"
#include "Util/Vector.h"

#include "ECS/Components/RailsControl.h"

namespace whal {

namespace ecs {
class Entity;
}

// there is no base component class, so I'll pass the entity to the creation function instead of returning a component
using ComponentAdder = void (*)(nlohmann::json&, nlohmann::json&, std::unordered_map<s32, s32>&, s32, ActiveLevel&, ecs::Entity);
class ComponentFactory : public Factory<ComponentAdder> {
public:
    ComponentFactory();
};

void addComponentRailsControl(nlohmann::json& values, nlohmann::json& allObjects, std::unordered_map<s32, s32>& idToIndex, s32 thisId,
                              ActiveLevel& level, ecs::Entity entity);
// void addComponentAnimator(nlohmann::json& data, ecs::Entity entity);
// void addComponentActorCollider(nlohmann::json& data, ecs::Entity entity);
void addComponentSolidCollider(nlohmann::json& values, nlohmann::json& allObjects, std::unordered_map<s32, s32>& idToIndex, s32 thisId,
                               ActiveLevel& level, ecs::Entity entity);
void addComponentDraw(nlohmann::json& values, nlohmann::json& allObjects, std::unordered_map<s32, s32>& idToIndex, s32 thisId, ActiveLevel& level,
                      ecs::Entity entity);
// void addComponentSprite(nlohmann::json& data, ecs::Entity entity);
// void addComponentLifetime(nlohmann::json& data, ecs::Entity entity);
// void addComponentPlayerControlRB(nlohmann::json& data, ecs::Entity entity);
// void addComponentPlayerControlFree(nlohmann::json& data, ecs::Entity entity);
// void addComponentChildren(nlohmann::json& data, ecs::Entity entity);
// void addComponentFollow(nlohmann::json& data, ecs::Entity entity);
// void addComponentRigidBody(nlohmann::json& data, ecs::Entity entity);
// void addComponentTags(nlohmann::json& data, ecs::Entity entity);
// void addComponentTransform(nlohmann::json& data, ecs::Entity entity);
// void addComponentVelocity(nlohmann::json& data, ecs::Entity entity);

void loadCheckpoints(nlohmann::json& checkpointData, std::vector<RailsControl::CheckPoint>& dstCheckpoints, ActiveLevel& level);
void loadCheckpointN2(nlohmann::json& data, std::vector<RailsControl::CheckPoint>& dstCheckpoints, ActiveLevel& level);

}  // namespace whal
