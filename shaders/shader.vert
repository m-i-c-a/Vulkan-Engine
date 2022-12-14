#version 450

struct ObjectData
{
    mat4 modelMatrix;
    uint matID;
};

layout(location=0) in vec3 a_pos;
layout(location=1) in vec3 a_norm;
layout(location=2) in vec2 a_uv;

layout(location=0) out vec3 o_norm;
layout(location=1) out vec2 o_uv;
layout(location=2) flat out uint o_matID;

layout(set=0, binding=0) uniform ubo_0
{
    mat4 projViewMatrix;
} ubo_global;

layout(set=0, binding=1) buffer readonly ssbo_0
{
    ObjectData data[];
} ssbo_objectData;

void main()
{
    ObjectData objectData = ssbo_objectData.data[gl_InstanceIndex];

    gl_Position = ubo_global.projViewMatrix * objectData.modelMatrix * vec4(a_pos, 1.0f);
    o_norm = a_norm; 
    o_uv = a_uv;
    o_matID = objectData.matID;
}
