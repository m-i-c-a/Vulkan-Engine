#include <vector>
#include <string.h>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "Defines.hpp"
#include "ShaderStructs.hpp"
#include "VulkanCore.hpp"
#include "debug-utils/DebugUtilsEXT.hpp"
// #include "core/Buffer.hpp"
// #include "core/BufferPool.hpp"
// #include "core/CommandBuffer.hpp"
// #include "core/CommandPool.hpp"
// #include "core/DescriptorPool.hpp"
// #include "core/Fence.hpp"
// #include "core/Image.hpp"
// #include "core/ImageView.hpp"
// #include "core/SceneBuffer.hpp"
// #include "core/ShaderModule.hpp"
// #include "core/StorageBuffer.hpp"

// #include "core/buffer/Buffer.hpp"
// #include "core/Test/StagingBuffer.hpp"

/*
Next:

Obj Loading
Camera
ImGui
Hard Instancing Test (50x50x50)
Replace wait idles with PipelineBarriers + Semaphores 
Materials



Programmable Vertex Pulling (PVP) on Triangle
PVP on obj file mesh
Phong Shading
Bindless PerDrawData (push constants (PC))
PVP on multple obj file meshes
Textures in samplers
Sparse Textures
Bindless PerMaterialData (PC)
DrawIndirect
DrawIndirect + instancing
Auto gen shader structs (offline/pre-step)
PBR materials

Research
--------
PVP vs Traditional

Projects
----------------
Neural Net Trees
Clouds
Scene Graph Exploration
Culling
*/

/*
shader can have its reflection info

vert = processShader() // get reflection info
frag = processShader(
struct GlobalUBO
{

};

struct ObjectData 
{
    glm::mat4 modelMatrix { 1.0f };
};ne layout formed by the union of all the reflection info
*/

constexpr uint32_t MAX_MESHES = 2;
constexpr uint32_t MAX_RENDERABLES = 20;

struct Mesh
{
    uint32_t m_uIndexCount = 0;
    uint32_t m_uFirstIndex = 0;
    int32_t  m_iVertexOffset = 0;
};

struct Renderable
{
    uint32_t m_uIndexCount = 0;
    uint32_t m_uInstanceCount = 0;
    uint32_t m_uFirstIndex = 0;
    int32_t  m_iVertexOffset = 0;

    uint32_t m_uMeshIdx = 0;
    uint32_t m_uObjectIdx = 0;
};

struct Vertex
{
    float pos[3];
    float normal[3];
    float uv[2];
};

constexpr Vertex planeVertexData[] {
     { { -0.5f, -0.5f, 0.0f  }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f } }, // top left
     { { -0.5f,  0.5f, 0.0f  }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } }, // bot left
     { {  0.5f,  0.5f, 0.0f  }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f } }, // bot right
     { {  0.5f, -0.5f, 0.0f  }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f } }  // top right
};

constexpr uint32_t planeIndexData[] {
    0, 1, 2,
    0, 2, 3
};

constexpr Vertex triangleVertexData[] {
     { { -0.5f, -0.5f, 0.0f  }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f } }, // top left
     { {  0.0f,  0.5f, 0.0f  }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } }, // bot left
     { {  0.5f, -0.5f, 0.0f  }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f } }, // bot right
};

constexpr uint32_t triangleIndexData[] {
    0, 1, 2,
};


#include "core/BufferPool.hpp"
#include "core/PersistentDeviceBuffer.hpp"
#include "core/PersistentStagingBuffer.hpp"
#include "core/SceneBuffer.hpp"
#include "vulkanwrapper/Buffer.hpp"
#include "vulkanwrapper/CommandBuffer.hpp"
#include "vulkanwrapper/CommandPool.hpp"
#include "vulkanwrapper/DescriptorPool.hpp"
#include "vulkanwrapper/Fence.hpp"
#include "vulkanwrapper/ShaderModule.hpp"

struct AppCore
{
    VulkanWrapper::CommandPool* m_cmdPool = nullptr;
    VulkanWrapper::CommandBuffer* m_cmdBuff = nullptr;
    VulkanWrapper::DescriptorPool* m_descPool = nullptr;
    VulkanWrapper::Fence* m_swapchainImageAcquireFence = nullptr;
    uint32_t m_uActiveSwapchainImageIdx = 0;

    PersistentStagingBuffer* m_persistentStagingBuffer = nullptr;
    PersistentDeviceBuffer*  m_globalUBO = nullptr;

    SceneBuffer* m_sceneBuffer = nullptr;
    BufferPool<ObjectData>* m_objectDataPool = nullptr;
    BufferPool<RenderableInfo>* m_renderableInfoPool = nullptr;
    BufferPool<MeshInfo>* m_meshInfoPool = nullptr;

    VkDescriptorSetLayout m_vkCullDescSetLayout = VK_NULL_HANDLE;
    VkDescriptorSetLayout m_vkCompactDescSetLayout = VK_NULL_HANDLE;
    VkDescriptorSetLayout m_vkResetDescSetLayout = VK_NULL_HANDLE;
    VkDescriptorSetLayout m_vkGraphicsDescSetLayout = VK_NULL_HANDLE;
    VkPipelineLayout m_vkCullPipelineLayout = VK_NULL_HANDLE;
    VkPipelineLayout m_vkCompactPipelineLayout = VK_NULL_HANDLE;
    VkPipelineLayout m_vkResetPipelineLayout = VK_NULL_HANDLE;
    VkPipelineLayout m_vkGraphicsPipelineLayout = VK_NULL_HANDLE;
    VkPipeline m_vkCullPipeline = VK_NULL_HANDLE;
    VkPipeline m_vkCompactPipeline = VK_NULL_HANDLE;
    VkPipeline m_vkResetPipeline = VK_NULL_HANDLE;
    VkPipeline m_vkGraphicsPipeline = VK_NULL_HANDLE;
    VkDescriptorSet m_vkCullDescSet = VK_NULL_HANDLE;
    VkDescriptorSet m_vkCompactDescSet = VK_NULL_HANDLE;
    VkDescriptorSet m_vkResetDescSet = VK_NULL_HANDLE;
    VkDescriptorSet m_vkGraphicsDescSet = VK_NULL_HANDLE;

    VulkanWrapper::Buffer* m_ssboVisibleRenderableInfos = nullptr;
    VulkanWrapper::Buffer* m_ssboVisibleRenderableCount = nullptr;
    VulkanWrapper::Buffer* m_ssboMeshToCommandMapping = nullptr;
    VulkanWrapper::Buffer* m_ssboMeshVisible = nullptr;
    VulkanWrapper::Buffer* m_ssboDrawCommands = nullptr;
    VulkanWrapper::Buffer* m_ssboDrawCommandCount = nullptr;
    VulkanWrapper::Buffer* m_ssboMeshInstanceCount = nullptr;
    VulkanWrapper::Buffer* m_ssboFirstInstanceCount = nullptr;
    VulkanWrapper::Buffer* m_ssboDrawInfos = nullptr;
};

