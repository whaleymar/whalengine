#include "ComponentFactory.h"

#include "ECS/Components/RigidBody.h"
#include "ECS/Systems/TagTrackers.h"
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

static const char* KEY_MEMBERS = "members";
static const char* KEY_NAME = "name";
static const char* KEY_VALUE = "value";

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
    {"Component_Follow", addComponentFollow},
    {"Component_RigidBody", addComponentRigidBody},
    // {"Tags", addComponentTags},
    {"Component_Velocity", addComponentVelocity},
};

ComponentFactory::ComponentFactory() : Factory<ComponentAdder>("ComponentFactory") {
    initFactory(S_COMPONENT_ENTRIES);
}

void ComponentFactory::makeDefaultComponent(nlohmann::json property) {
    // who cares

    std::string componentName = property[KEY_NAME];
    ComponentAdder creatorFunc = nullptr;
    if (getEntryIndex(componentName.c_str(), &creatorFunc) == -1) {
        return;
    }

    if (componentName == "Component_RailsControl") {
        for (auto& member : property[KEY_MEMBERS]) {
            std::string memberName = member[KEY_NAME];
            if (memberName == "isCycle") {
                DefaultRailsControl.isCycle = member[KEY_VALUE];
            } else if (memberName == "speed") {
                DefaultRailsControl.speed = member[KEY_VALUE];
            } else if (memberName == "waitTime") {
                DefaultRailsControl.waitTime = member[KEY_VALUE];
            } else {
                print("Skipping member ", memberName, "for", componentName);
            }
        }

    } else if (componentName == "Component_ActorCollider") {
        Vector2i half;
        for (auto& member : property[KEY_MEMBERS]) {
            std::string memberName = member[KEY_NAME];
            if (memberName == "halflenTexelsX") {
                half.e[0] = member[KEY_VALUE];
            } else if (memberName == "halflenTexelsY") {
                half.e[1] = member[KEY_VALUE];
            } else {
                print("Skipping member ", memberName, "for", componentName);
            }
        }
        half = Transform::texels(half.x(), half.y()).position;
        DefaultActorCollider.getCollider().setHalflen(half);

    } else if (componentName == "Component_SolidCollider") {
        Vector2i half;
        for (auto& member : property[KEY_MEMBERS]) {
            std::string memberName = member[KEY_NAME];
            if (memberName == "halflenTexelsX") {
                half.e[0] = member[KEY_VALUE];
            } else if (memberName == "halflenTexelsY") {
                half.e[1] = member[KEY_VALUE];
            } else {
                print("Skipping member ", memberName, "for", componentName);
            }
        }
        half = Transform::texels(half.x(), half.y()).position;
        DefaultSolidCollider.getCollider().setHalflen(half);

    } else if (componentName == "Component_Draw") {
        for (auto& member : property[KEY_MEMBERS]) {
            std::string memberName = member[KEY_NAME];
            if (memberName == "Color") {
                std::string hexString = member[KEY_VALUE];
                DefaultDraw.setColor(RGB::fromHexStringARGB(hexString));
            } else {
                print("Skipping member ", memberName, "for", componentName);
            }
        }
    } else if (componentName == "Component_Sprite_NoAnim") {
        for (auto& member : property[KEY_MEMBERS]) {
            std::string memberName = member[KEY_NAME];
            if (memberName == "Color") {
                std::string hexString = member[KEY_VALUE];
                DefaultSprite.setColor(RGB::fromHexStringARGB(hexString));
            } else if (memberName == "Sprite") {
                // do nothing
            } else {
                print("Skipping member ", memberName, "for", componentName);
            }
        }
    } else if (componentName == "Component_RigidBody") {
        for (auto& member : property[KEY_MEMBERS]) {
            std::string memberName = member[KEY_NAME];
            if (memberName == "coyoteTimeSecondsMax") {
                DefaultRigidBody.coyoteTimeSecondsMax = member[KEY_VALUE];
            } else if (memberName == "jumpInitialVelocity") {
                DefaultRigidBody.jumpInitialVelocity = member[KEY_VALUE];
            } else if (memberName == "jumpSecondsMax") {
                DefaultRigidBody.jumpSecondsMax = member[KEY_VALUE];
            } else {
                print("Skipping member ", memberName, "for", componentName);
            }
        }
    } else if (componentName == "Component_Velocity") {
        for (auto& member : property[KEY_MEMBERS]) {
            std::string memberName = member[KEY_NAME];
            if (memberName == "velX") {
                DefaultVelocity.stable.e[0] = member[KEY_VALUE];
            } else if (memberName == "velY") {
                DefaultVelocity.stable.e[1] = member[KEY_VALUE];
            } else {
                print("Skipping member ", memberName, "for", componentName);
            }
        }
    } else if (componentName == "Component_Follow") {
        for (auto& member : property[KEY_MEMBERS]) {
            std::string memberName = member[KEY_NAME];
            if (memberName == "lookAheadX") {
                DefaultFollow.lookAheadTexels.e[0] = member[KEY_VALUE];
            } else if (memberName == "lookAheadY") {
                DefaultFollow.lookAheadTexels.e[1] = member[KEY_VALUE];
            } else if (memberName == "deadZoneX") {
                DefaultFollow.deadZoneTexels.e[0] = member[KEY_VALUE];
            } else if (memberName == "deadZoneY") {
                DefaultFollow.deadZoneTexels.e[1] = member[KEY_VALUE];
            } else if (memberName == "dampingX") {
                DefaultFollow.damping.e[0] = member[KEY_VALUE];
            } else if (memberName == "dampingY") {
                DefaultFollow.damping.e[1] = member[KEY_VALUE];
            } else if (memberName == "FollowTarget") {
                // do nothing
            } else if (memberName == "boundsHalflenX") {
                DefaultFollow.boundsXTexels = Vector2i(member[KEY_VALUE], member[KEY_VALUE]);
            } else if (memberName == "boundsHalflenY") {
                DefaultFollow.boundsYTexels = Vector2i(member[KEY_VALUE], member[KEY_VALUE]);
            } else {
                print("Skipping member ", memberName, "for", componentName);
            }
        }
    } else {
        print("unhandled default component type: ", componentName);
    }
}

