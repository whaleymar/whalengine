#include "GLResourceManager.h"

#include <format>

namespace whal {

std::optional<Error> GLResourceManager::registerProgram(ShaderProgram program, ShaderType shaderType) {
    s32 ix = getProgramIndex(shaderType);
    if (ix >= 0) {
        return Error(std::format("Program with name '{}' already registered", toString(shaderType)));
    }
    mShaders.push_back(program);
    mShaderTypes.push_back(shaderType);
    return std::nullopt;
}

const ShaderProgram GLResourceManager::getProgram(ShaderType shaderType) const {
    return mShaders[getProgramIndex(shaderType)];
}

s32 GLResourceManager::getProgramIndex(ShaderType shaderType) const {
    for (size_t i = 0; i < mShaderTypes.size(); i++) {
        ShaderType sType = mShaderTypes[i];
        if (sType == shaderType) {
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

std::optional<Error> createAndRegisterShader(const char* vertexPath, const char* fragmentPath, ShaderType shaderType, VertexInfo vInfo) {
    Expected<ShaderProgram> program = createShader(vertexPath, fragmentPath, vInfo);
    if (!program.isExpected()) {
        return program.error();
    }
    return GLResourceManager::getInstance().registerProgram(*program, shaderType);
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
