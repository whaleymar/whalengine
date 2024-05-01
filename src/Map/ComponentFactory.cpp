#include "ComponentFactory.h"

#include <sstream>
#include "ECS/Components/RigidBody.h"
#include "json.hpp"

#include "Gfx/GLResourceManager.h"
#include "Gfx/GfxUtil.h"
#include "Map/Level.h"
#include "Map/Tiled.h"

#include "ECS/Components/Collision.h"
#include "ECS/Components/Draw.h"
#include "ECS/Components/RailsControl.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Velocity.h"
#include "ECS/Lib/ECS.h"

#include "Util/Print.h"

namespace whal {

static NameToCreator<ComponentAdder> S_COMPONENT_ENTRIES[] = {
    {"Component_RailsControl", addComponentRailsControl},
    // {"Animator", addComponentAnimator},
    {"Component_ActorCollider", addComponentActorCollider},
    {"Component_SolidCollider", addComponentSolidCollider},
    {"Component_Draw", addComponentDraw},
    {"Component_Sprite_NoAnim", addComponentSprite},
    // {"Lifetime", addComponentLifetime},
    // {"PlayerControlRB", addComponentPlayerControlRB},
    // {"PlayerControlFree", addComponentPlayerControlFree},
    // {"Children", addComponentChildren},
    // {"Follow", addComponentFollow},
    {"Component_RigidBody", addComponentRigidBody},
    // {"Tags", addComponentTags},
    {"Component_Velocity", addComponentVelocity},
};

ComponentFactory::ComponentFactory() : Factory<ComponentAdder>("ComponentFactory") {
    initFactory(S_COMPONENT_ENTRIES);
}

void addComponentVelocity(nlohmann::json& values, nlohmann::json& allObjects, std::unordered_map<s32, s32>& idToIndex, s32 thisId, ActiveLevel& level,
                          ecs::Entity entity) {
    f32 velx = 0;
    f32 vely = 0;

    if (values.contains("velX")) {
        velx = values["velX"];
    }
    if (values.contains("velY")) {
        vely = values["velY"];
    }

    entity.add(Velocity({velx, vely}));
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
    entity.add(Draw(RGB::fromInts(r, g, b), frameSizeTexels, Depth::Player));
}

void addComponentSprite(nlohmann::json& values, nlohmann::json& allObjects, std::unordered_map<s32, s32>& idToIndex, s32 thisId, ActiveLevel& level,
                        ecs::Entity entity) {
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
    std::string spritePath = "";
    if (values.contains("Sprite")) {
        spritePath = values["Sprite"];
        print(spritePath);
        std::replace(spritePath.begin(), spritePath.end(), '\\', '/');
    }
    auto frameOpt = GLResourceManager::getInstance().getTexture(TEXNAME_SPRITE).getFrame(spritePath.c_str());
    if (frameOpt) {
        Sprite sprite = Sprite(Depth::Player, frameOpt.value(), RGB::fromInts(r, g, b));
        entity.add(sprite);
    } else {
        print("Coudn't find frame for sprite:", spritePath);
        // add draw instead
        s32 ix = idToIndex[thisId];
        Vector2i frameSizeTexels = {allObjects[ix]["width"], allObjects[ix]["height"]};
        entity.add(Draw(RGB::fromInts(r, g, b), frameSizeTexels, Depth::Player));
    }
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

void addComponentActorCollider(nlohmann::json& values, nlohmann::json& allObjects, std::unordered_map<s32, s32>& idToIndex, s32 thisId,
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
    entity.add(ActorCollider(toFloatVec(center), half));
}

void addComponentRigidBody(nlohmann::json& values, nlohmann::json& allObjects, std::unordered_map<s32, s32>& idToIndex, s32 thisId,
                           ActiveLevel& level, ecs::Entity entity) {
    f32 jumpInitialVelocity = 15.5;
    f32 jumpSecondsMax = 1.25;
    f32 coyoteTimeSecondsMax = 0.1;

    if (values.contains("jumpInitialVelocity")) {
        jumpInitialVelocity = values["jumpInitialVelocity"];
    }
    if (values.contains("jumpSecondsMax")) {
        jumpSecondsMax = values["jumpSecondsMax"];
    }
    if (values.contains("coyoteTimeSecondsMax")) {
        coyoteTimeSecondsMax = values["coyoteTimeSecondsMax"];
    }

    entity.add(RigidBody(jumpInitialVelocity, jumpSecondsMax, coyoteTimeSecondsMax));
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
        Vector2i mapPos = {x + parentX, parentY + y - static_cast<s32>(TEXELS_PER_TILE) / 2};
        if (i == 0) {
            point1.position = getTransformFromMapPosition(mapPos, {0, 0}, level).position;
        } else {
            point2.position = getTransformFromMapPosition(mapPos, {0, 0}, level).position;
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
