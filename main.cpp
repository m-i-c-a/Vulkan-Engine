#include <vector>
#include <string.h>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "Defines.hpp"
#include "ShaderStructs.hpp"
#include "VulkanCore.hpp"
#include "core/Buffer.hpp"
#include "core/BufferPool.hpp"
#include "core/CommandBuffer.hpp"
#include "core/CommandPool.hpp"
#include "core/DescriptorPool.hpp"
#include "core/Fence.hpp"
#include "core/Image.hpp"
#include "core/ImageView.hpp"
#include "core/SceneBuffer.hpp"
#include "core/ShaderModule.hpp"

/*
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

struct AppCore
{
    CommandPool* m_cmdPool = nullptr;
    CommandBuffer* m_cmdBuff = nullptr;
    Fence* m_swapchainImageAcquireFence = nullptr;
    uint32_t m_uActiveSwapchainImageIdx = 0;

    Image* m_baseColorAttachmentImage = nullptr;
    ImageView* m_baseColorAttachmentImageView = nullptr;

    SceneBuffer* m_sceneBuffer = nullptr;
    BufferPool<GPUDrawCommand>* m_drawCommandPool = nullptr;
    BufferPool<ObjectData>* m_objectDataPool = nullptr;
    BufferPool<RenderableInfo>* m_renderableInfoPool = nullptr;


    VkPipelineLayout m_cullPipelineLayout = VK_NULL_HANDLE;
    VkPipelineLayout m_compactPipelineLayout = VK_NULL_HANDLE;
    VkPipeline m_cullPipeline = VK_NULL_HANDLE;
    VkPipeline m_compactPipeline = VK_NULL_HANDLE;



    BufferPool<ObjectData>* m_objectBufferPool = nullptr;
    Buffer* m_globalUBO = nullptr;

    DescriptorPool* m_descPool = nullptr;

    VkPipelineLayout m_vkPipelineLayout = VK_NULL_HANDLE;
    VkPipeline m_vkPipeline = VK_NULL_HANDLE;
    VkDescriptorSetLayout m_vkDescSet0Layout = VK_NULL_HANDLE;
    VkDescriptorSet m_vkDescSet0 = VK_NULL_HANDLE;

    std::vector<Renderable> m_renderables {};
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

void createPipelineLayout(const VulkanCore& vulkanCore, AppCore& appCore)
{
    const VkDescriptorSetLayoutBinding vk_descSetLayoutBindings[] {
        {   // Global UBO
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
            .pImmutableSamplers = nullptr,
        },
        {   // Global Draw SSBO
            .binding = 1,
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

    VkDescriptorSetLayout vk_descSet0Layout = VK_NULL_HANDLE;
    VK_CHECK(vkCreateDescriptorSetLayout(vulkanCore.vk_device, &vk_descSetLayoutCreateInfo, nullptr, &vk_descSet0Layout));

    const VkPushConstantRange vk_pushConstantRange {
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
        .offset = 0,
        .size = sizeof(uint32_t),
    };

    const VkPipelineLayoutCreateInfo vk_pipelineLayoutCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 1,
        .pSetLayouts = &vk_descSet0Layout,
        .pushConstantRangeCount = 1,
        .pPushConstantRanges = &vk_pushConstantRange,
    };

    VkPipelineLayout vk_pipelineLayout = VK_NULL_HANDLE;
    VK_CHECK(vkCreatePipelineLayout(vulkanCore.vk_device, &vk_pipelineLayoutCreateInfo, nullptr, &vk_pipelineLayout));

    appCore.m_vkDescSet0Layout = vk_descSet0Layout;
    appCore.m_vkPipelineLayout = vk_pipelineLayout;
}

void createPipeline(const VulkanCore& vulkanCore, AppCore& appCore)
{
    ShaderModule* vertexShaderModule = new ShaderModule();
    vertexShaderModule->create("../shaders/spirv/shader-vert.spv");

    ShaderModule* fragmentShaderModule = new ShaderModule();
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
        .layout = appCore.m_vkPipelineLayout,
        .renderPass = VK_NULL_HANDLE,
        .subpass = 0,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = 0,
    };

    VkPipeline vk_pipeline = VK_NULL_HANDLE;
    VK_CHECK(vkCreateGraphicsPipelines(vulkanCore.vk_device, VK_NULL_HANDLE, 1, &vk_graphicsPipelineCreateInfo, nullptr, &vk_pipeline));

    appCore.m_vkPipeline = vk_pipeline;

    delete fragmentShaderModule;
    delete vertexShaderModule;
}







VkPipelineLayout createCullPipelineLayout(const VulkanCore& vulkanCore)
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
    };

    const VkDescriptorSetLayoutCreateInfo vk_descSetLayoutCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = ARRAYSIZE(vk_descSetLayoutBindings),
        .pBindings = vk_descSetLayoutBindings,
    };

    VkDescriptorSetLayout vk_descSet0Layout = VK_NULL_HANDLE;
    VK_CHECK(vkCreateDescriptorSetLayout(vulkanCore.vk_device, &vk_descSetLayoutCreateInfo, nullptr, &vk_descSet0Layout));

    const VkPushConstantRange vk_pushConstantRange {
        .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
        .offset = 0,
        .size = sizeof(uint32_t),
    };

    const VkPipelineLayoutCreateInfo vk_pipelineLayoutCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 1,
        .pSetLayouts = &vk_descSet0Layout,
        .pushConstantRangeCount = 1,
        .pPushConstantRanges = &vk_pushConstantRange,
    };

    VkPipelineLayout vk_pipelineLayout = VK_NULL_HANDLE;
    VK_CHECK(vkCreatePipelineLayout(vulkanCore.vk_device, &vk_pipelineLayoutCreateInfo, nullptr, &vk_pipelineLayout));
    return vk_pipelineLayout;
}

VkPipelineLayout createCompactPipelineLayout(const VulkanCore& vulkanCore)
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
    };

    const VkDescriptorSetLayoutCreateInfo vk_descSetLayoutCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = ARRAYSIZE(vk_descSetLayoutBindings),
        .pBindings = vk_descSetLayoutBindings,
    };

    VkDescriptorSetLayout vk_descSet0Layout = VK_NULL_HANDLE;
    VK_CHECK(vkCreateDescriptorSetLayout(vulkanCore.vk_device, &vk_descSetLayoutCreateInfo, nullptr, &vk_descSet0Layout));

    const VkPipelineLayoutCreateInfo vk_pipelineLayoutCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 1,
        .pSetLayouts = &vk_descSet0Layout,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = nullptr
    };

    VkPipelineLayout vk_pipelineLayout = VK_NULL_HANDLE;
    VK_CHECK(vkCreatePipelineLayout(vulkanCore.vk_device, &vk_pipelineLayoutCreateInfo, nullptr, &vk_pipelineLayout));
    return vk_pipelineLayout;
}


void createPipelines(const VulkanCore& vulkanCore, AppCore& appCore)
{
    ShaderModule* cullShaderModule = new ShaderModule();
    cullShaderModule->create("../shaders/spirv/cull-comp.spv");

    ShaderModule* compactShaderModule = new ShaderModule();
    compactShaderModule->create("../shaders/spirv/compact-comp.spv");

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

    const VkComputePipelineCreateInfo vk_cullPipelineCreateInfo {
        .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
        .stage = vk_cullShaderStageCreateInfo,
        .layout = createCullPipelineLayout(vulkanCore)
    };

    const VkComputePipelineCreateInfo vk_compactPipelineCreateInfo {
        .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
        .stage = vk_compactShaderStageCreateInfo,
        .layout = createCompactPipelineLayout(vulkanCore)
    };

    VK_CHECK(vkCreateComputePipelines(vulkanCore.vk_device, VK_NULL_HANDLE, 1, &vk_cullPipelineCreateInfo, nullptr, &appCore.m_cullPipeline));
    VK_CHECK(vkCreateComputePipelines(vulkanCore.vk_device, VK_NULL_HANDLE, 1, &vk_compactPipelineCreateInfo, nullptr, &appCore.m_compactPipeline));

    appCore.m_cullPipelineLayout = vk_cullPipelineCreateInfo.layout;
    appCore.m_compactPipelineLayout = vk_compactPipelineCreateInfo.layout;

    delete compactShaderModule;
    delete cullShaderModule;
}




void loadMeshes(const VulkanCore& vulkanCore, AppCore& appCore)
{
    const uint32_t meshID = 0;
    const MeshInfo meshInfo = appCore.m_sceneBuffer->queueUpload(meshID, sizeof(planeVertexData), (void*)planeVertexData, sizeof(planeIndexData), planeIndexData);

    const uint32_t drawCommandBlockIdx = (uint32_t)appCore.m_drawCommandPool->acquireBlock();
    GPUDrawCommand& drawCommand = appCore.m_drawCommandPool->getWritableBlock(drawCommandBlockIdx);
    drawCommand.vk_drawIndexedIndirectCommand.indexCount = meshInfo.m_uIndexCount;
    drawCommand.vk_drawIndexedIndirectCommand.instanceCount = 0;
    drawCommand.vk_drawIndexedIndirectCommand.firstIndex = meshInfo.m_uFirstIndex;
    drawCommand.vk_drawIndexedIndirectCommand.vertexOffset = meshInfo.m_iVertexOffset;
    drawCommand.vk_drawIndexedIndirectCommand.firstInstance = 0;

    const uint32_t objectDataBlockIdx = (uint32_t)appCore.m_objectDataPool->acquireBlock();
    ObjectData& objectData = appCore.m_objectDataPool->getWritableBlock(objectDataBlockIdx);
    objectData.modelMatrix = glm::mat4(1.0f);

    const uint32_t renderableInfoBlockIdx = (uint32_t)appCore.m_renderableInfoPool->acquireBlock();
    RenderableInfo& renderableInfo = appCore.m_renderableInfoPool->getWritableBlock(renderableInfoBlockIdx);
    renderableInfo.meshID = meshID;
    renderableInfo.objectID = objectDataBlockIdx;


    constexpr VkCommandBufferBeginInfo vk_cmdBeginInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    };

    VK_CHECK(vkResetCommandPool(vulkanCore.vk_device, appCore.m_cmdPool->m_vkCmdPool, 0x0));
    VK_CHECK(vkBeginCommandBuffer(appCore.m_cmdBuff->m_vkCmdBuff, &vk_cmdBeginInfo));

    appCore.m_sceneBuffer->flushQueuedUploads(appCore.m_cmdBuff->m_vkCmdBuff);
    appCore.m_drawCommandPool->flushDirtyBlocks(appCore.m_cmdBuff->m_vkCmdBuff);
    appCore.m_objectDataPool->flushDirtyBlocks(appCore.m_cmdBuff->m_vkCmdBuff);
    appCore.m_renderableInfoPool->flushDirtyBlocks(appCore.m_cmdBuff->m_vkCmdBuff);

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



void createDescriptors(const VulkanCore& vulkanCore, AppCore& appCore)
{

}


void appIndirectInit(const VulkanCore& vulkanCore, AppCore& appCore)
{
    constexpr uint32_t MAX_MESHES = 1;
    constexpr uint32_t MAX_RENDERABLES = 1;

    appCore.m_sceneBuffer = new SceneBuffer(sizeof(Vertex), 5000000, 2000000, 5000000);
    appCore.m_drawCommandPool = new BufferPool<GPUDrawCommand>(MAX_MESHES, MAX_MESHES, VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT);
    appCore.m_objectBufferPool = new BufferPool<ObjectData>(MAX_RENDERABLES, MAX_RENDERABLES);
    appCore.m_renderableInfoPool = new BufferPool<RenderableInfo>(MAX_RENDERABLES, MAX_RENDERABLES); 

    Buffer* cullVisibleCounterBuffer = new Buffer();
    cullVisibleCounterBuffer->create(sizeof(uint32_t), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
    cullVisibleCounterBuffer->allocate(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    cullVisibleCounterBuffer->bind();

    // upload 0

    Buffer* compactInstanceCounterBuffer = new Buffer();
    compactInstanceCounterBuffer->create(sizeof(uint32_t), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
    compactInstanceCounterBuffer->allocate(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    compactInstanceCounterBuffer->bind();

    // upload 0

    Buffer* compactMeshActiveBuffer = new Buffer();
    compactMeshActiveBuffer->create(MAX_MESHES * sizeof(uint32_t), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
    compactMeshActiveBuffer->allocate(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    compactMeshActiveBuffer->bind();

    // upload list of 0s

    createPipelines(vulkanCore, appCore);
    createDescriptors(vulkanCore, appCore);
    loadMeshes(vulkanCore, appCore);

    constexpr VkCommandBufferBeginInfo vk_cmdBeginInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    };

    VK_CHECK(vkResetCommandPool(vulkanCore.vk_device, appCore.m_cmdPool->m_vkCmdPool, 0x0));
    VK_CHECK(vkBeginCommandBuffer(appCore.m_cmdBuff->m_vkCmdBuff, &vk_cmdBeginInfo));

    // input: 
    //      * loaded renderable count           (readonly)
    //      * buffer of draw indirect commands 

    // output:
    //      * buffer containing (meshId, objectID) pair
    //      * buffer containing count of visible renderables
    //      * buffer of draw indirect commands  (modifying instance count)

    vkCmdBindPipeline(appCore.m_cmdBuff->m_vkCmdBuff, VK_PIPELINE_BIND_POINT_COMPUTE, appCore.m_cullPipeline);
    
    // vkCmdBindDescriptorSets();
    // vkCmdPushConstants();

    vkCmdDispatch(appCore.m_cmdBuff->m_vkCmdBuff, 1, 1, 1);

    // pipeline barrier

    // input:
    //      * buffer of draw indirect commands                
    //      * buffer of bools (per loaded renderable)
    //      * buffer containing (meshId, objectID) pair       (readonly)
    //      * buffer containing count of visible renderables  (readonly)

    // ouptut:
    //      * buffer of draw indirect commands (modifying first instance)
    //      * buffer containing count of visible meshes

    vkCmdBindPipeline(appCore.m_cmdBuff->m_vkCmdBuff, VK_PIPELINE_BIND_POINT_COMPUTE, appCore.m_compactPipeline);

    // vkCmdBindDescriptorSets();

    // vkCmdDispatchIndirect()

    // pipeline barrier

    // input:
    //      * buffer of draw indirect commands             (readonly)
    //      * buffer containing count of visible meshes    (readonly)

    // vkCmdBindPipeline()

    // vkCmdDrawIndexedIndirectCount()

    VK_CHECK(vkEndCommandBuffer(appCore.m_cmdBuff->m_vkCmdBuff));
}


void appInit(const VulkanCore& vulkanCore, AppCore& appCore)
{
    createPipelineLayout(vulkanCore, appCore);
    createPipeline(vulkanCore, appCore);

    CommandPool* cmdPool = new CommandPool();
    cmdPool->create(0x0, vulkanCore.graphicsQFamIdx);

    CommandBuffer* cmdBuff = new CommandBuffer();
    cmdBuff->create(cmdPool->m_vkCmdPool);

    Fence* fence = new Fence();
    fence->create(0x0);

    SceneBuffer* sceneBuffer = new SceneBuffer(sizeof(Vertex), 5000000, 2000000, 5000000);
    BufferPool<ObjectData>* objectBufferPool = new BufferPool<ObjectData>(500, 500);

    Buffer* globalUBO = new Buffer();
    globalUBO->create(sizeof(GlobalUBO), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    globalUBO->allocate(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    globalUBO->bind();
    void* pGlobalUBO = globalUBO->map();
    glm::mat4 identMat { 1.0f };
    memcpy((char*)pGlobalUBO + offsetof(GlobalUBO, projMatrix), &(identMat[0][0]), sizeof(glm::mat4));
    memcpy((char*)pGlobalUBO + offsetof(GlobalUBO, viewMatrix), &(identMat[0][0]), sizeof(glm::mat4));
    globalUBO->unmap();

    DescriptorPool* descPool = new DescriptorPool();
    descPool->create(1, {{
                            .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                            .descriptorCount = 1
                        },
                        {
                            .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                            .descriptorCount = 1
                        }});

    const VkDescriptorSetAllocateInfo vk_descSetAllocInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = descPool->m_vkDescPool,
        .descriptorSetCount = 1,
        .pSetLayouts = &appCore.m_vkDescSet0Layout,
    };

    VkDescriptorSet vk_descSet0 = VK_NULL_HANDLE;
    VK_CHECK(vkAllocateDescriptorSets(vulkanCore.vk_device, &vk_descSetAllocInfo, &vk_descSet0));

    updateBufferDescriptorSet(vulkanCore.vk_device, vk_descSet0, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 
        {
            .buffer = globalUBO->m_vkBuffer,
            .offset = 0,
            .range = VK_WHOLE_SIZE,
        });

    updateBufferDescriptorSet(vulkanCore.vk_device, vk_descSet0, 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, objectBufferPool->getDescBufferInfo());

    std::vector<Renderable> renderables {};

    Renderable planeRenderable {
        .m_uIndexCount = ARRAYSIZE(planeIndexData),
        .m_uInstanceCount = 1,
        .m_uFirstIndex = sceneBuffer->queueIndexUpload(sizeof(planeIndexData), planeIndexData),
        .m_iVertexOffset = sceneBuffer->queueVertexUpload(sizeof(planeVertexData), (void*)planeVertexData),
    };

    Renderable triangleRenderable {
        .m_uIndexCount = ARRAYSIZE(triangleIndexData),
        .m_uInstanceCount = 1,
        .m_uFirstIndex = sceneBuffer->queueIndexUpload(sizeof(triangleIndexData), triangleIndexData),
        .m_iVertexOffset = sceneBuffer->queueVertexUpload(sizeof(triangleVertexData), (void*)triangleVertexData),
    };

    for (uint32_t i = 0; i < 10; ++i)
    {
       Renderable renderable = (i % 2 == 0) ? planeRenderable : triangleRenderable; 
       renderable.m_uObjectIdx = objectBufferPool->acquireBlock();

       ObjectData& objData = objectBufferPool->getWritableBlock(renderable.m_uObjectIdx);
       objData.modelMatrix = glm::mat4(1.0f);
       objData.modelMatrix = glm::translate(objData.modelMatrix, glm::vec3(-1.0f + 2.0f * ((float)i / 10.0f), 0.0f, 0.0f));
       objData.modelMatrix = glm::scale(objData.modelMatrix, glm::vec3(0.1f, 0.1f, 0.1f));

        renderables.push_back(std::move(renderable));
    }

    constexpr VkCommandBufferBeginInfo vk_cmdBeginInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    };

    VK_CHECK(vkBeginCommandBuffer(cmdBuff->m_vkCmdBuff, &vk_cmdBeginInfo));

    sceneBuffer->flushQueuedUploads(cmdBuff->m_vkCmdBuff);
    objectBufferPool->flushDirtyBlocks(cmdBuff->m_vkCmdBuff);

    VK_CHECK(vkEndCommandBuffer(cmdBuff->m_vkCmdBuff));

    const VkSubmitInfo vk_submitInfo {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = 0,
        .pWaitSemaphores = nullptr,
        .pWaitDstStageMask = nullptr,
        .commandBufferCount = 1,
        .pCommandBuffers = &cmdBuff->m_vkCmdBuff,
        .signalSemaphoreCount = 0,
        .pSignalSemaphores = 0,
    };

    VK_CHECK(vkQueueSubmit(vulkanCore.vk_graphicsQ, 1, &vk_submitInfo, VK_NULL_HANDLE));
    VK_CHECK(vkQueueWaitIdle(vulkanCore.vk_graphicsQ));

    // set
    appCore.m_cmdPool = cmdPool;
    appCore.m_cmdBuff = cmdBuff;
    appCore.m_swapchainImageAcquireFence = fence;
    appCore.m_sceneBuffer = sceneBuffer;
    appCore.m_objectBufferPool = objectBufferPool;
    appCore.m_globalUBO = globalUBO;
    appCore.m_renderables = std::move(renderables);
    appCore.m_descPool = descPool;
    appCore.m_vkDescSet0 = vk_descSet0;
}

void appCleanup(const VulkanCore& vulkanCore, AppCore& appCore)
{
    delete appCore.m_cmdBuff;
    delete appCore.m_cmdPool;
    delete appCore.m_swapchainImageAcquireFence;
    delete appCore.m_descPool;
    delete appCore.m_sceneBuffer;
    delete appCore.m_objectBufferPool;
    delete appCore.m_globalUBO;

    vkDestroyPipelineLayout(vulkanCore.vk_device, appCore.m_vkPipelineLayout, nullptr);
    vkDestroyPipeline(vulkanCore.vk_device, appCore.m_vkPipeline, nullptr);

    vkDestroyDescriptorSetLayout(vulkanCore.vk_device, appCore.m_vkDescSet0Layout, nullptr);
}

void appRender(const VulkanCore& vulkanCore, AppCore& appCore)
{
    VK_CHECK(vkAcquireNextImageKHR(vulkanCore.vk_device, vulkanCore.vk_swapchain, UINT64_MAX, VK_NULL_HANDLE, appCore.m_swapchainImageAcquireFence->m_vkFence, &appCore.m_uActiveSwapchainImageIdx));
    VK_CHECK(vkWaitForFences(vulkanCore.vk_device, 1, &appCore.m_swapchainImageAcquireFence->m_vkFence, VK_TRUE, UINT64_MAX));
    VK_CHECK(vkResetFences(vulkanCore.vk_device, 1, &appCore.m_swapchainImageAcquireFence->m_vkFence));

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

    vkCmdBindDescriptorSets(vk_cmdBuff, VK_PIPELINE_BIND_POINT_GRAPHICS, appCore.m_vkPipelineLayout, 0, 1, &appCore.m_vkDescSet0, 0, nullptr);

    vkCmdBindPipeline(vk_cmdBuff, VK_PIPELINE_BIND_POINT_GRAPHICS, appCore.m_vkPipeline);

    for (const Renderable& renderable : appCore.m_renderables)
    {
        vkCmdPushConstants(vk_cmdBuff, appCore.m_vkPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstants), &renderable.m_uObjectIdx);
        vkCmdDrawIndexed(vk_cmdBuff, renderable.m_uIndexCount, renderable.m_uInstanceCount, renderable.m_uFirstIndex, renderable.m_iVertexOffset, 0);
    }

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

    const VkPhysicalDeviceVulkan13Features vk_physicalDeviceFeatures13 {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
        .dynamicRendering = VK_TRUE
    };

    const VulkanInitInfo vulkanInitInfo {
        .window = glfw_window,
        .requestedInstanceLayerNames = { "VK_LAYER_KHRONOS_validation" },
        .requestedInstanceExtensionNames = { "VK_KHR_surface", "VK_KHR_xcb_surface" },
        .requestedDeviceExtensionNames = { VK_KHR_SWAPCHAIN_EXTENSION_NAME, "VK_KHR_dynamic_rendering" }, 
        .requestedSwapchainImageCount = 3,
        .requestedSwapchainImageFormat = VK_FORMAT_R8G8B8_SRGB,
        .requestedSwapchainImageExtent = { windowWidth, windowHeight },
        .requestedSwapchainImagePresentMode = VK_PRESENT_MODE_FIFO_KHR,
        .pDeviceFeatures = (void*)(&vk_physicalDeviceFeatures13)
    };

    VulkanCore vulkanCore = initVulkan(vulkanInitInfo);

    Resource::initResources(vulkanCore.vk_physicalDevice, vulkanCore.vk_device);

    AppCore appCore {};

    appInit(vulkanCore, appCore);

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