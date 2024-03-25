#pragma once

#include <optional>

#include "Gfx/IUseOpenGLHandle.h"
#include "Util/Expected.h"

#define STRIDE_SIZE 5

namespace whal {

class ShaderProgram;

Expected<ShaderProgram> InitShaders();

class ShaderProgram : public IUseOpenGLHandle {
public:
    ShaderProgram();
    void useProgram() const;

    u32 drawOffsetUniform;
    u32 textureUniform;
    u32 millis;
};

class Shader : public IUseOpenGLHandle {
public:
    Shader(bool isVertexShader);
    ~Shader();

    std::optional<Error> compile(const char* sourcePath) const;
};

void updateShaderVars(ShaderProgram program);

}  // namespace whal
