#version 450

struct ObjectData
{
    mat4 modelMatrix;
};

// struct DrawInfo
// {
//     // uint matID;
//     uint objID;
// };

layout(location=0) in vec3 a_pos;
layout(location=1) in vec3 a_norm;
layout(location=2) in vec2 a_uv;

layout(location=0) out vec3 o_norm;
layout(location=1) out vec2 o_uv;
layout(location=2) flat out uint o_texID;
// layout(location=1) flat out uint o_matID;

layout(set=0, binding=0) uniform ubo_0
{
    // mat4 projMatrix;
    // mat4 viewMatrix;
    mat4 projViewMatrix;
    // uint texID;
} ubo_global;

layout(set=0, binding=1) buffer readonly ssbo_0
{
    ObjectData data[];
} ssbo_objectData;

// layout(set=0, binding=2) buffer readonly ssbo_1
// {
//     DrawInfo data[];
// } ssbo_drawInfo;

void main()
{
    // DrawInfo drawInfo = ssbo_drawInfo.data[gl_InstanceIndex];
    // ObjectData objectData = ssbo_objectData.data[drawInfo.objID];

    ObjectData objectData = ssbo_objectData.data[gl_InstanceIndex];

    gl_Position = ubo_global.projViewMatrix * objectData.modelMatrix * vec4(a_pos, 1.0f);
    o_norm = a_norm; 
    o_uv = a_uv;
    // o_texID = ubo_global.texID;
    // o_matID = drawInfo.matID;
}
