#include <iostream>
#include "Util/MathUtil.h"
#include "Util/Vector.h"

using namespace whal;

int main(int argc, char* argv[]) {
    std::cout << "running..." << std::endl;

    Vector2f vec1(5.0, 5.0);
    Vector2f vec2(-5.0, -5.0);
    // f32 dot = vec1.dot(vec2);
    f32 out = abs(vec1.dot(vec2));
    // f32 out = vec1.dot(vec2);
    // f32 out = vec1.len();
    // f32 out = abs(-1 * Vector::zero2f.len());
    std::cout << out << std::endl;

    return 0;
}
