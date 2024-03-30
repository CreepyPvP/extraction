#version 440

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNorm;
layout(location = 2) in vec3 aColor;

out vec3 norm;
out vec3 base_color;

uniform mat4 proj;

void main() {
    norm = aNorm;
    base_color = aColor;
    gl_Position = proj * vec4(aPos, 1);
}
