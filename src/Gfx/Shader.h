#pragma once

#include <optional>

#include "Gfx/IUseOpenGLHandle.h"
#include "Util/Expected.h"

namespace whal {

class ShaderProgram;

enum class VertexInfo { XYZ, UV, RGB, RGBUV };

Expected<ShaderProgram> createShader(const char* vertexPath, const char* fragmentPath, VertexInfo vInfo);

class ShaderProgram : public IUseOpenGLHandle {
public:
    ShaderProgram(VertexInfo vInfo = VertexInfo::XYZ);
    void useProgram() const;
    void init();
    void onWindowResize();

    const u32 strideSize;
    const u32 uvOffset;
    const u32 rgbOffset;
    u32 drawOffsetUniform;
    u32 cameraPositionUniform;
    u32 sizeUniform;
    u32 textureUniform;
    u32 rgbUniform;
    u32 transformUniform;
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
