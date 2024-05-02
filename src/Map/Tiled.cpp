#include "Tiled.h"

#include <format>
#include <memory>
#include "ECS/Components/Transform.h"
#include "Gfx/GLResourceManager.h"
#include "Gfx/GfxUtil.h"
#include "Map/ComponentFactory.h"
#include "Map/Level.h"
#include "json.hpp"

#include "Gfx/Texture.h"
#include "Util/FileIO.h"
#include "Util/Print.h"

namespace whal {

inline const char* MAP_DIR = "data/map";
inline const char* TSET_SPRITE_DIR = "data/sprite/map";

Expected<TileSet> parseTileset(std::string basename, s32 firstgid);
void parseTileLayer(nlohmann::json layer, TileMap& map);
void parseObjectLayer(nlohmann::json layer, TileMap& map, ActiveLevel& level);

TileMap TileMap::parse(const char* path, ActiveLevel& level) {
    // TODO
    // - visibility?
    // - BETTER ERROR HANDLING (bad bc copy constructor removed so can't put it in expected container)

    TileMap map;
    using json = nlohmann::json;

    Expected<std::string> jString = readFile(std::format("{}/{}", MAP_DIR, path).c_str());
    if (!jString.isExpected()) {
        print("error parsing json:", jString.error());
        return map;
    }

    json data = json::parse(jString.value());

    map.widthTiles = data["width"];
    map.heightTiles = data["height"];
    map.tileSize = data["tilewidth"];

    for (auto& layer : data["layers"]) {
        std::string type = layer["type"];
        if (type == "tilelayer") {
            parseTileLayer(layer, map);
        } else if (type == "objectgroup") {
            parseObjectLayer(layer, map, level);
        } else {
            print("unrecognized layer: ", type, "\nSkipping for now");
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

    bool isNameFound = false;
    for (auto& property : data["properties"]) {
        std::string propName = property["name"];
        if (propName == "Name") {
            std::string mapName = property["value"];
            map.name = mapName.c_str();
            isNameFound = true;
        } else if (propName == "CameraFollowParams") {
            level.cameraFollow = loadFollowComponent(property["value"], level);
        }
    }
    if (!isNameFound) {
        print("A `Name` Property wasn't found in ", path);
        map.name = "Unknown";
    }
    level.name = map.name;

    return map;
}

void parseTileLayer(nlohmann::json layer, TileMap& map) {
    s32 width = layer["width"];
    s32 height = layer["height"];
    const auto& name = layer["name"];
    std::string tname = name;

    std::vector<s32> layerData = layer["data"].get<std::vector<s32>>();
    TileLayer tLayer = {tname, width, height, std::unique_ptr<s32[]>(new s32[width * height]())};
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

void parseObjectLayer(nlohmann::json layer, TileMap& map, ActiveLevel& level) {
    // this will create entities and immediately add them to the level
    auto& ecs = ecs::ECS::getInstance();

    using json = nlohmann::json;
    json& objects = layer["objects"];
    std::unordered_map<s32, s32> idToIndex;
    for (size_t ix = 0; ix < objects.size(); ix++) {
        s32 id = objects[ix]["id"];
        idToIndex.insert({id, ix});
    }

    for (auto& object : objects) {
        std::string objType = object["type"];
        if (objType != "Entity") {
            // check for metadata

            if (objType == "CameraPoint") {
                Vector2i cameraPoint;
                cameraPoint.e[0] = object["x"];
                cameraPoint.e[1] = object["y"];
                level.cameraFocalPoint = getTransformFromMapPosition(cameraPoint, {0, 0}, level).position;
                // print("loaded camerapoint with pos", cameraPoint, "-->", level.cameraFocalPoint);
            }
            continue;
        }

        auto eEntity = ecs.entity();
        if (!eEntity.isExpected()) {
            continue;
        }
        ecs::Entity entity = eEntity.value();
        level.childEntities.insert(entity);

        // top left
        auto positionTexels = Vector2i(object["x"], object["y"]);
        auto dimensionsTexels = Vector2i(object["width"], object["height"]);
        s32 thisId = object["id"];

        Transform trans = getTransformFromMapPosition(positionTexels, dimensionsTexels, level);
        entity.add(trans);

        for (auto& property : object["properties"]) {
            std::string componentName = property["propertytype"];
            ComponentAdder creatorFunc = nullptr;
            map.componentFactory.getEntryIndex(componentName.c_str(), &creatorFunc);
            if (creatorFunc == nullptr) {
                continue;
            }

            creatorFunc(property["value"], objects, idToIndex, thisId, level, entity);
        }
    }
}

Expected<TileSet> parseTileset(std::string basename, s32 firstgid) {
    using json = nlohmann::json;

    Expected<std::string> jString = readFile(std::format("{}/{}", MAP_DIR, basename).c_str());
    if (!jString.isExpected()) {
        return jString.error();
    }

    json data = json::parse(jString.value());

    const auto& name = data["image"];
    std::string sourceFilePath = name;

    s32 firstIx = std::max<s32>(sourceFilePath.find_last_of('/'), sourceFilePath.find_last_of('\\')) + 1;
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

TileSet getTileSet(const TileMap& map, s32 blockIx) {
    for (size_t i = 0; i < map.tilesets.size(); i++) {
        s32 firstgid = map.tilesets[i].firstgid;
        if (firstgid <= blockIx && blockIx < firstgid + map.tilesets[i].tilecount) {
            return map.tilesets[i];
        }
    }
    print("error getting tileset for blockIx", blockIx, "\nReturning first tileset instead");
    return map.tilesets[0];
}

Expected<Frame> getTileFrame(const TileMap& map, s32 blockId) {
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

std::optional<Error> parseMapProject(const char* mapfile) {
    using json = nlohmann::json;

    Expected<std::string> jString = readFile(std::format("{}/{}", MAP_DIR, mapfile).c_str());
    if (!jString.isExpected()) {
        return jString.error();
    }

    json data = json::parse(jString.value());
    for (auto& propType : data["propertyTypes"]) {
        TileMap::componentFactory.makeDefaultComponent(propType);
    }
    return std::nullopt;
}

std::optional<Error> parseWorld(const char* mapfile, Scene& dstScene) {
    using json = nlohmann::json;

    Expected<std::string> jString = readFile(std::format("{}/{}", MAP_DIR, mapfile).c_str());
    if (!jString.isExpected()) {
        return jString.error();
    }

    json data = json::parse(jString.value());

    std::string type = data["type"];
    if (type != "world") {
        return Error("Not a world file");
    }

    dstScene.name = mapfile;
    for (auto& map : data["maps"]) {
        std::string filename = map["fileName"];
        s32 x = map["x"];
        s32 y = map["y"];
        s32 width = map["width"];
        s32 height = map["height"];
        Level lvl = {filename, Vector2f(x, y), Vector2f(width, height)};
        dstScene.allLevels.push_back(lvl);
    }
    // TODO get startPos
    return std::nullopt;
}

// convert top-left coordinate to bottom-middle
Transform getTransformFromMapPosition(Vector2i positionTexels, Vector2i dimensionsTexels, ActiveLevel& level) {
    // add half a tile of height to each point, since they describe the center of an object, but Transform describes the bottom
    positionTexels.e[1] -= static_cast<s32>(TEXELS_PER_TILE) / 2;
    Transform trans = Transform::texels(positionTexels.x() + dimensionsTexels.x() * 0.5 - static_cast<s32>(TEXELS_PER_TILE) / 2,
                                        level.sizeTexels.y() - (positionTexels.y() - dimensionsTexels.y() * 0.5));
    trans.position += level.worldOffsetPixels;
    return trans;
}

}  // namespace whal
