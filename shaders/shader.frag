#version 450

#extension GL_EXT_nonuniform_qualifier : enable

// struct MatData
// {
//     uint albedo_id;
// };

layout(location=0) in vec3 in_norm;
layout(location=1) in vec2 in_uv;
// layout(location=2) flat in uint in_texID;

layout(location=0) out vec4 out_color;

// layout(set=0, binding=3) buffer readonly ssbo_0
// {
//     MatData data[];
// } ssbo_matData;

// layout(set=0, binding=4) uniform sampler   sampler_0;
// layout(set=0, binding=3) uniform sampler2D textures[];

void main()
{
    // MatData matData = ssbo_matData.data[];

    // vec3 color = texture(sampler2D(textures[0], sampler_0), in_uv);
    // vec3 color = texture(textures[in_texID], in_uv).rgb;
    vec3 color = vec3(1.0, 0.0, 0.0);
    out_color = vec4(color, 1.0);
    // out_color = vec4(in_uv, 0.0, 1.0);
}
