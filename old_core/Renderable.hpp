#include <inttypes.h>

struct Renderable
{
    uint32_t m_uIndexCount;    // get from loading
    uint32_t m_uInstanceCount; // get from loading
    int32_t  m_iVertexOffset;  // get from querying vertex buffer
};



/*

For every Vertex Format there is a dedicated vertex buffer.
For every Index Format there is a dedicated index buffer.

As of right now, we only support one vertex format and one index format - these being

vec3 pos
vec3 normal
vec2 uv

and UINT32_T indices.




*/