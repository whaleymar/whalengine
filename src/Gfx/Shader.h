#pragma once

#include <optional>

#include "Gfx/IUseOpenGLHandle.h"
#include "Util/Expected.h"

namespace whal {

class ShaderProgram;

Expected<ShaderProgram> createShader(const char* vertexPath, const char* fragmentPath);

class ShaderProgram : public IUseOpenGLHandle {
public:
    ShaderProgram();
    void useProgram() const;
    void init();
    void onWindowResize();

    u32 drawOffsetUniform;
    u32 textureUniform;
    u32 millis;

    // dependent on screen size:
    u32 projectionUniform;
};

class Shader : public IUseOpenGLHandle {
public:
    Shader(bool isVertexShader);
    ~Shader();

    std::optional<Error> compile(const char* sourcePath) const;
};

void updateShaderVars(ShaderProgram program);

}  // namespace whal
