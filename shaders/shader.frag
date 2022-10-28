#version 450

layout(location=0) in vec3 in_norm;
layout(location=1) in vec2 in_uv;

layout(location=0) out vec4 out_color;

void main()
{
    out_color = vec4(normalize(in_norm), 1.0);
}
