#include "Gfx/VertexObject.h"

#include "glad/gl.h"

namespace whal {

Vao::Vao() {
    glGenVertexArrays(1, &mHandle);
}

void Vao::bind() const {
    glBindVertexArray(mHandle);
}

void Vao::free() {
    glDeleteVertexArrays(1, &mHandle);
}

Vbo::Vbo() {
    glGenBuffers(1, &mHandle);
}

void Vbo::bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, mHandle);
}

void Vbo::buffer(f32* vertices, u32 sizeBytes) const {
    bind();
    glBufferData(GL_ARRAY_BUFFER, sizeBytes, vertices, GL_STATIC_DRAW);
}

void Vbo::free() {
    glDeleteBuffers(1, &mHandle);
}

f32 depthToFloat(Depth depth) {
    switch (depth) {
    case Depth::BackgroundFar:
        return -0.9;
    case Depth::BackgroundMid:
        return -0.8;
    case Depth::BackgroundNear:
        return -0.7;
    case Depth::BackgroundNoParallax:
        return -0.1;
    case Depth::Player:
        return 0.0;
    case Depth::Foreground:
        return 0.1;
    case Depth::Debug:
        return 0.9;
    }
}

VertArrayRect MakeRectVertices(f32 pixelWidth, f32 pixelHeight, f32 minU, f32 maxU, f32 minV, f32 maxV) {
    // TODO depth arg
    // clang-format off
	return {
		pixelWidth, pixelHeight, 0.0, maxU, minV,// top right
		0.0, pixelHeight, 0.0, minU, minV,       // top left
		pixelWidth, 0.0, 0.0, maxU, maxV,        // bottom right
		0.0, 0.0, 0.0, minU, maxV,               // bottom left
	};
    // clang-format on
}

}  // namespace whal
