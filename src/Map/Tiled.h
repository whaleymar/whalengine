#pragma once

#include <memory>
#include <optional>
#include <vector>

#include "ECS/Components/Transform.h"
#include "Map/ComponentFactory.h"
#include "Map/Level.h"
#include "Util/Expected.h"
#include "Util/Types.h"

namespace whal {

struct Frame;
struct TileMap;
struct Scene;

Expected<Frame> getTileFrame(const TileMap& map, s32 blockIx);
std::optional<Error> parseWorld(const char* mapfile, Scene& dstScene);
Transform getTransformFromMapPosition(Vector2i mapCenterPositionTexels, Vector2i dimensionsTexels, ActiveLevel& level);

struct TileLayer {
    std::string name;
    s32 width;
    s32 height;
    std::unique_ptr<s32[]> data;
};

struct TileSet {
    s32 firstgid;
    s32 tilecount;
    s32 tileWidthTexels;
    s32 tileHeightTexels;
    s32 widthTiles;
    s32 heightTiles;
    s32 margin;
    s32 spacing;
    std::string fileName;
    std::string spriteFileName;
};

struct TileMap {
    static TileMap parse(const char* file, ActiveLevel& level);

    static inline ComponentFactory componentFactory;

    const char* name;
    s32 widthTiles;
    s32 heightTiles;
    s32 tileSize;

    // maybe a vector would be better...
    TileLayer baseLayer;
    TileLayer collisionLayer;
    TileLayer foregroundLayer;
    TileLayer backgroundLayer;
    std::vector<TileSet> tilesets;
};

}  // namespace whal
