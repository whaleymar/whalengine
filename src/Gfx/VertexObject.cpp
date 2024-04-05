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

VertArrayRect MakeRectVertices(f32 pixelWidth, f32 pixelHeight) {
    // TODO depth arg
    // clang-format off
	return {
		pixelWidth, pixelHeight, 0.0, 1.0, 0.0,// top right
		0.0, pixelHeight, 0.0, 0.0, 0.0,       // top left
		pixelWidth, 0.0, 0.0, 1.0, 1.0,        // bottom right
		0.0, 0.0, 0.0, 0.0, 1.0,               // bottom left
	};
    // clang-format on
}

VertArrayRect MakeLineVertices(Vector2f pixelPosStart, Vector2f pixelPosEnd, f32 thickness) {
    // Vector2f delta = pixelPosStart - pixelPosEnd;
    // f32 norm = 1/delta.len();
    // f32 dx = delta.x() * norm;
    // f32 dy = delta.y() * norm;
    // f32 dx = delta.x();
    // f32 dy = delta.y();
    //
    // dx *= (thickness / 2.0);
    // dy *= (thickness / 2.0);
    //
    // f32 x1 = pixelPosStart.x() + dx + dy;
    // f32 y1 = pixelPosStart.y() + dy - dx;
    // f32 x2 = pixelPosStart.x() + dx - dy;
    // f32 y2 = pixelPosStart.y() + dy + dx;
    //
    // f32 x3 = pixelPosStart.x() - dx + dy;
    // f32 y3 = pixelPosStart.y() - dy - dx;
    // f32 x4 = pixelPosStart.x() - dx - dy;
    // f32 y4 = pixelPosStart.y() - dy + dx;
    //
    f32 depth = depthToFloat(Depth::Debug);

    // // clang-format off
    // return {
    //     x2, y2, depth, 0.0, 0.0,
    //     x4, y4, depth, 0.0, 0.0,
    //     x1, y1, depth, 0.0, 0.0,
    //     x3, y3, depth, 0.0, 0.0
    // };
    // // clang-format on

    pixelPosStart += Vector::unitfLeft;

    // clang-format off
    return {
        pixelPosEnd.x() + thickness, pixelPosEnd.y() , depth, 0.0, 0.0, // TR
        pixelPosStart.x(), pixelPosEnd.y(), depth, 0.0, 0.0, //TL
        pixelPosEnd.x() + thickness, pixelPosStart.y(), depth, 0.0, 0.0, // BR
        pixelPosStart.x(), pixelPosStart.y(), depth, 0.0, 0.0  // BL
    };
    // clang-format on
}

}  // namespace whal