void updateBufferDescriptorSet(const VkDevice vk_device, const VkDescriptorSet vk_descSet, const uint32_t descBinding, const VkDescriptorType vk_descType, VkDescriptorBufferInfo&& vk_descBufferInfo)
{
    const VkWriteDescriptorSet vk_writeDescriptorSet {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = vk_descSet,
        .dstBinding = descBinding,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = vk_descType,
        .pImageInfo = nullptr,
        .pBufferInfo = &vk_descBufferInfo,
        .pTexelBufferView = nullptr,
    };

    vkUpdateDescriptorSets(vk_device, 1, &vk_writeDescriptorSet, 0, nullptr);
}

VkDescriptorSet allocateDescSet(const VkDevice vk_device, const VkDescriptorPool vk_descPool, const VkDescriptorSetLayout vk_descSetLayout)
{
    const VkDescriptorSetAllocateInfo vk_descSetAllocInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = vk_descPool,
        .descriptorSetCount = 1,
        .pSetLayouts = &vk_descSetLayout
    };

    VkDescriptorSet vk_descSet = VK_NULL_HANDLE;
    VK_CHECK(vkAllocateDescriptorSets(vk_device, &vk_descSetAllocInfo, &vk_descSet));
    return vk_descSet;
} 

void createGraphicsPipelineLayout(const VulkanCore& vulkanCore, AppCore& appCore)
{
    const VkDescriptorSetLayoutBinding vk_descSetLayoutBindings[] {
        {   // Global UBO
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
            .pImmutableSamplers = nullptr,
        },
        {   // Object SSBO
            .binding = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
            .pImmutableSamplers = nullptr,
        },
        {   // Draw Info SSBO
            .binding = 2,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
            .pImmutableSamplers = nullptr,
        },
    };

    const VkDescriptorSetLayoutCreateInfo vk_descSetLayoutCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = ARRAYSIZE(vk_descSetLayoutBindings),
        .pBindings = vk_descSetLayoutBindings,
    };

    VK_CHECK(vkCreateDescriptorSetLayout(vulkanCore.vk_device, &vk_descSetLayoutCreateInfo, nullptr, &appCore.m_vkGraphicsDescSetLayout));

    const VkPipelineLayoutCreateInfo vk_pipelineLayoutCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 1,
        .pSetLayouts = &appCore.m_vkGraphicsDescSetLayout,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = nullptr,
    };

    VK_CHECK(vkCreatePipelineLayout(vulkanCore.vk_device, &vk_pipelineLayoutCreateInfo, nullptr, &appCore.m_vkGraphicsPipelineLayout));
}

void createGraphicsPipeline(const VulkanCore& vulkanCore, AppCore& appCore)
{
    VulkanWrapper::ShaderModule* vertexShaderModule = new VulkanWrapper::ShaderModule();
    vertexShaderModule->create("../shaders/spirv/shader-vert.spv");

    VulkanWrapper::ShaderModule* fragmentShaderModule = new VulkanWrapper::ShaderModule();
    fragmentShaderModule->create("../shaders/spirv/shader-frag.spv");

    const VkPipelineShaderStageCreateInfo vk_shaderStageCreateInfos[] = {
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_VERTEX_BIT,
            .module = vertexShaderModule->m_vkShaderModule,
            .pName = "main",
            .pSpecializationInfo = nullptr
        },
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            .module = fragmentShaderModule->m_vkShaderModule,
            .pName = "main",
            .pSpecializationInfo = nullptr
        }
    };

    const VkVertexInputBindingDescription vk_vertexInputBindingDesc {
        .binding = 0,
        .stride = sizeof(Vertex),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
    };

    const VkVertexInputAttributeDescription vk_vertexInputAttribDescs[] {
        {
            .location = 0,
            .binding = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = 0,
        },
        {
            .location = 1,
            .binding = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = sizeof(float) * 3,
        },
        {
            .location = 2,
            .binding = 0,
            .format = VK_FORMAT_R32G32_SFLOAT,
            .offset = sizeof(float) * 6,
        },
    };

    const VkPipelineVertexInputStateCreateInfo vk_vertexInputStateCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = 1,
        .pVertexBindingDescriptions = &vk_vertexInputBindingDesc,
        .vertexAttributeDescriptionCount = ARRAYSIZE(vk_vertexInputAttribDescs),
        .pVertexAttributeDescriptions = vk_vertexInputAttribDescs,
    };

    const VkPipelineInputAssemblyStateCreateInfo vk_inputAssemblyStateCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE,
    };

    const VkPipelineTessellationStateCreateInfo vk_tessellationStateCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,
        .patchControlPoints = 0,
    };

    const VkViewport vk_viewport {
        .x = 0,
        .y = 0,
        .width = static_cast<float>(vulkanCore.vk_swapchainExtent.width),
        .height = static_cast<float>(vulkanCore.vk_swapchainExtent.height),
        .minDepth = 0,
        .maxDepth = 1,
    };

    const VkRect2D vk_scissor {
        .offset = {.x = 0, .y = 0},
        .extent = vulkanCore.vk_swapchainExtent
    };

    const VkPipelineViewportStateCreateInfo vk_viewportStateCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .pViewports = &vk_viewport,
        .scissorCount = 1,
        .pScissors = &vk_scissor,
    };

    const VkPipelineRasterizationStateCreateInfo vk_rasterizationStateCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .cullMode = VK_CULL_MODE_NONE,
        .frontFace = VK_FRONT_FACE_CLOCKWISE,
        .depthBiasEnable = VK_FALSE,
        .depthBiasConstantFactor = 0.f,
        .depthBiasClamp = 0.f,
        .depthBiasSlopeFactor = 0.f,
        .lineWidth = 1.f,
    };

    const VkPipelineMultisampleStateCreateInfo vk_multisampleStateCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .sampleShadingEnable = VK_FALSE,
        .minSampleShading = 0,
        .pSampleMask = nullptr,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable = VK_FALSE,
    };

    const VkPipelineColorBlendAttachmentState vk_colorBlendAttachmentState {
        .blendEnable = VK_FALSE,
        .srcColorBlendFactor = VK_BLEND_FACTOR_ZERO,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
        .colorBlendOp = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        .alphaBlendOp = VK_BLEND_OP_ADD,
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    };
    
    const VkPipelineColorBlendStateCreateInfo vk_colorBlendStateCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY,
        .attachmentCount = 1,
        .pAttachments = &vk_colorBlendAttachmentState,
        .blendConstants = { 0, 0, 0, 0 }
    };

    const VkPipelineRenderingCreateInfoKHR vk_renderingCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
        .colorAttachmentCount = 1,
        .pColorAttachmentFormats = &vulkanCore.vk_swapchainImageFormat
    };

    VkGraphicsPipelineCreateInfo vk_graphicsPipelineCreateInfo {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext = &vk_renderingCreateInfo,
        .stageCount = ARRAYSIZE(vk_shaderStageCreateInfos),
        .pStages = vk_shaderStageCreateInfos,
        .pVertexInputState = &vk_vertexInputStateCreateInfo,
        .pInputAssemblyState = &vk_inputAssemblyStateCreateInfo,
        .pTessellationState = &vk_tessellationStateCreateInfo,
        .pViewportState = &vk_viewportStateCreateInfo,
        .pRasterizationState = &vk_rasterizationStateCreateInfo,
        .pMultisampleState = &vk_multisampleStateCreateInfo,
        .pDepthStencilState = nullptr,
        .pColorBlendState = &vk_colorBlendStateCreateInfo,
        .pDynamicState = nullptr,
        .layout = appCore.m_vkGraphicsPipelineLayout,
        .renderPass = VK_NULL_HANDLE,
        .subpass = 0,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = 0,
    };

    VK_CHECK(vkCreateGraphicsPipelines(vulkanCore.vk_device, VK_NULL_HANDLE, 1, &vk_graphicsPipelineCreateInfo, nullptr, &appCore.m_vkGraphicsPipeline));

    delete fragmentShaderModule;
    delete vertexShaderModule;
}

