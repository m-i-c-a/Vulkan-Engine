#version 450

struct DrawData
{
    mat4 modelMatrix;
};

layout(location=0) in vec3 a_pos;
layout(location=1) in vec3 a_norm;
layout(location=2) in vec2 a_uv;

layout(set=0, binding=0) uniform ubo_0
{
    mat4 projMatrix;
    mat4 viewMatrix;
} globalUBO;

layout(set=0, binding=1) buffer readonly ssbo_0
{
    DrawData data[];
} objectSSBO;

void main()
{
    DrawData drawData = objectSSBO.data[gl_InstanceIndex];
    gl_Position = globalUBO.projMatrix * globalUBO.viewMatrix * drawData.modelMatrix * vec4(a_pos, 1.0f);
}
