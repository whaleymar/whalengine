#include "Tiled.h"

#include <format>
#include <memory>
#include "Gfx/GLResourceManager.h"
#include "Gfx/GfxUtil.h"
#include "json.hpp"

#include "Gfx/Texture.h"
#include "Util/FileIO.h"
#include "Util/Print.h"

namespace whal {

inline const char* TSET_DIR = "data/map";
inline const char* TSET_SPRITE_DIR = "data/sprite/map";

Expected<TileSet> parseTileset(std::string basename, s32 firstgid);

TiledMap TiledMap::parse(const char* path) {
    // TODO
    // - layer type (tilelayer, object)
    // - visibility?
    // -
    TiledMap map;
    using json = nlohmann::json;

    Expected<std::string> jString = readFile(path);
    if (!jString.isExpected()) {
        print("error parsing json");
        return map;
    }

    json data = json::parse(jString.value());

    map.width = data["width"];
    map.height = data["height"];
    map.tileSize = data["tilewidth"];

    for (auto& layer : data["layers"]) {
        s32 width = layer["width"];
        s32 height = layer["height"];
        const auto& name = layer["name"];
        std::string tname = name;

        std::vector<s32> layerData = layer["data"].get<std::vector<s32>>();
        TiledLayer tLayer = {tname, width, height, std::unique_ptr<s32[]>(new s32[width * height]())};
        memcpy(tLayer.data.get(), layerData.data(), layerData.size() * sizeof(s32));

        if (tname == "Collision") {
            map.collisionLayer = std::move(tLayer);
        } else if (tname == "Base") {
            map.baseLayer = std::move(tLayer);
        } else if (tname == "Foreground") {
            map.foregroundLayer = std::move(tLayer);
        } else if (tname == "Background") {
            map.backgroundLayer = std::move(tLayer);
        } else {
            print("unrecognized layer name: ", tname);
        }
    }

    for (auto& tileset : data["tilesets"]) {
        s32 firstgid = tileset["firstgid"];
        std::string fileName = tileset["source"];

        Expected<TileSet> eTset = parseTileset(fileName, firstgid);
        if (!eTset.isExpected()) {
            print(eTset.error());
            continue;
        }
        TileSet tset = eTset.value();
        map.tilesets.push_back(tset);
    }

    return map;
}

Expected<TileSet> parseTileset(std::string basename, s32 firstgid) {
    using json = nlohmann::json;

    Expected<std::string> jString = readFile(std::format("{}/{}", TSET_DIR, basename).c_str());
    if (!jString.isExpected()) {
        return jString.error();
    }

    json data = json::parse(jString.value());

    const auto& name = data["image"];
    std::string sourceFilePath = name;
    s32 firstIx = std::max<s32>(sourceFilePath.find("/"), sourceFilePath.find("\\")) + 1;
    s32 lastIx = sourceFilePath.find(".", firstIx);
    std::string sourceFileBasenameNoExt = sourceFilePath.substr(firstIx, lastIx - firstIx);

    s32 tileWidth = data["tilewidth"];
    s32 tileHeight = data["tileheight"];
    s32 widthTexels = data["imagewidth"];
    s32 heightTexels = data["imageheight"];

    s32 widthTiles = widthTexels / tileWidth;
    s32 heightTiles = heightTexels / tileHeight;

    return TileSet(firstgid, data["tilecount"], tileWidth, tileHeight, widthTiles, heightTiles, data["margin"], data["spacing"], basename,
                   sourceFileBasenameNoExt);
}

TileSet getTileSet(const TiledMap& map, s32 blockIx) {
    for (size_t i = 0; i < map.tilesets.size(); i++) {
        s32 firstgid = map.tilesets[i].firstgid;
        if (firstgid <= blockIx && blockIx < firstgid + map.tilesets[i].tilecount) {
            return map.tilesets[i];
        }
    }
    print("error getting tileset for blockIx", blockIx, "\nReturning first tileset instead");
    return map.tilesets[0];
}

Expected<Frame> getTileFrame(const TiledMap& map, s32 blockId) {
    TileSet tset = getTileSet(map, blockId);
    std::string spritePath = std::format("{}/{}", "map", tset.spriteFileName);
    std::optional<Frame> tsetFrameOpt = GLResourceManager::getInstance().getTexture(TEXNAME_SPRITE).getFrame(spritePath.c_str());

    if (!tsetFrameOpt) {
        return Error(std::format("Couldn't find {} in sprite table", spritePath));
    }

    // ASSUMING 0 MARGIN && SPACING

    s32 blockIx = blockId - 1;  // id is 1-indexed

    s32 rowIx = blockIx / tset.heightTiles;
    s32 colIx = blockIx % tset.widthTiles;

    Frame fullFrame = tsetFrameOpt.value();
    Frame newFrame = {
        {fullFrame.atlasPositionTexels.x() + colIx * tset.tileWidthTexels, fullFrame.atlasPositionTexels.y() + rowIx * tset.tileHeightTexels},
        {tset.tileWidthTexels, tset.tileHeightTexels}};
    return newFrame;
}

}  // namespace whal
