#ifndef MICA_SHADER_STRUCTS_HPP
#define MICA_SHADER_STRUCTS_HPP
// should be written out via reflection

#include <glm/mat4x4.hpp>

struct GlobalUBO
{
    glm::mat4 projMatrix { 1.0f };
    glm::mat4 viewMatrix { 1.0f };
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

#endif // MICA_SHADER_STRUCTS_HPP