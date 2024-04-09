#include "GLResourceManager.h"

#include <format>

namespace whal {

std::optional<Error> GLResourceManager::registerProgram(ShaderProgram program, const char* name) {
    s32 ix = getProgramIndex(name);
    if (ix >= 0) {
        return Error(std::format("Program with name '{}' already registered", name));
    }
    mShaders.push_back(program);
    mShaderNames.push_back(name);
    return std::nullopt;
}

const ShaderProgram GLResourceManager::getProgram(const char* name) const {
    return mShaders[getProgramIndex(name)];
}

s32 GLResourceManager::getProgramIndex(std::string name) const {
    for (size_t i = 0; i < mShaderNames.size(); i++) {
        std::string shaderName = mShaderNames[i];
        if (shaderName == name) {
            return static_cast<s32>(i);
        }
    }
    return -1;
}

std::optional<Error> GLResourceManager::registerTexture(Texture texture, const char* name) {
    s32 ix = getTextureIndex(name);
    if (ix >= 0) {
        return Error(std::format("Texture with name '{}' already registered", name));
    }
    mTextures.push_back(std::move(texture));
    mTextureNames.push_back(name);
    return std::nullopt;
}

const Texture& GLResourceManager::getTexture(const char* name) {
    return mTextures[getTextureIndex(name)];
}

s32 GLResourceManager::getTextureIndex(std::string name) const {
    for (size_t i = 0; i < mTextureNames.size(); i++) {
        std::string texName = mTextureNames[i];
        if (texName == name) {
            return static_cast<s32>(i);
        }
    }
    return -1;
}

std::optional<Error> createAndRegisterShader(const char* vertexPath, const char* fragmentPath, const char* shaderName, VertexInfo vInfo) {
    Expected<ShaderProgram> program = createShader(vertexPath, fragmentPath, vInfo);
    if (!program.isExpected()) {
        return program.error();
    }
    return GLResourceManager::getInstance().registerProgram(*program, shaderName);
}

std::optional<Error> createAndRegisterTexture(const char* atlasPath, const char* textureName) {
    Texture tex;
    auto err = tex.loadAtlas(atlasPath);
    if (err) {
        return err;
    }
    return GLResourceManager::getInstance().registerTexture(std::move(tex), textureName);
}

}  // namespace whal