void createCullPipelineLayout(const VulkanCore& vulkanCore, AppCore& appCore)
{
    const VkDescriptorSetLayoutBinding vk_descSetLayoutBindings[] {
        {
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
            .pImmutableSamplers = nullptr,
        },
        {
            .binding = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
            .pImmutableSamplers = nullptr,
        },
        {
            .binding = 2,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
            .pImmutableSamplers = nullptr,
        },
        {
            .binding = 3,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
            .pImmutableSamplers = nullptr,
        },
        {
            .binding = 4,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
            .pImmutableSamplers = nullptr,
        },
        {
            .binding = 5,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
            .pImmutableSamplers = nullptr,
        },
        {
            .binding = 6,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
            .pImmutableSamplers = nullptr,
        },
        {
            .binding = 7,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
            .pImmutableSamplers = nullptr,
        },
    };

    const VkDescriptorSetLayoutCreateInfo vk_descSetLayoutCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = ARRAYSIZE(vk_descSetLayoutBindings),
        .pBindings = vk_descSetLayoutBindings,
    };

    VK_CHECK(vkCreateDescriptorSetLayout(vulkanCore.vk_device, &vk_descSetLayoutCreateInfo, nullptr, &appCore.m_vkCullDescSetLayout));

    const VkPushConstantRange vk_pushConstantRange {
        .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
        .offset = 0,
        .size = sizeof(uint32_t),
    };

    const VkPipelineLayoutCreateInfo vk_pipelineLayoutCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 1,
        .pSetLayouts = &appCore.m_vkCullDescSetLayout,
        .pushConstantRangeCount = 1,
        .pPushConstantRanges = &vk_pushConstantRange,
    };

    VK_CHECK(vkCreatePipelineLayout(vulkanCore.vk_device, &vk_pipelineLayoutCreateInfo, nullptr, &appCore.m_vkCullPipelineLayout));
}

void createCompactPipelineLayout(const VulkanCore& vulkanCore, AppCore& appCore)
{
    const VkDescriptorSetLayoutBinding vk_descSetLayoutBindings[] {
        {
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
            .pImmutableSamplers = nullptr,
        },
        {
            .binding = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
            .pImmutableSamplers = nullptr,
        },
        {
            .binding = 2,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
            .pImmutableSamplers = nullptr,
        },
        {
            .binding = 3,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
            .pImmutableSamplers = nullptr,
        },
        {
            .binding = 4,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
            .pImmutableSamplers = nullptr,
        },
        {
            .binding = 5,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
            .pImmutableSamplers = nullptr,
        },
        {
            .binding = 6,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
            .pImmutableSamplers = nullptr,
        },
    };

    const VkDescriptorSetLayoutCreateInfo vk_descSetLayoutCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = ARRAYSIZE(vk_descSetLayoutBindings),
        .pBindings = vk_descSetLayoutBindings,
    };

    VK_CHECK(vkCreateDescriptorSetLayout(vulkanCore.vk_device, &vk_descSetLayoutCreateInfo, nullptr, &appCore.m_vkCompactDescSetLayout));

    const VkPipelineLayoutCreateInfo vk_pipelineLayoutCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 1,
        .pSetLayouts = &appCore.m_vkCompactDescSetLayout,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = nullptr
    };

    VK_CHECK(vkCreatePipelineLayout(vulkanCore.vk_device, &vk_pipelineLayoutCreateInfo, nullptr, &appCore.m_vkCompactPipelineLayout));
}

void createResetPipelineLayout(const VulkanCore& vulkanCore, AppCore& appCore)
{
    const VkDescriptorSetLayoutBinding vk_descSetLayoutBindings[] {
        {
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
            .pImmutableSamplers = nullptr,
        },
        {
            .binding = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
            .pImmutableSamplers = nullptr,
        },
    };

    const VkDescriptorSetLayoutCreateInfo vk_descSetLayoutCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = ARRAYSIZE(vk_descSetLayoutBindings),
        .pBindings = vk_descSetLayoutBindings,
    };

    VK_CHECK(vkCreateDescriptorSetLayout(vulkanCore.vk_device, &vk_descSetLayoutCreateInfo, nullptr, &appCore.m_vkResetDescSetLayout));

    const VkPushConstantRange vk_pushConstantRange {
        .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
        .offset = 0,
        .size = sizeof(uint32_t),
    };

    const VkPipelineLayoutCreateInfo vk_pipelineLayoutCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 1,
        .pSetLayouts = &appCore.m_vkResetDescSetLayout,
        .pushConstantRangeCount = 1,
        .pPushConstantRanges = &vk_pushConstantRange,
    };

    VK_CHECK(vkCreatePipelineLayout(vulkanCore.vk_device, &vk_pipelineLayoutCreateInfo, nullptr, &appCore.m_vkResetPipelineLayout));
}

void createPipelines(const VulkanCore& vulkanCore, AppCore& appCore)
{
    createCullPipelineLayout(vulkanCore, appCore);
    createCompactPipelineLayout(vulkanCore, appCore);
    createResetPipelineLayout(vulkanCore, appCore);
    createGraphicsPipelineLayout(vulkanCore, appCore);
    createGraphicsPipeline(vulkanCore, appCore);

    VulkanWrapper::ShaderModule* cullShaderModule = new VulkanWrapper::ShaderModule();
    cullShaderModule->create("../shaders/spirv/cull-comp.spv");

    VulkanWrapper::ShaderModule* compactShaderModule = new VulkanWrapper::ShaderModule();
    compactShaderModule->create("../shaders/spirv/compact-comp.spv");

    VulkanWrapper::ShaderModule* resetShaderModule = new VulkanWrapper::ShaderModule();
    resetShaderModule->create("../shaders/spirv/reset-comp.spv");

    const VkPipelineShaderStageCreateInfo vk_cullShaderStageCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_COMPUTE_BIT,
        .module = cullShaderModule->m_vkShaderModule,
        .pName = "main",
        .pSpecializationInfo = nullptr
    };

    const VkPipelineShaderStageCreateInfo vk_compactShaderStageCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_COMPUTE_BIT,
        .module = compactShaderModule->m_vkShaderModule,
        .pName = "main",
        .pSpecializationInfo = nullptr
    };

    const VkPipelineShaderStageCreateInfo vk_resetShaderStageCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_COMPUTE_BIT,
        .module = resetShaderModule->m_vkShaderModule,
        .pName = "main",
        .pSpecializationInfo = nullptr
    };

    const VkComputePipelineCreateInfo vk_cullPipelineCreateInfo {
        .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
        .stage = vk_cullShaderStageCreateInfo,
        .layout = appCore.m_vkCullPipelineLayout
    };

    const VkComputePipelineCreateInfo vk_compactPipelineCreateInfo {
        .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
        .stage = vk_compactShaderStageCreateInfo,
        .layout = appCore.m_vkCompactPipelineLayout
    };

    const VkComputePipelineCreateInfo vk_resetPipelineCreateInfo {
        .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
        .stage = vk_resetShaderStageCreateInfo,
        .layout = appCore.m_vkResetPipelineLayout
    };

    VK_CHECK(vkCreateComputePipelines(vulkanCore.vk_device, VK_NULL_HANDLE, 1, &vk_cullPipelineCreateInfo, nullptr, &appCore.m_vkCullPipeline));
    VK_CHECK(vkCreateComputePipelines(vulkanCore.vk_device, VK_NULL_HANDLE, 1, &vk_compactPipelineCreateInfo, nullptr, &appCore.m_vkCompactPipeline));
    VK_CHECK(vkCreateComputePipelines(vulkanCore.vk_device, VK_NULL_HANDLE, 1, &vk_resetPipelineCreateInfo, nullptr, &appCore.m_vkResetPipeline));

    delete resetShaderModule;
    delete compactShaderModule;
    delete cullShaderModule;
}


