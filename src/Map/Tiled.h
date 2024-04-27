#pragma once

#include <memory>
#include <vector>
#include "Util/Expected.h"
#include "Util/Types.h"

namespace whal {

struct Frame;
struct TiledMap;

Expected<Frame> getTileFrame(const TiledMap& map, s32 blockIx);

struct TiledLayer {
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

struct TiledMap {
    // static Expected<TiledMap> parse(const char* file);
    static TiledMap parse(const char* file);

    s32 width;
    s32 height;
    s32 tileSize;

    // maybe a vector would be better...
    TiledLayer baseLayer;
    TiledLayer collisionLayer;
    TiledLayer foregroundLayer;
    TiledLayer backgroundLayer;
    std::vector<TileSet> tilesets;
};

}  // namespace whal
