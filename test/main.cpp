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

struct Renderer
{
    void registerMatieral();
};

template<class T>
struct BufferPool
{

};

template<class T>
struct MaterialState
{
    const VkDescriptorSet vk_descSet;
    BufferPool<T> pool;

    uint32_t acquireBlock();
    T& getWriteableData();
};

// std::unordered_map<uint32_t, MaterialState> map;

void createMaterialDescriptorSets(const VkDescriptorPool vk_descPool, const uint32_t descriptorSetCount, VkDescriptorSetLayout& vk_descSetLayout, std::vector<VkDescriptorSet>& vk_descSets);


int main()
{
    // INIT

    // Create Global Layout and set in renderer
    Renderer renderer;

    // Create Descriptor Pool
    VkDescriptorPool vk_descPool;

    // Iterate over all materials 
    // -- create material descriptor set
    // -- create material descriptor set layout
    // -- create material mat buffer pool
    VkDescriptorSetLayout vk_materialDescSetLayout = VK_NULL_HANDLE;

    const uint16_t materialCount = 1;
    std::vector<VkDescriptorSet> vk_materialDescSets (materialCount, VK_NULL_HANDLE);
    createMaterialDescriptorSets(vk_descPool, materialCount, vk_materialDescSetLayout, vk_materialDescSets);

    for (uint16_t i = 0; i < materialCount; ++i)
    {
        const VkDescriptorSet vk_descSet = vk_materialDescSets[i];

        BufferPool<Shader_MatData> pool;

        renderer.registerMatieral();
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

struct MatData
{

};

struct DrawData
{
    uint32_t matID;
};

struct DrawInfo
{

};

struct RenderPlan
{
    void registerPipeline(const uint32_t pipelineID, const VkPipeline vk_pipeline);

    void addDraw(const DrawInfo& drawInfo);

    void execute(const VkCommandBuffer vk_cmdBuff);
};

int func()
{
    // INIT

    // Create Global Layout and set in renderer
    RenderPlan renderer;

    // Create Global Vulkan Resources
    VkDescriptorPool vk_descPool;
    VkCommandBuffer vk_cmdBuff;

    // Create / Define Resources for Uber Material State
    VkDescriptorSetLayoutBinding vk_matDescSetLayBinding;

    BufferPool<MatData> matBufferPool;

    // Create / Define Resources for Uber Draw State
    VkDescriptorSetLayoutBinding vk_drawDescSetLayBinding;

    BufferPool<DrawData> drawBufferPool;

    // Create Global Descriptor Set
    VkDescriptorSetLayout vk_globalDescSetLayout;
    VkDescriptorSet vk_globalDescSet;

    // APP SPECIFIC INIT

    // Create Pipeline
    uint32_t pipelineID;
    VkPipelineLayout vk_pipelineLayout;
    VkPipeline vk_pipeline;

    // Register created Pipeline with renderer
    renderer.registerPipeline(pipelineID, vk_pipeline);

    // Load Model - currently drawID == renderableID
    uint32_t drawID; // <- drawBufferPool->acquireBlock()
    uint32_t matID ; // <- matBufferPool->acquireBlock()

    // Add matID to DrawData

    // Upload model information

#ifdef CULL
    // Add renderble(s) to tile

    // Add tile to grid

    // Cull
#endif

    // Add renderables we want to draw to Renderer
    DrawInfo drawInfo;
    renderer.addDraw(drawInfo);

    // Render
    renderer.execute(vk_cmdBuff);

    // Submit



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