void addComponentVelocity(nlohmann::json& values, nlohmann::json& allObjects, std::unordered_map<s32, s32>& idToIndex, s32 thisId, ActiveLevel& level,
                          ecs::Entity entity) {
    Velocity velocity = ComponentFactory::DefaultVelocity;

    if (values.contains("velX")) {
        velocity.stable.e[0] = values["velX"];
    }
    if (values.contains("velY")) {
        velocity.stable.e[1] = values["velY"];
    }

    entity.add(velocity);
}

void addComponentRailsControl(nlohmann::json& values, nlohmann::json& allObjects, std::unordered_map<s32, s32>& idToIndex, s32 thisId,
                              ActiveLevel& level, ecs::Entity entity) {
    std::vector<RailsControl::CheckPoint> checkpoints;
    if (values.contains("Checkpoints")) {
        s32 id = values["Checkpoints"];
        nlohmann::json checkPointObj = allObjects[idToIndex[id]];
        loadCheckpoints(checkPointObj, checkpoints, level);
    }

    // RailsControl rails = ComponentFactory::DefaultRailsControl;
    RailsControl rails(ComponentFactory::DefaultRailsControl.speed, checkpoints, ComponentFactory::DefaultRailsControl.waitTime,
                       ComponentFactory::DefaultRailsControl.isCycle);
    // rails.setCheckpoints(checkpoints);
    print("new railscontrol's checkpoints are at ", &rails.mCheckpoints);

    if (values.contains("isCycle")) {
        rails.isCycle = values["isCycle"];
    }
    if (values.contains("speed")) {
        rails.speed = values["speed"];
    }
    if (values.contains("waitTime")) {
        rails.waitTime = values["waitTime"];
    }

    entity.add(rails);
}

