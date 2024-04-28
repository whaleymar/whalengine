#version 330

in vec3 fragColor;
in vec2 fragTexCoord;
in vec2 fragObjSize;

out vec4 outputColor;

float inborder(vec2 vec) {
    float widthX = 1. / fragObjSize.x;
    float widthY = 1. / fragObjSize.y;
    float maxVX = 1. - widthX;
    float maxVY = 1. - widthY;
    float minVX = widthX;
    float minVY = widthY;

    float inX = (1 - step(maxVX, vec.x) ) * step(minVX, vec.x);
    float inY = (1 - step(maxVY, vec.y)) * step(minVY, vec.y);
    return 1 - inX * inY;
}

void main() {
    float d = inborder(fragTexCoord);
    outputColor = vec4(fragColor, d);
}
