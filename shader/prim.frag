#version 440

in vec3 norm;
in vec3 base_color;

out vec4 out_Color;

vec3 l = normalize(vec3(1, 2, 3));

void main()
{
    out_Color = max(dot(l, normalize(norm)), 0) * vec4(base_color, 1) + vec4(0.1, 0.05, 0.02, 0);
}