void addComponentDraw(nlohmann::json& values, nlohmann::json& allObjects, std::unordered_map<s32, s32>& idToIndex, s32 thisId, ActiveLevel& level,
                      ecs::Entity entity) {
    s32 ix = idToIndex[thisId];
    Vector2i frameSizeTexels = {allObjects[ix]["width"], allObjects[ix]["height"]};

    // TODO parse depth
    Draw draw = ComponentFactory::DefaultDraw;
    draw.setFrameSize(frameSizeTexels);

    // ARGB
    if (values.contains("Color")) {
        std::string hexcode = "#ffffffff";
        hexcode = values["Color"];
        RGB color = RGB::fromHexStringARGB(hexcode);
        draw.setColor(color);
    }

    entity.add(draw);
}

void addComponentSprite(nlohmann::json& values, nlohmann::json& allObjects, std::unordered_map<s32, s32>& idToIndex, s32 thisId, ActiveLevel& level,
                        ecs::Entity entity) {
    Sprite sprite = ComponentFactory::DefaultSprite;

    // ARGB
    if (values.contains("Color")) {
        std::string hexcode = "#ffffffff";
        hexcode = values["Color"];
        RGB color = RGB::fromHexStringARGB(hexcode);
        sprite.setColor(color);
    }

    std::string spritePath = "";
    if (values.contains("Sprite")) {
        spritePath = values["Sprite"];
        std::replace(spritePath.begin(), spritePath.end(), '\\', '/');
    }
    auto frameOpt = GLResourceManager::getInstance().getTexture(TEXNAME_SPRITE).getFrame(spritePath.c_str());
    if (frameOpt) {
        // TODO parse depth
        sprite.setFrame(frameOpt.value());
        entity.add(sprite);
    } else {
        print("Coudn't find frame for sprite:", spritePath);
        // add draw instead
        s32 ix = idToIndex[thisId];
        Vector2i frameSizeTexels = {allObjects[ix]["width"], allObjects[ix]["height"]};
        Draw draw = ComponentFactory::DefaultDraw;
        draw.setFrameSize(frameSizeTexels);
        entity.add(draw);
    }
}

void addComponentSolidCollider(nlohmann::json& values, nlohmann::json& allObjects, std::unordered_map<s32, s32>& idToIndex, s32 thisId,
                               ActiveLevel& level, ecs::Entity entity) {
    SolidCollider solid = ComponentFactory::DefaultSolidCollider;
    Vector2i halflenTexels = solid.getCollider().half * static_cast<s32>(PIXELS_PER_TEXEL);

    if (values.contains("halflenTexelsX")) {
        halflenTexels.e[0] = values["halflenTexelsX"];
    }
    if (values.contains("halflenTexelsY")) {
        halflenTexels.e[1] = values["halflenTexelsY"];
    }

    Vector2i half = Transform::texels(halflenTexels.x(), halflenTexels.y()).position;
    Vector2i center = entity.get<Transform>().position + Vector2i(0, half.y());
    entity.add(SolidCollider(toFloatVec(center), half));
}

void addComponentActorCollider(nlohmann::json& values, nlohmann::json& allObjects, std::unordered_map<s32, s32>& idToIndex, s32 thisId,
                               ActiveLevel& level, ecs::Entity entity) {
    ActorCollider actor = ComponentFactory::DefaultActorCollider;
    Vector2i halflenTexels = actor.getCollider().half * static_cast<s32>(PIXELS_PER_TEXEL);

    if (values.contains("halflenTexelsX")) {
        halflenTexels.e[0] = values["halflenTexelsX"];
    }
    if (values.contains("halflenTexelsY")) {
        halflenTexels.e[1] = values["halflenTexelsY"];
    }

    Vector2i half = Transform::texels(halflenTexels.x(), halflenTexels.y()).position;
    Vector2i center = entity.get<Transform>().position + Vector2i(0, half.y());
    entity.add(ActorCollider(toFloatVec(center), half));
}

