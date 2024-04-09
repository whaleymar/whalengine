#version 330

in vec3 fragColor;
in vec2 fragTexCoord;

out vec4 outputColor;

float inborder(vec2 vec) {
    // TODO i think i need to normalize based on entity size. Lines have varying thickness
    float width = 0.05;
    float maxV = 1. - width;
    float minV = width;

    float inX = (1 - step(maxV, vec.x) ) * step(minV, vec.x);
    float inY = (1 - step(maxV, vec.y)) * step(minV, vec.y);
    return 1 - inX * inY;
}

void main() {
    float d = inborder(fragTexCoord);
    outputColor = vec4(fragColor, d);
}
