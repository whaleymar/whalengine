#version 330

uniform sampler2D texture1;

in vec2 fragTexCoord;

out vec4 outputColor;

void main() {
    outputColor = texture(texture1, fragTexCoord);
}

