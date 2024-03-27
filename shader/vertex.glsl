#version 330

uniform mat4 projection;

uniform vec2 offset;

in vec3 vert;
in vec2 vertTexCoord;

out vec2 fragTexCoord;

void main() {
    vec4 totalOffset = projection * vec4(offset.x, offset.y, 0., 0.) + vec4(1., -1., 0., 0.);
    gl_Position = projection * vec4(vert.xy, 0., 1.) + totalOffset;

    fragTexCoord = vertTexCoord;
}
