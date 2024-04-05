#pragma once

#include <optional>
#include <vector>

#include "Gfx/Shader.h"
#include "Util/Expected.h"

namespace whal {

// singleton to manage shader programs
class GLResourceManager {
public:
    static GLResourceManager& getInstance() {
        static GLResourceManager instance;
        return instance;
    }

    std::optional<Error> registerProgram(ShaderProgram program, const char* name);
    ShaderProgram getProgram(const char* name);

private:
    GLResourceManager() = default;

    GLResourceManager(const GLResourceManager&) = delete;
    void operator=(const GLResourceManager&) = delete;

    s32 getProgramIndex(std::string name) const;

    std::vector<ShaderProgram> mShaders;
    std::vector<std::string> mShaderNames;
};

std::optional<Error> createAndRegisterShader(const char* vertexPath, const char* fragmentPath, const char* shaderName);

}  // namespace whal
