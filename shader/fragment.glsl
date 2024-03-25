#version 330

uniform sampler2D texture1;

in vec2 fragTexCoord;

out vec4 outputColor;

float invert(float x) {
    return 1-x;
}

float norm(float x) {
    return (x + 1.)/2.;
}

vec2 norm(vec2 vec) {
    return vec2(norm(vec.x), norm(vec.y));
}

void main() {
    outputColor =vec4(1.0, 0.5, 0.2, 1.); 
    // outputColor = texture(texture1, fragTexCoord);
}