void loadMeshes(const VulkanCore& vulkanCore, AppCore& appCore)
{
    constexpr uint32_t MESH_ID_PLANE = 0;
    constexpr uint32_t MESH_ID_TRIANGLE = 1;
    const MeshInfo meshInfoPlane = appCore.m_sceneBuffer->queueUpload(MESH_ID_PLANE, sizeof(planeVertexData), (void*)planeVertexData, sizeof(planeIndexData), planeIndexData);
    const MeshInfo meshInfoTriangle = appCore.m_sceneBuffer->queueUpload(MESH_ID_TRIANGLE, sizeof(triangleVertexData), (void*)triangleVertexData, sizeof(triangleIndexData), triangleIndexData);

    const uint32_t meshInfoPlaneBlockIdx = (uint32_t)appCore.m_meshInfoPool->acquireBlock();
    MeshInfo& _meshInfoPlane = appCore.m_meshInfoPool->getWritableBlock(meshInfoPlaneBlockIdx);
    _meshInfoPlane = meshInfoPlane;

    const uint32_t meshInfoTriangleBlockIdx = (uint32_t)appCore.m_meshInfoPool->acquireBlock();
    MeshInfo& _meshInfoTriangle = appCore.m_meshInfoPool->getWritableBlock(meshInfoTriangleBlockIdx);
    _meshInfoTriangle = meshInfoTriangle;

    for (int i = 0; i < MAX_RENDERABLES; ++i)
    {
        const uint32_t objectDataBlockIdx = (uint32_t)appCore.m_objectDataPool->acquireBlock();
        ObjectData& objectData = appCore.m_objectDataPool->getWritableBlock(objectDataBlockIdx);
        objectData.modelMatrix = glm::mat4(1.0f);
        objectData.modelMatrix = glm::translate(objectData.modelMatrix, glm::vec3((2.0f * i) / MAX_RENDERABLES - 1.0f, 0.0f, 0.0f));
        objectData.modelMatrix = glm::scale(objectData.modelMatrix, glm::vec3(0.1f, 0.1f, 1.0f));

        const uint32_t renderableInfoBlockIdx = (uint32_t)appCore.m_renderableInfoPool->acquireBlock();
        RenderableInfo& renderableInfo = appCore.m_renderableInfoPool->getWritableBlock(renderableInfoBlockIdx);
        renderableInfo.meshID = i % 2 == 0 ? MESH_ID_PLANE : MESH_ID_TRIANGLE;
        renderableInfo.objectID = objectDataBlockIdx;
    }
}

