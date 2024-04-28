#pragma once

#include <set>

#include "ECS/Lib/ECS.h"
#include "Util/Vector.h"

namespace whal {

class SolidCollider;
struct TileMap;

// TODO subscribe to death event
// TODO private constructor so levels are only created through loadLevel?
struct Level {
    const char* name;
    std::string filepath;     // used for level comparisons
    Vector2f worldPosOrigin;  // top left
    Vector2f sizeTexels;
    std::set<ecs::Entity> childEntities;
};

// TODO game should have a currentScene member which is used for hot reloading
struct Scene {
    std::vector<Level> allLevels;
    std::vector<Level> loadedLevels;

    Expected<Level> getLevelAt(Vector2f worldPosTexels);
};

Expected<Level> loadLevel(const char* levelPath);
void unloadLevel(Level& level);
void removeEntityFromLevel(ecs::Entity entity);
void makeCollisionMesh(std::vector<std::vector<s32>>& collisionGrid, std::vector<SolidCollider>& dstColliders, const TileMap& map);

}  // namespace whal
