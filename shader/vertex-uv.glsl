#version 330

uniform mat4 projection;

uniform vec2 offset;
uniform vec2 cameraPos;

in vec3 vert;
in vec2 vertTexCoord;

out vec2 fragTexCoord;

void main() {
    // idk why i need to flip the y coord
    // subtracting 2 from y coord puts y=0 at bottom of screen

    vec2 localOffset = offset - cameraPos;
    vec4 totalOffset = projection * vec4(localOffset.x, -1.*localOffset.y, 0., 0.) + vec4(1., -1., 0., 0.);
    gl_Position = projection * vec4(vert.xyz, 1.) + totalOffset;

    fragTexCoord = vertTexCoord;
}
