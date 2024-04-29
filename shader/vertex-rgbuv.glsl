#version 330

uniform mat4 projection;

uniform vec2 offset;
uniform vec2 cameraPos;
uniform vec2 objSize;

in vec3 vert;
in vec3 vertColor;
in vec2 vertTexCoord;

out vec3 fragColor;
out vec2 fragTexCoord;
out vec2 fragObjSize;

void main() {
    // idk why i need to flip the y coord
    // subtracting 1 (-2 + 1 for camera) from y coord puts y=0 at bottom of screen
    // adding 1 to x coord centers the camera

    vec2 localOffset = offset - cameraPos;
    vec4 totalOffset = projection * vec4(localOffset.x, -1.*localOffset.y, 0., 0.) + vec4(1., -1., 0., 0.);
    gl_Position = projection * vec4(vert.xy, 0., 1.) + totalOffset;

    fragColor = vertColor;
    fragTexCoord = vertTexCoord;
    fragObjSize = objSize;
}


