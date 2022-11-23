#include <vulkan/vulkan.h>

#include "Defines.hpp"

#include "vulkanwrapper/CommandBuffer.hpp"
#include "vulkanwrapper/CommandPool.hpp"
#include "vulkanwrapper/DescriptorPool.hpp"
#include "vulkanwrapper/DescriptorSet.hpp"
#include "vulkanwrapper/DescriptorSetLayout.hpp"

#include "core/BufferPool.hpp"

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
    void addPass();
    void addColorAttachment();
    void addDepthAttachment();

    void registerPipeline(const uint32_t pipelineID, const VkPipeline vk_pipeline);

    void addDraw(const DrawInfo& drawInfo);

    void execute(const VkCommandBuffer vk_cmdBuff);
};

VulkanWrapper::DescriptorPool createDescriptorPool();
VulkanWrapper::DescriptorSetLayout createDescriptorSetLayout();
VulkanWrapper::DescriptorSet allocateDescriptorSet(const VkDescriptorPool vk_pool, const VkDescriptorSetLayout vk_layout, void* pNext);

constexpr uint32_t MAX_RENDERABLE_COUNT = 1;
constexpr uint32_t MAX_MATERIAL_COUNT   = 1;
constexpr uint32_t MAX_PIPELINE_COUNT   = 1;
constexpr uint32_t MAX_TEXTURES         = 1;

constexpr VkDescriptorSetVariableDescriptorCountAllocateInfo vk_descriptorSetVariableDescriptorCountAllocInfo {
    .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO_EXT,
    .descriptorSetCount = 1,
    .pDescriptorCounts = &MAX_TEXTURES,
};

int func()
{
    // INIT

    // Create Global Layout and set in renderer
    RenderPlan renderer;
    renderer.addPass();
    renderer.addColorAttachment(/* Pass, AttachmentInfo */ );
    renderer.addColorAttachment(/* Pass, AttachmentInfo */ );
    renderer.addColorAttachment(/* Pass, AttachmentInfo */ );
    renderer.addDepthAttachment(/* Pass, AttachmentInfo */ );

    // Handle Subpass Dependencies

    // Create Global Vulkan Resources
    VulkanWrapper::CommandPool    cmdPool(0x0, 0);
    VulkanWrapper::CommandBuffer  cmdBuff(cmdPool.vk_handle);
    VulkanWrapper::DescriptorPool descPool = createDescriptorPool();
    VulkanWrapper::DescriptorSetLayout descSetLayout = createDescriptorSetLayout();
    VulkanWrapper::DescriptorSet descSet = allocateDescriptorSet(descPool.vk_handle, descSetLayout.vk_handle, (void*)&vk_descriptorSetVariableDescriptorCountAllocInfo);

    // Create / Define Resources for Uber Material State
    Core::BufferPool<MatData> matBufferPool(MAX_MATERIAL_COUNT, 
                                            MAX_MATERIAL_COUNT, 
                                            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                            VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT,
                                            VK_ACCESS_2_MEMORY_READ_BIT);

    // Create / Define Resources for Uber Draw State
    Core::BufferPool<DrawData> drawBufferPool(MAX_RENDERABLE_COUNT,
                                              MAX_RENDERABLE_COUNT,
                                              VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                              VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT,
                                              VK_ACCESS_2_MEMORY_READ_BIT);

    // APP SPECIFIC INIT

    // Create Pipeline
    uint32_t pipelineID = 0;
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



VulkanWrapper::DescriptorPool createDescriptorPool()
{
    // Allows for descriptors allocated from this pool to have bindings with the 
    // VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT flag set.
    const VkDescriptorPoolCreateFlags vk_flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;

    const VkDescriptorPoolSize vk_poolSizes[2] {
        {
            .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1
        },
        {
            .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .descriptorCount = 2
        }
    };

    const VkDescriptorPoolCreateInfo vk_createInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .flags = vk_flags,
        .maxSets = 1,
        .poolSizeCount = 2,
        .pPoolSizes = vk_poolSizes,
    };

    VulkanWrapper::DescriptorPool descPool(vk_createInfo);
    return descPool;
}

VulkanWrapper::DescriptorSetLayout createDescriptorSetLayout()
{
    const uint32_t bindingCount = 4;

    const VkDescriptorBindingFlags vk_bindlessFlags {
        // The size of the binding will be determined upon descriptor set allocation. 
        VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT | 

        // If all shader innvocations do not accesses memory via descriptor X, then descriptor
        // X does not need to contain a valid descriptor at the time of consumption. This is 
        // true for all descriptors within said binding. 
        VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT
    };

    const VkDescriptorBindingFlags vk_descriptorBindingFlags[bindingCount] {
        0x0,
        0x0,
        0x0,
        vk_bindlessFlags,
    };

    const VkDescriptorSetLayoutBindingFlagsCreateInfo vk_descriptorSetLayoutBindingFlagsCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO,
        .bindingCount = bindingCount,
        .pBindingFlags = vk_descriptorBindingFlags,
    };

    const VkDescriptorSetLayoutBinding vk_bindings[] {
        {
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_ALL,
            .pImmutableSamplers = nullptr,
        },
        {
            .binding = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_ALL,
            .pImmutableSamplers = nullptr,
        },
        {
            .binding = 2,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_ALL,
            .pImmutableSamplers = nullptr,
        },
        {
            .binding = 3,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_ALL,
            .pImmutableSamplers = nullptr,
        },
    };

    const VkDescriptorSetLayoutCreateInfo vk_createInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = &vk_descriptorSetLayoutBindingFlagsCreateInfo,
        .bindingCount = bindingCount,
        .pBindings = vk_bindings,
    };

    VulkanWrapper::DescriptorSetLayout descSetLayout(vk_createInfo);
    return descSetLayout;
}

VulkanWrapper::DescriptorSet allocateDesriptorSet(const VkDescriptorPool vk_pool, const VkDescriptorSetLayout vk_layout, void* pNext)
{
    const VkDescriptorSetAllocateInfo vk_allocInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext = pNext,
        .descriptorPool = vk_pool,
        .descriptorSetCount = 1,
        .pSetLayouts = &vk_layout,
    };

    VulkanWrapper::DescriptorSet descSet(vk_allocInfo);
    return descSet;

    VkDescriptorSetLayout vk_globalDescSetLayout;
}


// VulkanWrapper::PipelineLay
// {
//     const VkPipelineLayoutCreateInfo vk_pipelineLayoutCreateInfo {
//         .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
//         .setLayoutCount = 1,
//         .pSetLayouts = &appCore.m_vkGraphicsDescSetLayout,
//         .pushConstantRangeCount = 0,
//         .pPushConstantRanges = nullptr,
//     };

//     VK_CHECK(vkCreatePipelineLayout(vulkanCore.vk_device, &vk_pipelineLayoutCreateInfo, nullptr, &appCore.m_vkGraphicsPipelineLayout));
// }