void appIndirectInit(const VulkanCore& vulkanCore, AppCore& appCore)
{
    appCore.m_cmdPool = new VulkanWrapper::CommandPool();
    appCore.m_cmdPool->create(0x0, vulkanCore.graphicsQFamIdx);

    appCore.m_cmdBuff = new VulkanWrapper::CommandBuffer();
    appCore.m_cmdBuff->create(appCore.m_cmdPool->m_vkCmdPool);

    appCore.m_swapchainImageAcquireFence = new VulkanWrapper::Fence();
    appCore.m_swapchainImageAcquireFence->create(0x0);

    appCore.m_sceneBuffer = new SceneBuffer(sizeof(Vertex), 5000000, 2000000, 5000000);
    appCore.m_objectDataPool = new BufferPool<ObjectData>(MAX_RENDERABLES, MAX_RENDERABLES);
    appCore.m_renderableInfoPool = new BufferPool<RenderableInfo>(MAX_RENDERABLES, MAX_RENDERABLES); 
    appCore.m_meshInfoPool = new BufferPool<MeshInfo>(MAX_MESHES, MAX_MESHES); 

    createPipelines(vulkanCore, appCore);
    loadMeshes(vulkanCore, appCore);

    appCore.m_persistentStagingBuffer = new PersistentStagingBuffer();
    appCore.m_globalUBO = appCore.m_persistentStagingBuffer->registerDeviceBuffer(sizeof(GlobalUBO), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    appCore.m_persistentStagingBuffer->completeRegistration();

    glm::mat4 identMat { 1.0f };
    appCore.m_globalUBO->update(offsetof(GlobalUBO, projMatrix), sizeof(glm::mat4), &(identMat[0][0]));
    appCore.m_globalUBO->update(offsetof(GlobalUBO, viewMatrix), sizeof(glm::mat4), &(identMat[0][0]));

    // maybe set to all zeros?
    appCore.m_ssboVisibleRenderableInfos = new VulkanWrapper::Buffer();
    appCore.m_ssboVisibleRenderableInfos->create(MAX_RENDERABLES * sizeof(RenderableInfo), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
    appCore.m_ssboVisibleRenderableInfos->allocate(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    appCore.m_ssboVisibleRenderableInfos->bind();

    // maybe set to all zeros?
    appCore.m_ssboVisibleRenderableCount = new VulkanWrapper::Buffer();
    appCore.m_ssboVisibleRenderableCount->create(MAX_RENDERABLES * sizeof(uint32_t), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT);
    appCore.m_ssboVisibleRenderableCount->allocate(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    appCore.m_ssboVisibleRenderableCount->bind();

    // maybe set to all zeros?
    appCore.m_ssboMeshToCommandMapping = new VulkanWrapper::Buffer();
    appCore.m_ssboMeshToCommandMapping->create(MAX_MESHES * sizeof(MeshInfo), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
    appCore.m_ssboMeshToCommandMapping->allocate(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    appCore.m_ssboMeshToCommandMapping->bind();

    // maybe set to all zeros?
    appCore.m_ssboMeshVisible = new VulkanWrapper::Buffer();
    appCore.m_ssboMeshVisible->create(MAX_MESHES * sizeof(int), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
    appCore.m_ssboMeshVisible->allocate(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    appCore.m_ssboMeshVisible->bind();

    // maybe set to all zeros?
    appCore.m_ssboDrawCommands = new VulkanWrapper::Buffer();
    appCore.m_ssboDrawCommands->create(MAX_MESHES * sizeof(VkDrawIndexedIndirectCommand), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT);
    appCore.m_ssboDrawCommands->allocate(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    appCore.m_ssboDrawCommands->bind();

    // maybe set to all zeros?
    appCore.m_ssboDrawCommandCount = new VulkanWrapper::Buffer();
    appCore.m_ssboDrawCommandCount->create(sizeof(uint32_t), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT);
    appCore.m_ssboDrawCommandCount->allocate(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    appCore.m_ssboDrawCommandCount->bind();

    // maybe set to all zeros?
    appCore.m_ssboMeshInstanceCount = new VulkanWrapper::Buffer();
    appCore.m_ssboMeshInstanceCount->create(MAX_MESHES * sizeof(uint32_t), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
    appCore.m_ssboMeshInstanceCount->allocate(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    appCore.m_ssboMeshInstanceCount->bind();

    // maybe set to all zeros?
    appCore.m_ssboFirstInstanceCount = new VulkanWrapper::Buffer();
    appCore.m_ssboFirstInstanceCount->create(sizeof(uint32_t), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
    appCore.m_ssboFirstInstanceCount->allocate(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    appCore.m_ssboFirstInstanceCount->bind();

    // maybe set to all zeros?
    appCore.m_ssboDrawInfos = new VulkanWrapper::Buffer();
    appCore.m_ssboDrawInfos->create(MAX_RENDERABLES * sizeof(DrawInfo), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
    appCore.m_ssboDrawInfos->allocate(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    appCore.m_ssboDrawInfos->bind();

#ifdef DEBUG
    DebugUtilsEXT::setObjectName(VK_OBJECT_TYPE_BUFFER, (uint64_t)appCore.m_sceneBuffer->getVertexBuffer(), "Vertex Buffer");
    DebugUtilsEXT::setObjectName(VK_OBJECT_TYPE_BUFFER, (uint64_t)appCore.m_sceneBuffer->getIndexBuffer(), "Index Buffer");
    DebugUtilsEXT::setObjectName(VK_OBJECT_TYPE_BUFFER, (uint64_t)appCore.m_objectDataPool->getStagingBufferHandle(), "BufferPool - ObjectData - Staging");
    DebugUtilsEXT::setObjectName(VK_OBJECT_TYPE_BUFFER, (uint64_t)appCore.m_objectDataPool->getStorageBufferHandle(), "BufferPool - ObjectData - Storage");
    DebugUtilsEXT::setObjectName(VK_OBJECT_TYPE_BUFFER, (uint64_t)appCore.m_renderableInfoPool->getStagingBufferHandle(), "BufferPool - RenderableInfo - Staging");
    DebugUtilsEXT::setObjectName(VK_OBJECT_TYPE_BUFFER, (uint64_t)appCore.m_renderableInfoPool->getStorageBufferHandle(), "BufferPool - RenderableInfo - Storage");
    DebugUtilsEXT::setObjectName(VK_OBJECT_TYPE_BUFFER, (uint64_t)appCore.m_ssboVisibleRenderableInfos->m_vkBuffer, "VisibleRenderableInfos");
    DebugUtilsEXT::setObjectName(VK_OBJECT_TYPE_BUFFER, (uint64_t)appCore.m_ssboVisibleRenderableCount->m_vkBuffer, "VisibleCountBuffer");
    DebugUtilsEXT::setObjectName(VK_OBJECT_TYPE_BUFFER, (uint64_t)appCore.m_ssboDrawCommandCount->m_vkBuffer, "DrawCommandCount");
    DebugUtilsEXT::setObjectName(VK_OBJECT_TYPE_BUFFER, (uint64_t)appCore.m_ssboDrawCommands->m_vkBuffer, "DrawCommands");
    DebugUtilsEXT::setObjectName(VK_OBJECT_TYPE_BUFFER, (uint64_t)appCore.m_ssboDrawInfos->m_vkBuffer, "DrawInfos");
    DebugUtilsEXT::setObjectName(VK_OBJECT_TYPE_BUFFER, (uint64_t)appCore.m_ssboFirstInstanceCount->m_vkBuffer, "FirstInstanceCount");
    DebugUtilsEXT::setObjectName(VK_OBJECT_TYPE_BUFFER, (uint64_t)appCore.m_ssboMeshToCommandMapping->m_vkBuffer, "MeshToCommandMapptig");
    DebugUtilsEXT::setObjectName(VK_OBJECT_TYPE_BUFFER, (uint64_t)appCore.m_ssboMeshVisible->m_vkBuffer, "MeshVisible");
    DebugUtilsEXT::setObjectName(VK_OBJECT_TYPE_BUFFER, (uint64_t)appCore.m_ssboMeshInstanceCount->m_vkBuffer, "MeshInstanceCount");
    DebugUtilsEXT::setObjectName(VK_OBJECT_TYPE_BUFFER, (uint64_t)appCore.m_meshInfoPool->getStagingBufferHandle(), "MeshInfoStaging");
    DebugUtilsEXT::setObjectName(VK_OBJECT_TYPE_BUFFER, (uint64_t)appCore.m_meshInfoPool->getStorageBufferHandle(), "MeshInfoStorage");
    DebugUtilsEXT::setObjectName(VK_OBJECT_TYPE_BUFFER, (uint64_t)appCore.m_globalUBO->vkBuffer(), "GlobalUBO");
#endif

    appCore.m_descPool = new VulkanWrapper::DescriptorPool();
    appCore.m_descPool->create(4, {{
                            .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                            .descriptorCount = 1
                        },
                        {
                            .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                            .descriptorCount = 20
                        }});

    appCore.m_vkCullDescSet = allocateDescSet(vulkanCore.vk_device, appCore.m_descPool->m_vkDescPool, appCore.m_vkCullDescSetLayout);
    appCore.m_vkCompactDescSet = allocateDescSet(vulkanCore.vk_device, appCore.m_descPool->m_vkDescPool, appCore.m_vkCompactDescSetLayout);
    appCore.m_vkResetDescSet = allocateDescSet(vulkanCore.vk_device, appCore.m_descPool->m_vkDescPool, appCore.m_vkResetDescSetLayout);
    appCore.m_vkGraphicsDescSet = allocateDescSet(vulkanCore.vk_device, appCore.m_descPool->m_vkDescPool, appCore.m_vkGraphicsDescSetLayout);

    updateBufferDescriptorSet(vulkanCore.vk_device, appCore.m_vkCullDescSet, 0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, appCore.m_renderableInfoPool->getDescBufferInfo());
    updateBufferDescriptorSet(vulkanCore.vk_device, appCore.m_vkCullDescSet, 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, appCore.m_meshInfoPool->getDescBufferInfo());
    updateBufferDescriptorSet(vulkanCore.vk_device, appCore.m_vkCullDescSet, 2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, { appCore.m_ssboVisibleRenderableInfos->m_vkBuffer, 0, VK_WHOLE_SIZE });
    updateBufferDescriptorSet(vulkanCore.vk_device, appCore.m_vkCullDescSet, 3, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, { appCore.m_ssboVisibleRenderableCount->m_vkBuffer, 0, VK_WHOLE_SIZE });
    updateBufferDescriptorSet(vulkanCore.vk_device, appCore.m_vkCullDescSet, 4, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, { appCore.m_ssboMeshToCommandMapping->m_vkBuffer, 0, VK_WHOLE_SIZE });
    updateBufferDescriptorSet(vulkanCore.vk_device, appCore.m_vkCullDescSet, 5, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, { appCore.m_ssboMeshVisible->m_vkBuffer, 0, VK_WHOLE_SIZE });
    updateBufferDescriptorSet(vulkanCore.vk_device, appCore.m_vkCullDescSet, 6, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, { appCore.m_ssboDrawCommands->m_vkBuffer, 0, VK_WHOLE_SIZE });
    updateBufferDescriptorSet(vulkanCore.vk_device, appCore.m_vkCullDescSet, 7, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, { appCore.m_ssboDrawCommandCount->m_vkBuffer, 0, VK_WHOLE_SIZE });

    updateBufferDescriptorSet(vulkanCore.vk_device, appCore.m_vkCompactDescSet, 0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, { appCore.m_ssboVisibleRenderableInfos->m_vkBuffer, 0, VK_WHOLE_SIZE });
    updateBufferDescriptorSet(vulkanCore.vk_device, appCore.m_vkCompactDescSet, 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, { appCore.m_ssboMeshToCommandMapping->m_vkBuffer, 0, VK_WHOLE_SIZE });
    updateBufferDescriptorSet(vulkanCore.vk_device, appCore.m_vkCompactDescSet, 2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, { appCore.m_ssboMeshInstanceCount->m_vkBuffer, 0, VK_WHOLE_SIZE });
    updateBufferDescriptorSet(vulkanCore.vk_device, appCore.m_vkCompactDescSet, 3, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, { appCore.m_ssboFirstInstanceCount->m_vkBuffer, 0, VK_WHOLE_SIZE });
    updateBufferDescriptorSet(vulkanCore.vk_device, appCore.m_vkCompactDescSet, 4, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, { appCore.m_ssboDrawCommands->m_vkBuffer, 0, VK_WHOLE_SIZE });
    updateBufferDescriptorSet(vulkanCore.vk_device, appCore.m_vkCompactDescSet, 5, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, { appCore.m_ssboDrawInfos->m_vkBuffer, 0, VK_WHOLE_SIZE });
    updateBufferDescriptorSet(vulkanCore.vk_device, appCore.m_vkCompactDescSet, 6, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, { appCore.m_ssboVisibleRenderableCount->m_vkBuffer, 0, VK_WHOLE_SIZE });

    updateBufferDescriptorSet(vulkanCore.vk_device, appCore.m_vkGraphicsDescSet, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, { appCore.m_globalUBO->vkBuffer(), 0, VK_WHOLE_SIZE });
    updateBufferDescriptorSet(vulkanCore.vk_device, appCore.m_vkGraphicsDescSet, 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, appCore.m_objectDataPool->getDescBufferInfo());
    updateBufferDescriptorSet(vulkanCore.vk_device, appCore.m_vkGraphicsDescSet, 2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, { appCore.m_ssboDrawInfos->m_vkBuffer, 0, VK_WHOLE_SIZE });

    constexpr VkCommandBufferBeginInfo vk_cmdBeginInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    };

    VK_CHECK(vkResetCommandPool(vulkanCore.vk_device, appCore.m_cmdPool->m_vkCmdPool, 0x0));
    VK_CHECK(vkBeginCommandBuffer(appCore.m_cmdBuff->m_vkCmdBuff, &vk_cmdBeginInfo));

    appCore.m_sceneBuffer->flushQueuedUploads(appCore.m_cmdBuff->m_vkCmdBuff);
    appCore.m_objectDataPool->flushDirtyBlocks(appCore.m_cmdBuff->m_vkCmdBuff);
    appCore.m_renderableInfoPool->flushDirtyBlocks(appCore.m_cmdBuff->m_vkCmdBuff);
    appCore.m_meshInfoPool->flushDirtyBlocks(appCore.m_cmdBuff->m_vkCmdBuff);
    appCore.m_globalUBO->flush(appCore.m_cmdBuff->m_vkCmdBuff);

    VK_CHECK(vkEndCommandBuffer(appCore.m_cmdBuff->m_vkCmdBuff));

    const VkSubmitInfo vk_submitInfo {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = 0,
        .pWaitSemaphores = nullptr,
        .pWaitDstStageMask = nullptr,
        .commandBufferCount = 1,
        .pCommandBuffers = &appCore.m_cmdBuff->m_vkCmdBuff,
        .signalSemaphoreCount = 0,
        .pSignalSemaphores = 0,
    };

    VK_CHECK(vkQueueSubmit(vulkanCore.vk_graphicsQ, 1, &vk_submitInfo, VK_NULL_HANDLE));
    VK_CHECK(vkQueueWaitIdle(vulkanCore.vk_graphicsQ));
}

void appIndirectCull(const VulkanCore& vulkanCore, AppCore& appCore)
{
    constexpr VkCommandBufferBeginInfo vk_cmdBeginInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    };

    VK_CHECK(vkResetCommandPool(vulkanCore.vk_device, appCore.m_cmdPool->m_vkCmdPool, 0x0));
    VK_CHECK(vkBeginCommandBuffer(appCore.m_cmdBuff->m_vkCmdBuff, &vk_cmdBeginInfo));

    vkCmdBindPipeline(appCore.m_cmdBuff->m_vkCmdBuff, VK_PIPELINE_BIND_POINT_COMPUTE, appCore.m_vkCullPipeline);
    
    vkCmdBindDescriptorSets(appCore.m_cmdBuff->m_vkCmdBuff, VK_PIPELINE_BIND_POINT_COMPUTE, appCore.m_vkCullPipelineLayout, 0, 1, &appCore.m_vkCullDescSet, 0, nullptr);

    uint32_t renderableCount = MAX_RENDERABLES;
    vkCmdPushConstants(appCore.m_cmdBuff->m_vkCmdBuff, appCore.m_vkCullPipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(uint32_t), &renderableCount);

    vkCmdDispatch(appCore.m_cmdBuff->m_vkCmdBuff, MAX_RENDERABLES, 1, 1);

    VK_CHECK(vkEndCommandBuffer(appCore.m_cmdBuff->m_vkCmdBuff));

    const VkSubmitInfo vk_submitInfo {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = 0,
        .pWaitSemaphores = nullptr,
        .pWaitDstStageMask = nullptr,
        .commandBufferCount = 1,
        .pCommandBuffers = &appCore.m_cmdBuff->m_vkCmdBuff,
        .signalSemaphoreCount = 0,
        .pSignalSemaphores = 0,
    };

    VK_CHECK(vkQueueSubmit(vulkanCore.vk_graphicsQ, 1, &vk_submitInfo, VK_NULL_HANDLE));
    VK_CHECK(vkQueueWaitIdle(vulkanCore.vk_graphicsQ));
}

void appIndirectCompact(const VulkanCore& vulkanCore, AppCore& appCore)
{
    constexpr VkCommandBufferBeginInfo vk_cmdBeginInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    };

    VK_CHECK(vkResetCommandPool(vulkanCore.vk_device, appCore.m_cmdPool->m_vkCmdPool, 0x0));
    VK_CHECK(vkBeginCommandBuffer(appCore.m_cmdBuff->m_vkCmdBuff, &vk_cmdBeginInfo));

    vkCmdBindPipeline(appCore.m_cmdBuff->m_vkCmdBuff, VK_PIPELINE_BIND_POINT_COMPUTE, appCore.m_vkCompactPipeline);
    
    vkCmdBindDescriptorSets(appCore.m_cmdBuff->m_vkCmdBuff, VK_PIPELINE_BIND_POINT_COMPUTE, appCore.m_vkCompactPipelineLayout, 0, 1, &appCore.m_vkCompactDescSet, 0, nullptr);

    vkCmdDispatchIndirect(appCore.m_cmdBuff->m_vkCmdBuff, appCore.m_ssboVisibleRenderableCount->m_vkBuffer, 0);

    VK_CHECK(vkEndCommandBuffer(appCore.m_cmdBuff->m_vkCmdBuff));

    const VkSubmitInfo vk_submitInfo {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = 0,
        .pWaitSemaphores = nullptr,
        .pWaitDstStageMask = nullptr,
        .commandBufferCount = 1,
        .pCommandBuffers = &appCore.m_cmdBuff->m_vkCmdBuff,
        .signalSemaphoreCount = 0,
        .pSignalSemaphores = 0,
    };

    VK_CHECK(vkQueueSubmit(vulkanCore.vk_graphicsQ, 1, &vk_submitInfo, VK_NULL_HANDLE));
    VK_CHECK(vkQueueWaitIdle(vulkanCore.vk_graphicsQ));
}

void appIndirectReset(const VulkanCore& vulkanCore, AppCore& appCore)
{
    constexpr VkCommandBufferBeginInfo vk_cmdBeginInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    };

    VK_CHECK(vkResetCommandPool(vulkanCore.vk_device, appCore.m_cmdPool->m_vkCmdPool, 0x0));
    VK_CHECK(vkBeginCommandBuffer(appCore.m_cmdBuff->m_vkCmdBuff, &vk_cmdBeginInfo));

    vkCmdBindPipeline(appCore.m_cmdBuff->m_vkCmdBuff, VK_PIPELINE_BIND_POINT_COMPUTE, appCore.m_vkResetPipeline);
    
    vkCmdBindDescriptorSets(appCore.m_cmdBuff->m_vkCmdBuff, VK_PIPELINE_BIND_POINT_COMPUTE, appCore.m_vkResetPipelineLayout, 0, 1, &appCore.m_vkResetDescSet, 0, nullptr);

    uint32_t renderableCount = MAX_RENDERABLES;
    vkCmdPushConstants(appCore.m_cmdBuff->m_vkCmdBuff, appCore.m_vkResetPipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(uint32_t), &renderableCount);

    vkCmdDispatch(appCore.m_cmdBuff->m_vkCmdBuff, 1, 1, 1);

    VK_CHECK(vkEndCommandBuffer(appCore.m_cmdBuff->m_vkCmdBuff));

    const VkSubmitInfo vk_submitInfo {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = 0,
        .pWaitSemaphores = nullptr,
        .pWaitDstStageMask = nullptr,
        .commandBufferCount = 1,
        .pCommandBuffers = &appCore.m_cmdBuff->m_vkCmdBuff,
        .signalSemaphoreCount = 0,
        .pSignalSemaphores = 0,
    };

    VK_CHECK(vkQueueSubmit(vulkanCore.vk_graphicsQ, 1, &vk_submitInfo, VK_NULL_HANDLE));
    VK_CHECK(vkQueueWaitIdle(vulkanCore.vk_graphicsQ));
}

void appCleanup(const VulkanCore& vulkanCore, AppCore& appCore)
{
    delete appCore.m_cmdBuff;
    delete appCore.m_cmdPool;
    delete appCore.m_swapchainImageAcquireFence;
    delete appCore.m_descPool;

    delete appCore.m_sceneBuffer;
    delete appCore.m_objectDataPool;
    delete appCore.m_renderableInfoPool;
    delete appCore.m_meshInfoPool;

    delete appCore.m_globalUBO;
    delete appCore.m_persistentStagingBuffer;

    delete appCore.m_ssboVisibleRenderableInfos;
    delete appCore.m_ssboVisibleRenderableCount;
    delete appCore.m_ssboMeshToCommandMapping;
    delete appCore.m_ssboMeshVisible;
    delete appCore.m_ssboDrawCommands;
    delete appCore.m_ssboDrawCommandCount;
    delete appCore.m_ssboMeshInstanceCount;
    delete appCore.m_ssboFirstInstanceCount;
    delete appCore.m_ssboDrawInfos;

    vkDestroyDescriptorSetLayout(vulkanCore.vk_device, appCore.m_vkCullDescSetLayout, nullptr);
    vkDestroyDescriptorSetLayout(vulkanCore.vk_device, appCore.m_vkCompactDescSetLayout, nullptr);
    vkDestroyDescriptorSetLayout(vulkanCore.vk_device, appCore.m_vkResetDescSetLayout, nullptr);
    vkDestroyDescriptorSetLayout(vulkanCore.vk_device, appCore.m_vkGraphicsDescSetLayout, nullptr);

    vkDestroyPipelineLayout(vulkanCore.vk_device, appCore.m_vkCullPipelineLayout, nullptr);
    vkDestroyPipelineLayout(vulkanCore.vk_device, appCore.m_vkCompactPipelineLayout, nullptr);
    vkDestroyPipelineLayout(vulkanCore.vk_device, appCore.m_vkResetPipelineLayout, nullptr);
    vkDestroyPipelineLayout(vulkanCore.vk_device, appCore.m_vkGraphicsPipelineLayout, nullptr);

    vkDestroyPipeline(vulkanCore.vk_device, appCore.m_vkCullPipeline, nullptr);
    vkDestroyPipeline(vulkanCore.vk_device, appCore.m_vkCompactPipeline, nullptr);
    vkDestroyPipeline(vulkanCore.vk_device, appCore.m_vkResetPipeline, nullptr);
    vkDestroyPipeline(vulkanCore.vk_device, appCore.m_vkGraphicsPipeline, nullptr);
}

void appRender(const VulkanCore& vulkanCore, AppCore& appCore)
{
    VK_CHECK(vkAcquireNextImageKHR(vulkanCore.vk_device, vulkanCore.vk_swapchain, UINT64_MAX, VK_NULL_HANDLE, appCore.m_swapchainImageAcquireFence->m_vkFence, &appCore.m_uActiveSwapchainImageIdx));
    VK_CHECK(vkWaitForFences(vulkanCore.vk_device, 1, &appCore.m_swapchainImageAcquireFence->m_vkFence, VK_TRUE, UINT64_MAX));
    VK_CHECK(vkResetFences(vulkanCore.vk_device, 1, &appCore.m_swapchainImageAcquireFence->m_vkFence));

    // appIndirectCull(vulkanCore, appCore);
    // appIndirectCompact(vulkanCore, appCore);
    // appIndirectReset(vulkanCore, appCore);

    const VkRenderingAttachmentInfo vk_baseColorAttachmentInfo {
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .imageView = vulkanCore.vk_swapchainImageViews[appCore.m_uActiveSwapchainImageIdx],
        .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .clearValue = { .color = { 0.2f, 0.2f, 0.2f, 1.0f } }
    };

    const VkRenderingInfo vk_renderingInfo {
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
        .renderArea = { .offset = { 0, 0 }, .extent = { vulkanCore.vk_swapchainExtent.width, vulkanCore.vk_swapchainExtent.height } },
        .layerCount = 1,
        .colorAttachmentCount = 1,
        .pColorAttachments = &vk_baseColorAttachmentInfo,
        .pDepthAttachment = nullptr,
        .pStencilAttachment = nullptr,
    };

    const VkCommandBufferBeginInfo vk_cmdBuffBeginInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
    };

    VkCommandBuffer vk_cmdBuff = appCore.m_cmdBuff->m_vkCmdBuff;

    vkResetCommandPool(vulkanCore.vk_device, appCore.m_cmdPool->m_vkCmdPool, 0x0);

    VK_CHECK(vkBeginCommandBuffer(vk_cmdBuff, &vk_cmdBuffBeginInfo));

    const VkImageMemoryBarrier vk_colorAttachmentPreRenderBarrier {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .srcAccessMask = VK_ACCESS_NONE,
        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .image = vulkanCore.vk_swapchainImages[appCore.m_uActiveSwapchainImageIdx],
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
        }
    };

    vkCmdPipelineBarrier(vk_cmdBuff,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,             // srcStageMask
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, // dstStageMask
        0,
        0, nullptr,
        0, nullptr,
        1, &vk_colorAttachmentPreRenderBarrier // pImageMemoryBarriers
    );

    vkCmdBeginRendering(vk_cmdBuff, &vk_renderingInfo);

    VkDeviceSize pOffsets[] = { 0 }; 
    const VkBuffer vertexBuffer = appCore.m_sceneBuffer->getVertexBuffer();
    const VkBuffer indexBuffer = appCore.m_sceneBuffer->getIndexBuffer();

    vkCmdBindVertexBuffers(vk_cmdBuff, 0, 1, &vertexBuffer, pOffsets);
    vkCmdBindIndexBuffer(vk_cmdBuff, indexBuffer, 0, VK_INDEX_TYPE_UINT32);

    vkCmdBindDescriptorSets(vk_cmdBuff, VK_PIPELINE_BIND_POINT_GRAPHICS, appCore.m_vkGraphicsPipelineLayout, 0, 1, &appCore.m_vkGraphicsDescSet, 0, nullptr);

    vkCmdBindPipeline(vk_cmdBuff, VK_PIPELINE_BIND_POINT_GRAPHICS, appCore.m_vkGraphicsPipeline);

    vkCmdDrawIndexedIndirectCount(vk_cmdBuff, appCore.m_ssboDrawCommands->m_vkBuffer, 0, appCore.m_ssboDrawCommandCount->m_vkBuffer, 0, MAX_MESHES, sizeof(VkDrawIndexedIndirectCommand));

    vkCmdEndRendering(vk_cmdBuff);

    const VkImageMemoryBarrier vk_colorAttachmentPostRenderBarrier {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        .dstAccessMask = VK_ACCESS_NONE,
        .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        .image = vulkanCore.vk_swapchainImages[appCore.m_uActiveSwapchainImageIdx],
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
        }
    };

    vkCmdPipelineBarrier(vk_cmdBuff,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, // srcStageMask
        VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,          // dstStageMask
        0,
        0, nullptr,
        0, nullptr,
        1, &vk_colorAttachmentPostRenderBarrier // pImageMemoryBarriers
    );

    VK_CHECK(vkEndCommandBuffer(vk_cmdBuff));
}

void appSubmit(const VulkanCore& vulkanCore, AppCore& appCore)
{
      const VkSubmitInfo vk_submitInfo {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .waitSemaphoreCount = 0,
            .pWaitSemaphores = nullptr,
            .pWaitDstStageMask = nullptr,
            .commandBufferCount = 1,
            .pCommandBuffers = &appCore.m_cmdBuff->m_vkCmdBuff,
            .signalSemaphoreCount = 0,
            .pSignalSemaphores = nullptr,
        };

        VK_CHECK(vkQueueSubmit(vulkanCore.vk_graphicsQ, 1, &vk_submitInfo, VK_NULL_HANDLE));
        VK_CHECK(vkDeviceWaitIdle(vulkanCore.vk_device));

        //*** Present (wait for graphics work to complete)
        const VkPresentInfoKHR vk_presentInfo {
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .waitSemaphoreCount = 0,
            .pWaitSemaphores = nullptr,
            .swapchainCount = 1,
            .pSwapchains = &vulkanCore.vk_swapchain,
            .pImageIndices = &appCore.m_uActiveSwapchainImageIdx,
            .pResults = nullptr,
        };

        VK_CHECK(vkQueuePresentKHR(vulkanCore.vk_graphicsQ, &vk_presentInfo));
        VK_CHECK(vkDeviceWaitIdle(vulkanCore.vk_device));
}


int main()
{
    const int windowWidth = 500;
    const int windowHeight = 500;
    const char* windowName = "Engine";
    bool continueExecuting = true;

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    GLFWwindow* glfw_window = glfwCreateWindow(windowWidth, windowHeight, windowName, nullptr, nullptr);
    assert(glfw_window && "Failed to create window");

    const VkPhysicalDeviceVulkan12Features vk_physicalDeviceFeature12 {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
        .drawIndirectCount = VK_TRUE
    };

    const VkPhysicalDeviceVulkan13Features vk_physicalDeviceFeatures13 {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
        .pNext = (void*)&vk_physicalDeviceFeature12,
        .dynamicRendering = VK_TRUE
    };

    const VulkanInitInfo vulkanInitInfo {
        .window = glfw_window,
        .requestedInstanceLayerNames = { "VK_LAYER_KHRONOS_validation" },
        .requestedInstanceExtensionNames = { "VK_KHR_surface", "VK_KHR_xcb_surface", VK_EXT_DEBUG_UTILS_EXTENSION_NAME },
        .requestedDeviceExtensionNames = { VK_KHR_SWAPCHAIN_EXTENSION_NAME, "VK_KHR_dynamic_rendering" },
        .requestedSwapchainImageCount = 3,
        .requestedSwapchainImageFormat = VK_FORMAT_R8G8B8_SRGB,
        .requestedSwapchainImageExtent = { windowWidth, windowHeight },
        .requestedSwapchainImagePresentMode = VK_PRESENT_MODE_FIFO_KHR,
        .pDeviceFeatures = (void*)(&vk_physicalDeviceFeatures13)
    };

    VulkanCore vulkanCore = initVulkan(vulkanInitInfo);

    // Resource::initResources(vulkanCore.vk_physicalDevice, vulkanCore.vk_device);
    VulkanWrapper::Resource::initResources(vulkanCore.vk_physicalDevice, vulkanCore.vk_device);
    DebugUtilsEXT::init(vulkanCore.vk_instance, vulkanCore.vk_device);

    AppCore appCore {};

    // appInit(vulkanCore, appCore);
    appIndirectInit(vulkanCore, appCore);
    appIndirectCull(vulkanCore, appCore);
    appIndirectCompact(vulkanCore, appCore);


    while (!glfwWindowShouldClose(glfw_window))
    {
        glfwPollEvents();

        appRender(vulkanCore, appCore);

        appSubmit(vulkanCore, appCore);
    }

    appCleanup(vulkanCore, appCore);

    cleanupVulkan(vulkanCore);

    glfwDestroyWindow(glfw_window);
    glfwTerminate();
}