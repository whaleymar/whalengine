#version 330

uniform sampler2D texture1;

in vec2 fragTexCoord;

out vec4 outputColor;

void main() {
    outputColor =vec4(2., 1., 0., 1.); 
    // outputColor = vec4(fragTexCoord.x, fragTexCoord.y, 0., 1.);
}


