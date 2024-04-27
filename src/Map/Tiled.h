#pragma once

#include <memory>
#include <vector>
#include "Util/Types.h"

namespace whal {

struct TiledLayer {
    std::string name;
    s32 width;
    s32 height;
    std::unique_ptr<s32[]> data;
};

struct TileSet {
    s32 firstgid;
    s32 tilecount;
    std::string fileName;
    std::string sourceFileName;
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
