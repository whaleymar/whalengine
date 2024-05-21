#include "Gfx/Shader.h"

#include <format>

#include <glad/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Gfx/GfxUtil.h"
#include "Gfx/VertexObject.h"
#include "Util/FileIO.h"

namespace whal {

inline constexpr int LOG_LENGTH = 512;

constexpr u32 getStride(VertexInfo vInfo) {
    switch (vInfo) {
    case VertexInfo::XYZ:
        return STRIDE_SIZE_XYZ;
    case VertexInfo::RGB:
        return STRIDE_SIZE_RGB;
    case VertexInfo::UV:
        return STRIDE_SIZE_UV;
    case VertexInfo::RGBUV:
        return STRIDE_SIZE_RGBUV;
    }
}

constexpr u32 getUVOffset(VertexInfo vInfo) {
    // 0 == no UV
    switch (vInfo) {
    case VertexInfo::XYZ:
        return 0;
    case VertexInfo::RGB:
        return 0;
    case VertexInfo::UV:
        return 3;
    case VertexInfo::RGBUV:
        return 6;
    }
}

constexpr u32 getRGBOffset(VertexInfo vInfo) {
    // 0 == no RGB
    switch (vInfo) {
    case VertexInfo::RGB:
        return 3;
    case VertexInfo::RGBUV:
        return 3;
    default:
        return 0;
    }
}

Expected<ShaderProgram> createShader(const char* vertexPath, const char* fragmentPath, VertexInfo vInfo) {
    Shader vertexShader(true);
    Shader fragmentShader(false);

    std::optional<Error> error = vertexShader.compile(vertexPath);
    if (error) {
        return Expected<ShaderProgram>(*error);
    }

    error = fragmentShader.compile(fragmentPath);
    if (error) {
        return Expected<ShaderProgram>(*error);
    }

    ShaderProgram program(vInfo);
    program.useProgram();
    u32 handle = program.handle();
    glAttachShader(handle, vertexShader.handle());
    glAttachShader(handle, fragmentShader.handle());
    glLinkProgram(handle);

    s32 success;
    glGetProgramiv(handle, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[LOG_LENGTH];
        glGetProgramInfoLog(handle, sizeof(infoLog), NULL, infoLog);
        return Expected<ShaderProgram>::error(std::format("Got Linker Error: {}", infoLog));
    }

    program.init();
    program.onWindowResize();

    return program;
}

ShaderProgram::ShaderProgram(VertexInfo vInfo_) : strideSize(getStride(vInfo_)), uvOffset(getUVOffset(vInfo_)), rgbOffset(getRGBOffset(vInfo_)) {
    mHandle = glCreateProgram();
}

void ShaderProgram::useProgram() const {
    glUseProgram(mHandle);
}

void ShaderProgram::init() {
    useProgram();

    drawOffsetUniform = glGetUniformLocation(mHandle, "offset");
    transformUniform = glGetUniformLocation(mHandle, "transform");
    cameraPositionUniform = glGetUniformLocation(mHandle, "cameraPos");

    if (uvOffset > 0) {
        textureUniform = glGetUniformLocation(mHandle, "tex");
        sizeUniform = glGetUniformLocation(mHandle, "objSize");
    }

    // millis = glGetUniformLocation(mHandle, "millis");
}

void ShaderProgram::onWindowResize() {
    // Adjust camera projection
    useProgram();

    f32 fWinWidth = static_cast<f32>(WINDOW_WIDTH_PIXELS);
    f32 fWinHeight = static_cast<f32>(WINDOW_HEIGHT_PIXELS);

    glm::mat4 projection = glm::ortho(0.0f, fWinWidth, fWinHeight, 0.0f, -1.0f, 1.0f);
    projectionUniform = glGetUniformLocation(mHandle, "projection");
    glUniformMatrix4fv(projectionUniform, 1, false, glm::value_ptr(projection));
}

Shader::Shader(bool isVertexShader) {
    auto shaderType = isVertexShader ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER;
    mHandle = glCreateShader(shaderType);
}

Shader::~Shader() {
    glDeleteShader(mHandle);
}

std::optional<Error> Shader::compile(const char* sourcePath) const {
    auto result = readFile(sourcePath);
    if (!result.isExpected()) {
        return result.error();
    }
    const char* source = result.value().c_str();

    glShaderSource(mHandle, 1, &source, NULL);
    glCompileShader(mHandle);

    s32 success;
    glGetShaderiv(mHandle, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[LOG_LENGTH];
        glGetShaderInfoLog(mHandle, sizeof(infoLog), NULL, infoLog);
        return Error(infoLog);
    }
    return std::nullopt;
}

void updateShaderVars(ShaderProgram program) {
    // only affects the *current* vao bound to glArrayBuffer

    glBindFragDataLocation(program.handle(), 0, "outputColor");
    u32 vertAttrib = glGetAttribLocation(program.handle(), "vert");
    glEnableVertexAttribArray(vertAttrib);
    glVertexAttribPointer(vertAttrib, 3, GL_FLOAT, GL_FALSE, program.strideSize * sizeof(float), (void*)0);

    if (program.uvOffset > 0) {
        u32 texCoordAttrib = glGetAttribLocation(program.handle(), "vertTexCoord");
        glEnableVertexAttribArray(texCoordAttrib);
        // last arg is byte offset to first vertex's UV data
        glVertexAttribPointer(texCoordAttrib, 2, GL_FLOAT, GL_FALSE, program.strideSize * sizeof(float), (void*)(program.uvOffset * sizeof(float)));
    }

    if (program.rgbOffset > 0) {
        u32 rgbAttrib = glGetAttribLocation(program.handle(), "vertColor");
        glEnableVertexAttribArray(rgbAttrib);
        // last arg is byte offset to first vertex's UV data
        glVertexAttribPointer(rgbAttrib, 3, GL_FLOAT, GL_FALSE, program.strideSize * sizeof(float), (void*)(program.rgbOffset * sizeof(float)));
    }
}

}  // namespace whal
