#ifndef MICA_SHADER_STRUCTS_HPP
#define MICA_SHADER_STRUCTS_HPP
// should be written out via reflection

#include <glm/mat4x4.hpp>

struct GlobalUBO
{
    glm::mat4 projMatrix { 1.0f };
    glm::mat4 viewMatrix { 1.0f };
    glm::mat4 projViewMatrix { 1.0f };
    uint texID = 0;
};

struct ObjectData 
{
    glm::mat4 modelMatrix { 1.0f };
};

struct PushConstants
{
    uint32_t objectIdx = 0;
};

struct GPUDrawCommand
{
   VkDrawIndexedIndirectCommand vk_drawIndexedIndirectCommand; 
};

struct RenderableInfo
{
    uint32_t meshID;
    uint32_t objectID;
};

struct DrawInfo
{
    uint32_t objectID;
};

struct MeshInfo
{
    uint32_t m_uIndexCount = 0;
    uint32_t m_uFirstIndex = 0;
    int32_t  m_iVertexOffset = 0;
};

struct MatData
{
    float x;
};

struct DrawData
{
    glm::mat4 modelMatrix { 1.0f };
    uint32_t matID;
};

#endif // MICA_SHADER_STRUCTS_HPP