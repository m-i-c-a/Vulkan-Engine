#version 450

struct ObjectData
{
    mat4 modelMatrix;
};

struct DrawInfo
{
    uint objID;
};

layout(location=0) in vec3 a_pos;
layout(location=1) in vec3 a_norm;
layout(location=2) in vec2 a_uv;

layout(set=0, binding=0) uniform ubo_0
{
    mat4 projMatrix;
    mat4 viewMatrix;
} ubo_global;

layout(set=0, binding=1) buffer readonly ssbo_0
{
    ObjectData data[];
} ssbo_objectData;

layout(set=0, binding=2) buffer readonly ssbo_1
{
    DrawInfo data[];
} ssbo_drawInfo;


void getIDs(in uint instanceIndex, inout uint objID)
{
    DrawInfo info = ssbo_drawInfo.data[instanceIndex];
    objID  = info.objID;
}


void main()
{
    uint objID = 0;
    getIDs(gl_InstanceIndex, objID);

    ObjectData objectData = ssbo_objectData.data[objID];
    gl_Position = ubo_global.projMatrix * ubo_global.viewMatrix * objectData.modelMatrix * vec4(a_pos, 1.0f);
}
