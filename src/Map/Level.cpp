#include "Level.h"
// #include <chrono>

#include "ECS/Components/Collision.h"
#include "ECS/Components/Draw.h"
#include "ECS/Components/Transform.h"
#include "ECS/Entities/Block.h"
#include "Gfx/GfxUtil.h"
#include "Gfx/Texture.h"
#include "Gfx/VertexObject.h"
#include "Tiled.h"
#include "Util/Print.h"

namespace whal {

Expected<ActiveLevel> loadLevel(const char* levelPath) {
    TileMap map = TileMap::parse(levelPath);
    ActiveLevel lvl = {{"asdf",  // TODO
                        levelPath,
                        {0, 0},  // TODO
                        Vector2f(map.widthTiles * map.tileSize, map.heightTiles * map.tileSize)},
                       {}};

    std::vector<std::vector<s32>> collisionGrid;
    for (s32 x = 0; x < map.widthTiles; x++) {
        std::vector<s32> collisionColumn;
        for (s32 y = 0; y < map.heightTiles; y++) {
            s32 ix = map.widthTiles * y + x;
            s32 blockID = map.baseLayer.data.get()[ix];

            if (blockID != 0) {
                // for now, am assuming everything in the base layer has collision
                collisionColumn.push_back(1);

                Expected<Frame> frame = getTileFrame(map, blockID);
                if (!frame.isExpected()) {
                    print(frame.error());
                    auto eEntity = createBlock(Transform::tiles(x, map.heightTiles - y));
                    if (eEntity.isExpected()) {
                        lvl.childEntities.insert(eEntity.value());
                    }
                } else {
                    Sprite sprite = Sprite(Depth::Player, frame.value());
                    auto eEntity = createDecal(Transform::tiles(x, map.heightTiles - y), sprite);
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
                    auto eEntity = createDecal(Transform::tiles(x, map.heightTiles - y), sprite);
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
                    auto eEntity = createDecal(Transform::tiles(x, map.heightTiles - y), sprite);
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
    makeCollisionMesh(collisionGrid, mesh, map);
    for (auto collider : mesh) {
        auto eEntity = ecs::ECS::getInstance().entity();
        if (!eEntity.isExpected()) {
            print("Error creating entity for mesh");
            continue;
        }
        auto entity = eEntity.value();
        entity.add(collider);
        lvl.childEntities.insert(entity);
    }

    // print("mesh creation time: ", std::chrono::duration<f32>(std::chrono::steady_clock::now() - now).count());

    return lvl;
}

void unloadLevel(ActiveLevel& level) {
    // remove from Scene's list of loaded levels first,
    // so the EntityKilled listener doesn't mutate the list we're iterating

    // Scene scene = game.getScene();
    // for (auto it = scene.loadedLevels.begin(); it != scene.loadedLevels.end(); ++it) {
    //     auto& lvl = *it;
    //     if (lvl.filepath == level.filepath) {
    //         scene.loadedLevels.erase(it);
    //         break;
    //     }
    // }
    // for (auto entity : level.childEntities) {
    //     entity.kill();
    // }
}

void removeEntityFromLevel(ecs::Entity entity) {
    // TODO

    // Scene scene = game.getScene();
    // for (auto& lvl : scene.loadedLevels) {
    //     if (lvl.childEntities.erase(entity)) {
    //         break;
    //     }
    // }
}

void makeCollisionMesh(std::vector<std::vector<s32>>& collisionGrid, std::vector<SolidCollider>& dstColliders, const TileMap& map) {
    // could be a LOT faster with std::vector<bool> + bitwise ops
    // timed @ 0.004 seconds for 1 level (quarter frame; can be asynch?)

    using Point = std::pair<s32, s32>;
    std::set<Point> visited;

    bool isStarted = false;
    Point startPoint;
    Point endPoint;

    for (size_t rowIx = 0; rowIx < collisionGrid.size(); rowIx++) {
        for (size_t colIx = 0; colIx < collisionGrid[0].size(); colIx++) {
            Point point = {rowIx, colIx};
            if (visited.find(point) != visited.end()) {
                continue;
            }
            visited.insert(point);

            if (collisionGrid[rowIx][colIx]) {
                if (!isStarted) {
                    isStarted = true;
                    startPoint = point;
                }
                endPoint = point;
            } else {
                if (!isStarted) {
                    continue;
                }
                // fix endpoint's x coord, try expanding vertically
                bool isDone = false;
                bool endedEarly = false;
                Point newPoint;
                for (size_t newRowIx = endPoint.first + 1; newRowIx < collisionGrid.size(); newRowIx++) {
                    std::vector<Point> toInsert;
                    for (size_t newColIx = startPoint.second; newColIx <= static_cast<size_t>(endPoint.second); newColIx++) {
                        newPoint = {newRowIx, newColIx};  // pretty sure visit check is unecessary
                        toInsert.push_back(newPoint);
                        if (!collisionGrid[newRowIx][newColIx]) {
                            endedEarly = true;
                            break;
                        }
                        isDone = newRowIx == collisionGrid.size();
                    }
                    if (isDone || endedEarly) {
                        if (isDone) {
                            endPoint = newPoint;
                            for (Point p : toInsert) {
                                visited.insert(p);
                            }
                        }
                        s32 meshWidthTiles = endPoint.first - startPoint.first + 1;
                        s32 meshHeightTiles = endPoint.second - startPoint.second + 1;
                        constexpr f32 pixelsPerTile = static_cast<f32>(PIXELS_PER_TEXEL) * TEXELS_PER_TILE;
                        constexpr s32 s_pixelsPerTile = static_cast<s32>(TEXELS_PER_TILE) * static_cast<s32>(PIXELS_PER_TEXEL);

                        f32 centerX = static_cast<f32>(startPoint.first) * pixelsPerTile + static_cast<f32>(meshWidthTiles - 1) * pixelsPerTile * 0.5;
                        f32 centerY = static_cast<f32>(map.heightTiles) * pixelsPerTile - static_cast<f32>(startPoint.second) * pixelsPerTile -
                                      static_cast<f32>(meshHeightTiles - 2) * pixelsPerTile * 0.5;
                        Vector2f center = {centerX, centerY};
                        Vector2i halflen = {meshWidthTiles * s_pixelsPerTile / 2, meshHeightTiles * s_pixelsPerTile / 2};
                        dstColliders.push_back(SolidCollider(center, halflen));

                        break;
                    } else {
                        endPoint = newPoint;
                        for (Point p : toInsert) {
                            visited.insert(p);
                        }
                    }
                }
                isStarted = false;
            }
        }
    }
}

}  // namespace whal
