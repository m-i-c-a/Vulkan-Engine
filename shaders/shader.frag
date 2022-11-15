#version 450

struct MatData
{
    uint albedo_id;
};

layout(location=0) in vec3 in_norm;
layout(location=1) in vec2 in_uv;
layout(location=2) flat in uint in_matID;

layout(location=0) out vec4 out_color;

layout(set=0, binding=3) buffer readonly ssbo_0
{
    MatData data[];
} ssbo_matData;

layout(set=0, binding=4) uniform sampler   sampler_0;
layout(set=0, binding=5) uniform texture2D textures[1];

void main()
{
    MatData matData = ssbo_matData.data[in_matID];

    vec3 color = texture(sampler2D(textures[matData.albedo_id], sampler_0), in_uv);
    out_color = vec4(color, 1.0);
}
