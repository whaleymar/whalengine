#pragma once

#include <optional>

#include "Gfx/IUseOpenGLHandle.h"
#include "Util/Expected.h"

namespace whal {

class ShaderProgram;

Expected<ShaderProgram> InitShaders();

class ShaderProgram : public IUseOpenGLHandle {
public:
    ShaderProgram();
    void useProgram() const;
};

class Shader : public IUseOpenGLHandle {
public:
    Shader(bool isVertexShader);
    ~Shader();

    std::optional<Error> compile(const char* sourcePath) const;
};

}  // namespace whal
