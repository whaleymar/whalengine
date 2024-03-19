#include "Gfx/VertexObject.h"
#include "glad/gl.h"

namespace whal {

Vao::Vao() {
    glGenVertexArrays(1, &mHandle);
}

Vao::~Vao() {
    glDeleteVertexArrays(1, &mHandle);
}

void Vao::bind() const {
    glBindVertexArray(mHandle);
}

Vbo::Vbo() {
    glGenBuffers(1, &mHandle);
}

Vbo::~Vbo() {
    glDeleteBuffers(1, &mHandle);
}

void Vbo::bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, mHandle);
}

void Vbo::buffer(f32* vertices, u32 sizeBytes) const {
    bind();
    glBufferData(GL_ARRAY_BUFFER, sizeBytes, vertices, GL_STATIC_DRAW);
}

std::array<f32, 30> MakeRectVertices(f32 pixelWidth, f32 pixelHeight) {
    // clang-format off
	return {
		0.0, pixelHeight, 0.0, 0.0, 0.0,
		pixelWidth, pixelHeight, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 0.0, 1.0,
		pixelWidth, pixelHeight, 0.0, 1.0, 0.0,
		pixelWidth, 0.0, 0.0, 1.0, 1.0,
		0.0, 0.0, 0.0, 0.0, 1.0,
	};
    // clang-format on
}

}  // namespace whal
