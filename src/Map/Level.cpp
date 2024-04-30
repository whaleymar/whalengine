#include "Level.h"

#include "ECS/Components/Collision.h"
#include "ECS/Components/Draw.h"
#include "ECS/Components/Transform.h"
#include "ECS/Entities/Block.h"
#include "Game/Game.h"
#include "Gfx/GfxUtil.h"
#include "Gfx/Texture.h"
#include "Gfx/VertexObject.h"
#include "Tiled.h"
#include "Util/Print.h"
#include "Util/Vector.h"

namespace whal {

std::optional<Level> Scene::getLevelAt(Vector2f worldPosTexels) {
    for (Level lvl : allLevels) {
        if (worldPosTexels.x() >= lvl.worldPosOriginTexels.x() && worldPosTexels.x() < (lvl.worldPosOriginTexels.x() + lvl.sizeTexels.x()) &&
            worldPosTexels.y() < lvl.worldPosOriginTexels.y() && worldPosTexels.y() >= (lvl.worldPosOriginTexels.y() - lvl.sizeTexels.y())) {
            return lvl;
        }
    }
    return std::nullopt;
}

std::optional<Error> loadLevel(const Level level) {
    Vector2i worldOffsetPixels = Transform::texels(level.worldPosOriginTexels.x(), level.worldPosOriginTexels.y() - level.sizeTexels.y()).position;
    ActiveLevel lvl = {level, "", {}, worldOffsetPixels};
    TileMap map = TileMap::parse(level.filepath.c_str(), lvl);
    print("loaded", map.name);

    std::vector<std::vector<s32>> collisionGrid;
    for (s32 x = 0; x < map.widthTiles; x++) {
        std::vector<s32> collisionColumn;
        for (s32 y = 0; y < map.heightTiles; y++) {
            Transform trans = Transform(Transform::tiles(x, map.heightTiles - y).position + worldOffsetPixels);
            s32 ix = map.widthTiles * y + x;
            s32 blockID = map.baseLayer.data.get()[ix];

            if (blockID != 0) {
                // for now, am assuming everything in the base layer has collision
                collisionColumn.push_back(1);

                Expected<Frame> frame = getTileFrame(map, blockID);
                if (!frame.isExpected()) {
                    print(frame.error());
                    auto eEntity = createBlock(trans);
                    if (eEntity.isExpected()) {
                        lvl.childEntities.insert(eEntity.value());
                    }
                } else {
                    Sprite sprite = Sprite(Depth::Player, frame.value());
                    auto eEntity = createDecal(trans, sprite);
                    if (eEntity.isExpected()) {
                        lvl.childEntities.insert(eEntity.value());
                    }
                }
            } else {
                collisionColumn.push_back(0);
            }

            blockID = map.backgroundLayer.data.get()[ix];
            if (blockID != 0) {
                Expected<Frame> frame = getTileFrame(map, blockID);
                if (!frame.isExpected()) {
                    print(frame.error());
                } else {
                    Sprite sprite = Sprite(Depth::BackgroundNoParallax, frame.value());
                    auto eEntity = createDecal(trans, sprite);
                    if (eEntity.isExpected()) {
                        lvl.childEntities.insert(eEntity.value());
                    }
                }
            }

            blockID = map.foregroundLayer.data.get()[ix];
            if (blockID != 0) {
                Expected<Frame> frame = getTileFrame(map, blockID);
                if (!frame.isExpected()) {
                    print(frame.error());
                } else {
                    Sprite sprite = Sprite(Depth::Foreground, frame.value());
                    auto eEntity = createDecal(trans, sprite);
                    if (eEntity.isExpected()) {
                        lvl.childEntities.insert(eEntity.value());
                    }
                }
            }
        }
        collisionGrid.push_back(collisionColumn);
    }

    std::vector<SolidCollider> mesh;
    // auto now = std::chrono::steady_clock::now();
    makeCollisionMesh(collisionGrid, lvl);
    // print("mesh creation time: ", std::chrono::duration<f32>(std::chrono::steady_clock::now() - now).count());

    Game::instance().getScene().loadedLevels.push_back(lvl);

    return std::nullopt;
}

void unloadAndRemoveLevel(ActiveLevel& level) {
    // remove from Scene's list of loaded levels first,
    // so the EntityKilled listener doesn't mutate the list we're iterating
    // also copy it so erasing it doesn't invalidate our pointer

    ActiveLevel copy = level;
    Scene& scene = Game::instance().getScene();
    for (auto it = scene.loadedLevels.begin(); it != scene.loadedLevels.end(); ++it) {
        auto& lvl = *it;
        if (lvl == level) {
            scene.loadedLevels.erase(it);
            break;
        }
    }
    unloadLevel(copy);
}

void unloadLevel(ActiveLevel& level) {
    for (auto entity : level.childEntities) {
        entity.kill();
    }
    print("unloaded level:", level.name);
}

void removeEntityFromLevel(ecs::Entity entity) {
    Scene& scene = Game::instance().getScene();
    for (auto& lvl : scene.loadedLevels) {
        if (lvl.childEntities.erase(entity)) {
            break;
        }
    }
}

void addCollider(ActiveLevel& lvl, std::pair<s32, s32> startPoint, std::pair<s32, s32> endPoint) {
    s32 meshWidthTiles = endPoint.first - startPoint.first + 1;
    s32 meshHeightTiles = endPoint.second - startPoint.second + 1;
    constexpr f32 pixelsPerTexel = static_cast<f32>(PIXELS_PER_TEXEL);
    constexpr f32 pixelsPerTile = pixelsPerTexel * TEXELS_PER_TILE;
    constexpr s32 s_pixelsPerTile = static_cast<s32>(TEXELS_PER_TILE) * static_cast<s32>(PIXELS_PER_TEXEL);

    f32 centerX = lvl.worldPosOriginTexels.x() * pixelsPerTexel + static_cast<f32>(startPoint.first) * pixelsPerTile +
                  static_cast<f32>(meshWidthTiles - 1) * pixelsPerTile * 0.5;
    f32 centerY = lvl.worldPosOriginTexels.y() * pixelsPerTexel - static_cast<f32>(startPoint.second) * pixelsPerTile -
                  static_cast<f32>(meshHeightTiles - 2) * pixelsPerTile * 0.5;

    Vector2f center = {centerX, centerY};
    Vector2i halflen = {meshWidthTiles * s_pixelsPerTile / 2, meshHeightTiles * s_pixelsPerTile / 2};
    SolidCollider collider = SolidCollider(center, halflen);

    auto eEntity = ecs::ECS::getInstance().entity();
    if (!eEntity.isExpected()) {
        print("Error creating entity for mesh");
    } else {
        auto entity = eEntity.value();
        entity.add(collider);
        entity.add(Transform(collider.getCollider().getPositionEdge(Vector2i::unitDown)));
        lvl.childEntities.insert(entity);
    }
}

void makeCollisionMesh(std::vector<std::vector<s32>>& collisionGrid, ActiveLevel& lvl) {
    // could be a LOT faster with std::vector<bool> + bitwise ops
    // timed @ 0.004 seconds for 1 level (quarter frame; can be asynch?)

    using Point = std::pair<s32, s32>;
    std::set<Point> visited;

    bool isStarted = false;
    Point startPoint;
    Point endPoint;

    for (size_t x = 0; x < collisionGrid.size(); x++) {
        for (size_t y = 0; y < collisionGrid[0].size(); y++) {
            Point point = {x, y};
            if (visited.find(point) != visited.end()) {
                continue;
            }
            visited.insert(point);

            bool isCollisionTile = collisionGrid[x][y] > 0;
            if (isCollisionTile) {
                if (!isStarted) {
                    isStarted = true;
                    startPoint = point;
                }
                endPoint = point;
            }
            if (!isCollisionTile || y == (collisionGrid[0].size() - 1) || (x == (collisionGrid.size() - 1))) {
                if (!isStarted) {
                    continue;
                }
                // fix endpoint's x coord, try expanding vertically
                bool isDone = false;
                bool endedEarly = false;
                Point newPoint;

                for (size_t xNew = endPoint.first + 1; xNew < collisionGrid.size(); xNew++) {
                    std::vector<Point> toInsert;
                    for (size_t yNew = startPoint.second; yNew <= static_cast<size_t>(endPoint.second); yNew++) {
                        newPoint = {xNew, yNew};  // pretty sure visit check is unecessary
                        toInsert.push_back(newPoint);
                        if (!collisionGrid[xNew][yNew]) {
                            endedEarly = true;
                            break;
                        }
                        isDone = (xNew == collisionGrid.size() - 1);
                    }
                    if (isDone || endedEarly) {
                        if (isDone) {
                            endPoint = newPoint;
                            for (Point p : toInsert) {
                                visited.insert(p);
                            }
                        }
                        break;
                    } else {
                        endPoint = newPoint;
                        for (Point p : toInsert) {
                            visited.insert(p);
                        }
                    }
                }
                addCollider(lvl, startPoint, endPoint);
                isStarted = false;
            }
        }
    }
}

}  // namespace whal
