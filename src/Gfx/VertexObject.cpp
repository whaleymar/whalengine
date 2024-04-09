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

constexpr f32 depthToFloat(Depth depth) {
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

VertArrayRect MakeRectVertices(f32 pixelWidth, f32 pixelHeight, Depth depth) {
    f32 fdepth = depthToFloat(depth);
    // clang-format off
	return {
		pixelWidth, pixelHeight, fdepth,    // top right
		0.0,        pixelHeight, fdepth,    // top left
		pixelWidth, 0.0,         fdepth,    // bottom right
		0.0,        0.0,         fdepth,    // bottom left
	};
    // clang-format on
}

VertArrayRectUV MakeRectVerticesUV(f32 pixelWidth, f32 pixelHeight, Depth depth, f32 minU, f32 maxU, f32 minV, f32 maxV) {
    f32 fdepth = depthToFloat(depth);
    // clang-format off
	return {
		pixelWidth, pixelHeight, fdepth, maxU, minV,    // top right
		0.0,        pixelHeight, fdepth, minU, minV,    // top left
		pixelWidth, 0.0,         fdepth, maxU, maxV,    // bottom right
		0.0,        0.0,         fdepth, minU, maxV,    // bottom left
	};
    // clang-format on
}

VertArrayRectRGB MakeRectVerticesRGB(f32 pixelWidth, f32 pixelHeight, Depth depth, const RGB rgb) {
    f32 fdepth = depthToFloat(depth);
    // clang-format off
	return {
		pixelWidth, pixelHeight, fdepth, rgb.r, rgb.g, rgb.b,    // top right
		0.0,        pixelHeight, fdepth, rgb.r, rgb.g, rgb.b,    // top left
		pixelWidth, 0.0,         fdepth, rgb.r, rgb.g, rgb.b,    // bottom right
		0.0,        0.0,         fdepth, rgb.r, rgb.g, rgb.b,    // bottom left
	};
    // clang-format on
}

VertArrayRectRGBUV MakeRectVerticesRGBUV(f32 pixelWidth, f32 pixelHeight, Depth depth, const RGB rgb, f32 minU, f32 maxU, f32 minV, f32 maxV) {
    f32 fdepth = depthToFloat(depth);
    // clang-format off
	return {
		pixelWidth, pixelHeight, fdepth, rgb.r, rgb.g, rgb.b, maxU, minV,    // top right
		0.0,        pixelHeight, fdepth, rgb.r, rgb.g, rgb.b, minU, minV,    // top left
		pixelWidth, 0.0,         fdepth, rgb.r, rgb.g, rgb.b, maxU, maxV,    // bottom right
		0.0,        0.0,         fdepth, rgb.r, rgb.g, rgb.b, minU, maxV,    // bottom left
	};
    // clang-format on
}

}  // namespace whal
