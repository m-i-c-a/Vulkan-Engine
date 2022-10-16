#version 450

struct DrawData
{
    mat4 model_matrix;
};

layout(location=0) in vec3 a_pos;
layout(location=1) in vec3 a_norm;
layout(location=2) in vec2 a_uv;

layout(set=0, binding=0) uniform _global_ubo
{
    mat4 proj_matrix;
    mat4 view_matrix;
} global_ubo;

layout(set=0, binding=1) buffer readonly _global_draw_ssbo
{
    DrawData draw_data[];
} global_draw_ssbo;

layout(push_constant) uniform _push_constants
{
    uint draw_idx;
} push_constants;

void main()
{
    DrawData draw_data = global_draw_ssbo.draw_data[push_constants.draw_idx];
    gl_Position = global_ubo.proj_matrix * global_ubo.view_matrix * draw_data.model_matrix * vec4(a_pos, 1.0f);
}
