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

ShaderProgram GLResourceManager::getProgram(const char* name) {
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

std::optional<Error> createAndRegisterShader(const char* vertexPath, const char* fragmentPath, const char* shaderName) {
    Expected<ShaderProgram> program = createShader(vertexPath, fragmentPath);
    if (!program.isExpected()) {
        return program.error();
    }
    return GLResourceManager::getInstance().registerProgram(*program, shaderName);
}

}  // namespace whal