void addComponentFollow(nlohmann::json& values, nlohmann::json& allObjects, std::unordered_map<s32, s32>& idToIndex, s32 thisId, ActiveLevel& level,
                        ecs::Entity entity) {
    entity.add(loadFollowComponent(values, level));
}

void addComponentRigidBody(nlohmann::json& values, nlohmann::json& allObjects, std::unordered_map<s32, s32>& idToIndex, s32 thisId,
                           ActiveLevel& level, ecs::Entity entity) {
    RigidBody rb = ComponentFactory::DefaultRigidBody;

    if (values.contains("jumpInitialVelocity")) {
        rb.jumpInitialVelocity = values["jumpInitialVelocity"];
    }
    if (values.contains("jumpSecondsMax")) {
        rb.jumpSecondsMax = values["jumpSecondsMax"];
    }
    if (values.contains("coyoteTimeSecondsMax")) {
        rb.coyoteTimeSecondsMax = values["coyoteTimeSecondsMax"];
    }

    entity.add(rb);
}

Follow loadFollowComponent(nlohmann::json& values, ActiveLevel& level) {
    Follow follow = ComponentFactory::DefaultFollow;
    if (values.contains("FollowTarget")) {
        // TODO compare to entities with Name component and follow first one which matches
        // std::string followTargetName = values["FollowTarget"];
        follow.targetEntity = PlayerSystem::instance()->first();
    }
    if (values.contains("dampingX")) {
        follow.damping.e[0] = values["dampingX"];
    }
    if (values.contains("dampingY")) {
        follow.damping.e[1] = values["dampingY"];
    }
    if (values.contains("deadZoneX")) {
        follow.deadZoneTexels.e[0] = values["deadZoneX"];
    }
    if (values.contains("deadZoneY")) {
        follow.deadZoneTexels.e[1] = values["deadZoneY"];
    }
    if (values.contains("lookAheadX")) {
        follow.lookAheadTexels.e[0] = values["lookAheadX"];
    }
    if (values.contains("lookAheadY")) {
        follow.lookAheadTexels.e[1] = values["lookAheadY"];
    }
    if (values.contains("boundsHalflenX")) {
        follow.boundsXTexels = Vector2i(values["boundsHalflenX"], values["boundsHalflenX"]);
    }
    if (values.contains("boundsHalflenY")) {
        follow.boundsYTexels = Vector2i(values["boundsHalflenY"], values["boundsHalflenY"]);
    }

    // convert bounds from local half length to world coords
    Vector2i levelPosTexels = Vector2i(level.worldOffsetPixels.x() / SPIXELS_PER_TEXEL, level.worldOffsetPixels.y() / SPIXELS_PER_TEXEL) +
                              Vector2i(level.sizeTexels.x() / 2, level.sizeTexels.y() / 2);
    follow.boundsXTexels = {levelPosTexels.x() - follow.boundsXTexels.x(), levelPosTexels.x() + follow.boundsXTexels.x()};
    follow.boundsYTexels = {levelPosTexels.y() - follow.boundsYTexels.y(), levelPosTexels.y() + follow.boundsYTexels.y()};
    return follow;
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
    for (auto& point : checkpointData["polyline"]) {
        s32 x = point["x"];
        s32 y = point["y"];
        Vector2i mapPos = {x + parentX, parentY + y};
        if (i == 0) {
            point1.position = getTransformFromMapPosition(mapPos, {0, 0}, level, true).position;
        } else {
            point2.position = getTransformFromMapPosition(mapPos, {0, 0}, level, true).position;
        }
        i++;
    }

    for (auto& moveProperty : checkpointData["properties"]) {
        std::string name = moveProperty[KEY_NAME];
        if (name == "move1") {
            s32 moveIx = moveProperty[KEY_VALUE];
            point1.movement = static_cast<RailsControl::Movement>(moveIx);
        } else if (name == "move2") {
            s32 moveIx = moveProperty[KEY_VALUE];
            point2.movement = static_cast<RailsControl::Movement>(moveIx);
        }
    }

    dstCheckpoints.push_back(point1);
    dstCheckpoints.push_back(point2);
}

}  // namespace whal
