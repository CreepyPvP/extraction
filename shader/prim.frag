#version 440

in vec3 base_color;

out vec4 out_Color;

void main()
{
    out_Color = vec4(base_color, 1);
}
