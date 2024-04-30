#include "ComponentFactory.h"

#include <sstream>
#include "Gfx/GfxUtil.h"
#include "Map/Level.h"
#include "json.hpp"

#include "ECS/Components/Collision.h"
#include "ECS/Components/Draw.h"
#include "ECS/Components/RailsControl.h"
#include "ECS/Components/Transform.h"
#include "ECS/Lib/ECS.h"

#include "Util/Print.h"

namespace whal {

static NameToCreator<ComponentAdder> S_COMPONENT_ENTRIES[] = {
    {"Component_RailsControl", addComponentRailsControl},
    // {"Animator", addComponentAnimator},
    // {"ActorCollider", addComponentActorCollider},
    {"Component_SolidCollider", addComponentSolidCollider},
    {"Component_Draw", addComponentDraw},
    // {"Lifetime", addComponentLifetime},
    // {"PlayerControlRB", addComponentPlayerControlRB},
    // {"PlayerControlFree", addComponentPlayerControlFree},
    // {"Children", addComponentChildren},
    // {"Follow", addComponentFollow},
    // {"RigidBody", addComponentRigidBody},
    // {"Tags", addComponentTags},
    // {"Transform", addComponentTransform},
    // {"Velocity", addComponentVelocity},
};

ComponentFactory::ComponentFactory() : Factory<ComponentAdder>("ComponentFactory") {
    initFactory(S_COMPONENT_ENTRIES);
}

void addComponentRailsControl(nlohmann::json& values, nlohmann::json& allObjects, std::unordered_map<s32, s32>& idToIndex, s32 thisId,
                              ActiveLevel& level, ecs::Entity entity) {
    std::vector<RailsControl::CheckPoint> checkpoints;
    if (values.contains("Checkpoints")) {
        s32 id = values["Checkpoints"];
        nlohmann::json checkPointObj = allObjects[idToIndex[id]];
        loadCheckpoints(checkPointObj, checkpoints, level);
    }

    // TODO get default values from project file
    bool isCycle = true;
    f32 speed = 5;
    f32 waitTime = 0;

    if (values.contains("isCycle")) {
        isCycle = values["isCycle"];
    }

    if (values.contains("speed")) {
        speed = values["speed"];
    }

    if (values.contains("waitTime")) {
        waitTime = values["waitTime"];
    }

    entity.add(RailsControl(speed, checkpoints, waitTime, isCycle));
}

void addComponentDraw(nlohmann::json& values, nlohmann::json& allObjects, std::unordered_map<s32, s32>& idToIndex, s32 thisId, ActiveLevel& level,
                      ecs::Entity entity) {
    s32 ix = idToIndex[thisId];
    Vector2i frameSizeTexels = {allObjects[ix]["width"], allObjects[ix]["height"]};

    // ARGB TODO default values
    std::string hexcode = "#ffffffff";
    if (values.contains("Color")) {
        hexcode = values["Color"];
    }
    s32 r, g, b;
    hexcode = hexcode.erase(0, 3);  // remove "#" and alpha
    std::istringstream(hexcode.substr(0, 2)) >> std::hex >> r;
    std::istringstream(hexcode.substr(2, 2)) >> std::hex >> g;
    std::istringstream(hexcode.substr(4, 2)) >> std::hex >> b;

    // TODO parse depth
    entity.add(Draw(RGB(r, g, b), frameSizeTexels, Depth::Player));
}

void addComponentSolidCollider(nlohmann::json& values, nlohmann::json& allObjects, std::unordered_map<s32, s32>& idToIndex, s32 thisId,
                               ActiveLevel& level, ecs::Entity entity) {
    // TODO default values
    s32 halflenTexelsX = 0;
    s32 halflenTexelsY = 0;

    if (values.contains("halflenTexelsX")) {
        halflenTexelsX = values["halflenTexelsX"];
    }
    if (values.contains("halflenTexelsY")) {
        halflenTexelsY = values["halflenTexelsY"];
    }

    Vector2i half = Transform::texels(halflenTexelsX, halflenTexelsY).position;
    Vector2i center = entity.get<Transform>().position + Vector2i(0, half.y());
    entity.add(SolidCollider(toFloatVec(center), half));
}

// Tiled doesn't support array properties so I have to do some BS
// TODO maybe I can just iterate the polyline points and assume they're in order?
void loadCheckpoints(nlohmann::json& checkpointData, std::vector<RailsControl::CheckPoint>& dstCheckpoints, ActiveLevel& level) {
    std::string checkpointType = checkpointData["type"];
    if (checkpointType == "CheckpointsN2") {
        loadCheckpointN2(checkpointData, dstCheckpoints, level);
    } else {
        print("ERROR -- unrecognized checkpoint type: ", checkpointType);
    }
}

void loadCheckpointN2(nlohmann::json& checkpointData, std::vector<RailsControl::CheckPoint>& dstCheckpoints, ActiveLevel& level) {
    RailsControl::CheckPoint point1;
    RailsControl::CheckPoint point2;

    s32 parentX = checkpointData["x"];
    s32 parentY = checkpointData["y"];

    s32 i = 0;
    // add half a tile of height to each point, since they describe the center of a tile, and RailsControl expects transform coordinates (at bottom of
    // tile)
    for (auto& point : checkpointData["polyline"]) {
        s32 x = point["x"];
        s32 y = point["y"];
        if (i == 0) {
            point1.position =
                Transform::texels(x + parentX, level.sizeTexels.y() - (parentY + y) + TEXELS_PER_TILE / 2).position + level.worldOffsetPixels;
        } else {
            point2.position =
                Transform::texels(x + parentX, level.sizeTexels.y() - (parentY + y) + TEXELS_PER_TILE / 2).position + level.worldOffsetPixels;
        }
        i++;
    }

    for (auto& moveProperty : checkpointData["properties"]) {
        std::string name = moveProperty["name"];
        if (name == "move1") {
            s32 moveIx = moveProperty["value"];
            point1.movement = static_cast<RailsControl::Movement>(moveIx);
        } else if (name == "move2") {
            s32 moveIx = moveProperty["value"];
            point2.movement = static_cast<RailsControl::Movement>(moveIx);
        }
    }

    dstCheckpoints.push_back(point1);
    dstCheckpoints.push_back(point2);
}

}  // namespace whal
