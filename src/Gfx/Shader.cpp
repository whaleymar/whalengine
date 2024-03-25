#include "Gfx/Shader.h"

#include <format>

#include <glad/gl.h>

#include "Util/FileIO.h"

#define VERTEX_SHADER_PATH "shader/vertex.glsl"
#define FRAG_SHADER_PATH "shader/fragment.glsl"

namespace whal {

static constexpr int LOG_LENGTH = 512;

Expected<ShaderProgram> InitShaders() {
    Shader vertexShader(true);
    Shader fragmentShader(false);

    std::optional<Error> error = vertexShader.compile(VERTEX_SHADER_PATH);
    if (error) {
        return Expected<ShaderProgram>(*error);
    }

    error = fragmentShader.compile(FRAG_SHADER_PATH);
    if (error) {
        return Expected<ShaderProgram>(*error);
    }

    ShaderProgram program;
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

    return program;
}

ShaderProgram::ShaderProgram() {
    mHandle = glCreateProgram();

    drawOffsetUniform = glGetUniformLocation(mHandle, "offset");
    textureUniform = glGetUniformLocation(mHandle, "tex");
    millis = glGetUniformLocation(mHandle, "millis");
}

void ShaderProgram::useProgram() const {
    glUseProgram(mHandle);
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
    glVertexAttribPointer(vertAttrib, 3, GL_FLOAT, GL_FALSE, STRIDE_SIZE * sizeof(float), (void*)0);

    u32 texCoordAttrib = glGetAttribLocation(program.handle(), "vertTexCoord");
    glEnableVertexAttribArray(texCoordAttrib);
    // last arg is byte offset to first vertex's UV data
    glVertexAttribPointer(texCoordAttrib, 2, GL_FLOAT, GL_FALSE, STRIDE_SIZE * sizeof(float), (void*)(3 * sizeof(float)));
}

}  // namespace whal
