#include "Gfx/Texture.h"

#include <GL/gl.h>

#include <cstdlib>
#include <cstring>
#include <format>
#include <string>

#include "Gfx/GfxUtil.h"
#include "Util/FileIO.h"
#include "Util/Print.h"

#define RAPIDXML_NO_EXCEPTIONS
#include "RapidXML/rapidxml.hpp"

namespace rapidxml {

void parse_error_handler(const char* what, void* where) {
    whal::print("Got XML parsing error: ", what);
    std::abort();
}

}  // namespace rapidxml

namespace whal {

static int TEXTURE_ID = GL_TEXTURE0;

Texture::Texture() {
    glGenTextures(1, &mHandle);
    mTextureID = TEXTURE_ID++;
}

std::optional<Error> Texture::loadAtlas(const char* atlasPath) {
    if (!atlasPath) {
        return Error("Cannot load empty file name");
    }
    if (!isExist(atlasPath)) {
        return Error(std::format("%s does not exist", atlasPath));
    }

    Image img(atlasPath);
    mAtlas = TextureAtlas(img);
    if (!mAtlas->isValid()) {
        return Error("Invalid atlas");
    }

    bind();

    // GL_NEAREST for pixelated look (no interp)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // repeat at end of texture https://registry.khronos.org/OpenGL-Refpages/gl4/html/glTexParameter.xhtml
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(), img.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.getData());

    mIsValid = true;

    return std::nullopt;
}

Vector2f Texture::getSize() const {
    return mAtlas->getSize();
}

void Texture::bind() const {
    glActiveTexture(mTextureID);
    glBindTexture(GL_TEXTURE_2D, mHandle);
}
bool Texture::isValid() const {
    return mAtlas != std::nullopt;
}

std::optional<Frame> Texture::getFrame(const char* name) const {
    return mAtlas->getFrame(name);
}

TextureAtlas::TextureAtlas(Image& img) {
    mSize = {static_cast<f32>(img.width()), static_cast<f32>(img.height())};
    auto err = init();
    if (err) {
        print("Got error initializing texture atlas: ", *err);
    } else {
        mIsValid = true;
    }
}

std::optional<Error> TextureAtlas::init() {
    Expected<std::string> content = readFile(ATLAS_METADATA_PATH);
    if (!content.isExpected()) {
        return content.error();
    }

    using namespace rapidxml;

    xml_document<> doc;
    std::string xmldata = content.value();

    doc.parse<parse_no_entity_translation>(&xmldata[0]);
    xml_node<>* atlasNode = doc.first_node("atlas");
    if (!atlasNode) {
        return Error("Could not find 'atlas' root node");
    }
    xml_node<>* trimNode = atlasNode->first_node("trim");
    if (!trimNode) {
        return Error("Could not find 'trim' node");
    }
    mIsTrimEnabled = strcmp(trimNode->value(), "true") == 0;

    xml_node<>* rotateNode = atlasNode->first_node("rotate");
    if (!rotateNode) {
        return Error("Could not find 'rotate' node");
    }
    mIsRotateEnabled = strcmp(rotateNode->value(), "true") == 0;

    xml_node<>* texNode = atlasNode->first_node("tex");
    if (!texNode) {
        return Error("Could not find 'tex' node");
    }

    for (xml_node<>* spriteNode = texNode->first_node("img"); spriteNode; spriteNode = spriteNode->next_sibling("img")) {
        const char* name = spriteNode->first_attribute("n")->value();
        Vector2i atlasPosition = {std::stoi(spriteNode->first_attribute("x")->value()), std::stoi(spriteNode->first_attribute("y")->value())};
        Vector2i dimensions = {std::stoi(spriteNode->first_attribute("w")->value()), std::stoi(spriteNode->first_attribute("h")->value())};

        // ignoring trim and rotate unless i need them
        Frame frame = {atlasPosition, dimensions};
        mTable.insert({name, frame});
    }

    return std::nullopt;
}

Vector2f TextureAtlas::getSize() const {
    return mSize;
}

std::optional<Frame> TextureAtlas::getFrame(const char* name) const {
    auto search = mTable.find(name);
    if (search == mTable.end()) {
        return std::nullopt;
    }
    return search->second;
}

}  // namespace whal
