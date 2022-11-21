#include <vector>
#include <tuple>
#include <string>
#include <unordered_map>

#include <vulkan/vulkan.h>

#include "Defines.hpp"

struct Shader_DrawData
{

};

struct Shader_MatData
{

};

struct Shader_Material_0_Data
{

};

struct Shader_Material_1_Data
{

};

struct Shader_Material_2_Data
{

};

std::tuple<Shader_Material_0_Data,
           Shader_Material_1_Data,
           Shader_Material_2_Data> materialBuffers;


struct Material
{
    // Reflection Information for structure

};

class BufferPool_HashMap
{
private:
    struct MemberInfo
    {
        VkDeviceSize vk_offset;
        VkDeviceSize vk_size;
    };

    void* pData;
    std::unordered_map<std::string, MemberInfo> memberInfo; 
public:

    uint16_t acquireBlock();
    void updateBlock(const uint16_t blockID, const std::string& memberName, void* data);
};

void createMaterialDescriptorSets(const VkDescriptorPool vk_descPool, const uint32_t descriptorSetCount, VkDescriptorSetLayout& vk_descSetLayout, std::vector<VkDescriptorSet>& vk_descSets);


int main()
{
    // INIT

    // Create Global Layout and set in renderer

    // Create Descriptor Pool
    VkDescriptorPool vk_descPool;

    // Iterate over all materials 
    // -- create material descriptor set
    // -- create material descriptor set layout
    // -- create material mat buffer pool
    VkDescriptorSetLayout vk_materialDescSetLayout = VK_NULL_HANDLE;

    const uint16_t materialCount = 0;
    std::vector<VkDescriptorSet> vk_materialDescSets (materialCount, VK_NULL_HANDLE);
    createMaterialDescriptorSets(vk_descPool, materialCount, vk_materialDescSetLayout, vk_materialDescSets);

    std::apply([](){}, materialBuffers);

    for (uint16_t i = 0; i < materialCount; ++i)
    {
        const VkDescriptorSet vk_descSet = vk_materialDescSets[i];

        // Get Shader Structure

        // Create Buffer Pool

        // Register Material with Renderer
    }

    // Iterate over all pipelines
    // -- create pipeline layout 
    // -- -- hash and make sure only created once + set layout for associated material
    // -- create pipeline

    const uint32_t pipelineCount = 0;

    for (uint16_t i = 0; i < pipelineCount; ++i)
    {
        // Get reflection info


        // If material has not been created, create it and register with Renderer

        // Create Pipeline

        // Register Pipeline with Renderer
    }

    // Register materials + pipelines with renderer

    // MODEL LOAD

    // uint32_t renderableInfoID
    // uint32_t matID
    // uint32_t drawID

    // Populate staging memory with draw + mat ssbo + textures updates

    // Add renderables to grid

    // CULL

    // Add renderables to renderer

    // RENDER

    // Draw renderables

    // SUBMIT 

    // Ensure all needed uploads are done
    // Submit
}

void createMaterialDescriptorSets(const VkDescriptorPool vk_descPool, const uint32_t descriptorSetCount, VkDescriptorSetLayout& vk_descSetLayout, std::vector<VkDescriptorSet>& vk_descSets)
{
    const VkDevice vk_device = VK_NULL_HANDLE;

    const VkDescriptorSetLayoutBinding vk_descSetLayoutBinding {
        .binding = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_ALL,
        .pImmutableSamplers = nullptr,
    };

    const VkDescriptorSetLayoutCreateInfo vk_descSetLayoutCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0x0,
        .bindingCount = 1,
        .pBindings = &vk_descSetLayoutBinding,
    };

    VK_CHECK(vkCreateDescriptorSetLayout(vk_device, &vk_descSetLayoutCreateInfo, nullptr, &vk_descSetLayout));

    VkDescriptorSetAllocateInfo vk_descSetAllocInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext = nullptr,
        .descriptorPool = vk_descPool,
        .descriptorSetCount = descriptorSetCount,
        .pSetLayouts = &vk_descSetLayout,
    };

    VK_CHECK(vkAllocateDescriptorSets(vk_device, &vk_descSetAllocInfo, vk_descSets.data()));
}