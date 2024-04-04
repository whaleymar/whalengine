#version 330

uniform mat4 projection;

uniform vec2 offset;
uniform vec2 halfScreen;

in vec3 vert;
in vec2 vertTexCoord;

out vec2 fragTexCoord;

void main() {
    // idk why i need to flip the y coord
    // subtracting 2 from y coord puts y=0 at bottom of screen
    vec4 totalOffset = projection * vec4(offset.x, -1.*offset.y, 0., 0.) + vec4(0., -2., 0., 0.);
    gl_Position = projection * vec4(vert.xy, 0., 1.) + totalOffset;

    fragTexCoord = vertTexCoord;
}
