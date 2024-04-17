#pragma once

#include <optional>
#include <vector>
#include "Gfx/GfxUtil.h"
#include "Gfx/Shader.h"
#include "Gfx/Texture.h"
#include "Util/Expected.h"

namespace whal {

// singleton to manage shader programs
class GLResourceManager {
public:
    static GLResourceManager& getInstance() {
        static GLResourceManager instance;
        return instance;
    }

    std::optional<Error> registerProgram(ShaderProgram program, ShaderType shaderType);
    const ShaderProgram getProgram(ShaderType shaderType) const;

    std::optional<Error> registerTexture(Texture texture, const char* name);
    const Texture& getTexture(const char* name);

private:
    GLResourceManager() = default;

    GLResourceManager(const GLResourceManager&) = delete;
    void operator=(const GLResourceManager&) = delete;

    s32 getProgramIndex(ShaderType shaderType) const;
    s32 getTextureIndex(std::string name) const;

    std::vector<ShaderProgram> mShaders;
    std::vector<ShaderType> mShaderTypes;

    std::vector<Texture> mTextures;
    std::vector<std::string> mTextureNames;
};

std::optional<Error> createAndRegisterShader(const char* vertexPath, const char* fragmentPath, ShaderType shaderType, VertexInfo vInfo);
std::optional<Error> createAndRegisterTexture(const char* atlasPath, const char* textureName);

}  // namespace whal
