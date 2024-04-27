#include "Tiled.h"

#include <format>
#include <memory>
#include "json.hpp"

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
    s32 lastIx = std::max<s32>(sourceFilePath.find("/"), sourceFilePath.find("\\"));
    return TileSet(firstgid, data["tilecount"], basename, sourceFilePath.substr(lastIx + 1));
}

}  // namespace whal
