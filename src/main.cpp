#include <vector>
#include <string.h>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_vulkan.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "Defines.hpp"
#include "ShaderStructs.hpp"
#include "VulkanCore.hpp"
#include "Loader.hpp"
#include "TextureLoader.hpp"
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
Depth Buffer
Sparse Textures

LODs
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

constexpr uint32_t MAX_MESHES = 3;
constexpr uint32_t DIM = 1;
constexpr uint32_t MAX_RENDERABLES = DIM * DIM * DIM;
constexpr uint32_t CURRENT_RENDERABLES = MAX_RENDERABLES;
// constexpr uint32_t MAX_RENDERABLES = 8000;
// constexpr uint32_t CURRENT_RENDERABLES = 8000;

constexpr uint32_t ARRAY_OF_TEXTURES_SIZE = 10;
constexpr uint32_t ACTIVE_TEXTURES = 4;

static constexpr int windowWidth = 500;
static constexpr int windowHeight = 500;

static bool renderGui = true;
static bool globalUBODirty = false;
static float FOV = 45.0f;
static float nearZ = 0.1f;
static float farZ = 100.0f;
static glm::vec3 camPos { 0.0f, 0.0f, -1.0f * (float)DIM };

static int activeTextureID = 0;

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

#include "core/BufferPool.hpp"
#include "core/PersistentDeviceBuffer.hpp"
#include "core/PersistentStagingBuffer.hpp"
#include "core/SceneBuffer.hpp"
#include "core/StagingBuffer.hpp"
#include "core/Texture.hpp"

#include "vulkanwrapper/Generic.hpp"

#include "vulkanwrapper/Buffer.hpp"
#include "vulkanwrapper/CommandBuffer.hpp"
#include "vulkanwrapper/CommandPool.hpp"
#include "vulkanwrapper/DescriptorPool.hpp"
#include "vulkanwrapper/DeviceMemory.hpp"
#include "vulkanwrapper/Fence.hpp"
#include "vulkanwrapper/Image.hpp"
#include "vulkanwrapper/ImageView.hpp"
#include "vulkanwrapper/Sampler.hpp"
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

    VkDescriptorSetLayout m_vkCullDescSetLayout = VK_NULL_HANDLE;
    VkDescriptorSetLayout m_vkCompactDrawsDescSetLayout = VK_NULL_HANDLE;
    VkDescriptorSetLayout m_vkCompactBindlessDescSetLayout = VK_NULL_HANDLE;
    VkDescriptorSetLayout m_vkResetDescSetLayout = VK_NULL_HANDLE;
    VkDescriptorSetLayout m_vkGraphicsDescSetLayout = VK_NULL_HANDLE;
    VkPipelineLayout m_vkCullPipelineLayout = VK_NULL_HANDLE;
    VkPipelineLayout m_vkCompactDrawsPipelineLayout = VK_NULL_HANDLE;
    VkPipelineLayout m_vkCompactBindlessPipelineLayout = VK_NULL_HANDLE;
    VkPipelineLayout m_vkResetPipelineLayout = VK_NULL_HANDLE;
    VkPipelineLayout m_vkGraphicsPipelineLayout = VK_NULL_HANDLE;
    VkPipeline m_vkCullPipeline = VK_NULL_HANDLE;
    VkPipeline m_vkCompactDrawsPipeline = VK_NULL_HANDLE;
    VkPipeline m_vkCompactBindlessPipeline = VK_NULL_HANDLE;
    VkPipeline m_vkResetPipeline = VK_NULL_HANDLE;
    VkPipeline m_vkGraphicsPipeline = VK_NULL_HANDLE;
    VkDescriptorSet m_vkCullDescSet = VK_NULL_HANDLE;
    VkDescriptorSet m_vkCompactDrawsDescSet = VK_NULL_HANDLE;
    VkDescriptorSet m_vkCompactBindlessDescSet = VK_NULL_HANDLE;
    VkDescriptorSet m_vkResetDescSet = VK_NULL_HANDLE;
    VkDescriptorSet m_vkGraphicsDescSet = VK_NULL_HANDLE;

    Core::SceneBuffer* m_sceneBuffer = nullptr;
    Core::BufferPool<ObjectData>* m_objectDataPool = nullptr;
    Core::BufferPool<RenderableInfo>* m_renderableInfoPool = nullptr;
    Core::BufferPool<MeshInfo>* m_meshInfoPool = nullptr;
    Core::StagingBuffer* m_stagingBuffer = nullptr;

    Core::Texture* m_textures[ARRAY_OF_TEXTURES_SIZE] { nullptr };

    VulkanWrapper::Sampler* m_defaultSampler = nullptr;

    VulkanWrapper::Image* m_depthImage              = nullptr;
    VulkanWrapper::ImageView* m_depthImageView      = nullptr;
    VulkanWrapper::DeviceMemory* m_depthImageMemory = nullptr;

    VulkanWrapper::Buffer* m_ssboMeshInstanceCount      = nullptr;
    VulkanWrapper::Buffer* m_ssboActiveMesh             = nullptr;
    VulkanWrapper::Buffer* m_ssboVisibleRenderableInfos = nullptr;
    VulkanWrapper::Buffer* m_ssboVisibleRenderableCount = nullptr;
    VulkanWrapper::Buffer* m_ssboMeshToCommandMapping   = nullptr;
    VulkanWrapper::Buffer* m_ssboDrawCommands           = nullptr;
    VulkanWrapper::Buffer* m_ssboFirstInstanceCount     = nullptr;
    VulkanWrapper::Buffer* m_ssboDrawInfos              = nullptr;

    VulkanWrapper::DeviceMemory* m_ssboMeshInstanceCountMemory      = nullptr;
    VulkanWrapper::DeviceMemory* m_ssboActiveMeshMemory             = nullptr;
    VulkanWrapper::DeviceMemory* m_ssboVisibleRenderableInfosMemory = nullptr;
    VulkanWrapper::DeviceMemory* m_ssboVisibleRenderableCountMemory = nullptr;
    VulkanWrapper::DeviceMemory* m_ssboMeshToCommandMappingMemory   = nullptr;
    VulkanWrapper::DeviceMemory* m_ssboDrawCommandsMemory           = nullptr;
    VulkanWrapper::DeviceMemory* m_ssboFirstInstanceCountMemory     = nullptr;
    VulkanWrapper::DeviceMemory* m_ssboDrawInfosMemory              = nullptr;

    VkRenderPass m_vkImGuiRenderPass = VK_NULL_HANDLE;
    std::vector<VkFramebuffer> m_vkImGuiFramebuffers;
    VkDescriptorPool m_vkImguiDescriptorPool = VK_NULL_HANDLE;
};

void updateProjectionMatrix(AppCore& appCore)
{
    glm::mat4 newProjMat = glm::perspective(glm::radians(FOV), ((float)windowWidth) / windowHeight, nearZ, farZ);
    glm::mat4 viewMat;
    memcpy(&(viewMat[0][0]), (char*)appCore.m_globalUBO->getData() + offsetof(GlobalUBO, viewMatrix), sizeof(glm::mat4));
    glm::mat4 newProjViewMat = newProjMat * viewMat;

    appCore.m_globalUBO->update(offsetof(GlobalUBO, projMatrix), sizeof(glm::mat4), &(newProjMat[0][0]));
    appCore.m_globalUBO->update(offsetof(GlobalUBO, projViewMatrix), sizeof(glm::mat4), &(newProjViewMat[0][0]));
    globalUBODirty = true;
}

void updateViewMatrix(AppCore& appCore)
{
    glm::mat4 newViewMat = glm::mat4(1.0f);
    newViewMat = glm::translate(newViewMat, camPos);
    glm::mat4 newProjViewMat; 
    memcpy(&(newProjViewMat[0][0]), (char*)appCore.m_globalUBO->getData() + offsetof(GlobalUBO, projMatrix), sizeof(glm::mat4));
    newProjViewMat *= newViewMat;

    appCore.m_globalUBO->update(offsetof(GlobalUBO, viewMatrix), sizeof(glm::mat4), &(newViewMat[0][0]));
    appCore.m_globalUBO->update(offsetof(GlobalUBO, projViewMatrix), sizeof(glm::mat4), &(newProjViewMat[0][0]));
    globalUBODirty = true;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    switch (key)
    {
        case GLFW_KEY_ESCAPE:
            if (action == GLFW_PRESS)
                renderGui = !renderGui;
            break;
        default:
            break;
    };
}

uint32_t loadMesh(AppCore& appCore, const std::string_view& filename)
{
    static uint32_t meshID = 0;

    ObjectBufferData vertexIndexData = loadObjFile(filename);
    const MeshInfo meshInfo = appCore.m_sceneBuffer->queueUpload(meshID, sizeof(Vertex) * vertexIndexData.vertices.size(), (void*)vertexIndexData.vertices.data(), sizeof(uint32_t) * vertexIndexData.indices.size(), vertexIndexData.indices.data());

    const uint32_t meshInfoBlockIdx = (uint32_t)appCore.m_meshInfoPool->acquireBlock();
    MeshInfo& _meshInfo = appCore.m_meshInfoPool->getWritableBlock(meshInfoBlockIdx);
    _meshInfo = meshInfo;

    return meshID++;
}

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

VkDescriptorSet allocateDescSet(const VkDevice vk_device, const VkDescriptorPool vk_descPool, const VkDescriptorSetLayout vk_descSetLayout, void* pNext = nullptr)
{
    const VkDescriptorSetAllocateInfo vk_descSetAllocInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext = pNext,
        .descriptorPool = vk_descPool,
        .descriptorSetCount = 1,
        .pSetLayouts = &vk_descSetLayout
    };

    VkDescriptorSet vk_descSet = VK_NULL_HANDLE;
    VK_CHECK(vkAllocateDescriptorSets(vk_device, &vk_descSetAllocInfo, &vk_descSet));
    return vk_descSet;
} 


// INIT - PIPELINES


void createGraphicsPipelineLayout(const VulkanCore& vulkanCore, AppCore& appCore)
{
    const VkDescriptorBindingFlags vk_bindlessFlags {
        // The size of the binding will be determed upon descriptor set allocation. 
        VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT | 

        // If all shader innvocations do not accesses memory via descriptor X, then descriptor
        // X does not need to contain a valid descriptor at the time of consumption. This is 
        // true for all descriptors within said binding. 
        VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT
    };

    const VkDescriptorBindingFlags vk_descriptorBindingFlags[] {
        0x0,
        0x0,
        0x0,
        vk_bindlessFlags,
    };

    const VkDescriptorSetLayoutBindingFlagsCreateInfo vk_descriptorSetLayoutBindingFlagsCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO,
        .bindingCount = ARRAYSIZE(vk_descriptorBindingFlags),
        .pBindingFlags = vk_descriptorBindingFlags,
    };

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
        {   // Global Unbounded Array of Texturex
            .binding = 3,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = ARRAY_OF_TEXTURES_SIZE,
            .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
            .pImmutableSamplers = nullptr,
        },
    };

    const VkDescriptorSetLayoutCreateInfo vk_descSetLayoutCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = &vk_descriptorSetLayoutBindingFlagsCreateInfo,
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

void createGraphicsResources(const VulkanCore& vulkanCore, AppCore& appCore)
{
    createGraphicsPipelineLayout(vulkanCore, appCore);

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

    const VkPipelineDepthStencilStateCreateInfo vk_depthStencilStateCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .depthTestEnable = VK_TRUE,
        .depthWriteEnable = VK_TRUE,
        .depthCompareOp = VK_COMPARE_OP_LESS,
        .depthBoundsTestEnable = VK_FALSE,
        .stencilTestEnable = VK_FALSE,
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
        .pColorAttachmentFormats = &vulkanCore.vk_swapchainImageFormat,
        .depthAttachmentFormat = VK_FORMAT_D32_SFLOAT
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
        .pDepthStencilState = &vk_depthStencilStateCreateInfo,
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


    VkDescriptorSetVariableDescriptorCountAllocateInfo vk_descriptorSetVariableDescriptorCountAllocInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO_EXT,
        .descriptorSetCount = 1,
        .pDescriptorCounts = &ARRAY_OF_TEXTURES_SIZE,
    };

    appCore.m_vkGraphicsDescSet = allocateDescSet(vulkanCore.vk_device, appCore.m_descPool->m_vkDescPool, appCore.m_vkGraphicsDescSetLayout, (void*)&vk_descriptorSetVariableDescriptorCountAllocInfo);

    updateBufferDescriptorSet(vulkanCore.vk_device, appCore.m_vkGraphicsDescSet, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, { appCore.m_globalUBO->getBuffer(), 0, VK_WHOLE_SIZE });
    updateBufferDescriptorSet(vulkanCore.vk_device, appCore.m_vkGraphicsDescSet, 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, appCore.m_objectDataPool->getDescBufferInfo());
    updateBufferDescriptorSet(vulkanCore.vk_device, appCore.m_vkGraphicsDescSet, 2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, { appCore.m_ssboDrawInfos->vk_handle, 0, VK_WHOLE_SIZE });


    VkDescriptorImageInfo vk_descImageInfo[ACTIVE_TEXTURES];
    for (uint32_t i = 0; i < ACTIVE_TEXTURES; ++i)
    {
        vk_descImageInfo[i].sampler = appCore.m_defaultSampler->m_vkSampler;
        vk_descImageInfo[i].imageView =  appCore.m_textures[i]->getVkImageView();
        vk_descImageInfo[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    }

    VkWriteDescriptorSet vk_writeDescSet {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = appCore.m_vkGraphicsDescSet,
        .dstBinding = 3,
        .dstArrayElement = 0,
        .descriptorCount = ACTIVE_TEXTURES,
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .pImageInfo = vk_descImageInfo,
    };

    vkUpdateDescriptorSets(vulkanCore.vk_device, 1, &vk_writeDescSet, 0, nullptr);
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

void createCullResources(const VulkanCore& vulkanCore, AppCore& appCore)
{
    createCullPipelineLayout(vulkanCore, appCore);

    VulkanWrapper::ShaderModule* cullShaderModule = new VulkanWrapper::ShaderModule();
    cullShaderModule->create("../shaders/spirv/cull-comp.spv");

    const VkPipelineShaderStageCreateInfo vk_cullShaderStageCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_COMPUTE_BIT,
        .module = cullShaderModule->m_vkShaderModule,
        .pName = "main",
        .pSpecializationInfo = nullptr
    };

    const VkComputePipelineCreateInfo vk_cullPipelineCreateInfo {
        .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
        .stage = vk_cullShaderStageCreateInfo,
        .layout = appCore.m_vkCullPipelineLayout
    };

    VK_CHECK(vkCreateComputePipelines(vulkanCore.vk_device, VK_NULL_HANDLE, 1, &vk_cullPipelineCreateInfo, nullptr, &appCore.m_vkCullPipeline));

    delete cullShaderModule;

    appCore.m_ssboMeshInstanceCount       = new VulkanWrapper::Buffer(MAX_MESHES * sizeof(uint32_t), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
    appCore.m_ssboMeshInstanceCountMemory = new VulkanWrapper::DeviceMemory(appCore.m_ssboMeshInstanceCount->vk_handle, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    appCore.m_ssboMeshInstanceCountMemory->bind(appCore.m_ssboMeshInstanceCount->vk_handle, 0);

    appCore.m_ssboActiveMesh       = new VulkanWrapper::Buffer(MAX_MESHES * sizeof(uint32_t) + 3 * sizeof(uint32_t), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT);
    appCore.m_ssboActiveMeshMemory = new VulkanWrapper::DeviceMemory(appCore.m_ssboActiveMesh->vk_handle, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    appCore.m_ssboActiveMeshMemory->bind(appCore.m_ssboActiveMesh->vk_handle, 0);

    appCore.m_ssboVisibleRenderableInfos       = new VulkanWrapper::Buffer(MAX_RENDERABLES * sizeof(RenderableInfo) + 3 * sizeof(uint32_t), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT);
    appCore.m_ssboVisibleRenderableInfosMemory = new VulkanWrapper::DeviceMemory(appCore.m_ssboVisibleRenderableInfos->vk_handle, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    appCore.m_ssboVisibleRenderableInfosMemory->bind(appCore.m_ssboVisibleRenderableInfos->vk_handle, 0);

    appCore.m_ssboMeshToCommandMapping       = new VulkanWrapper::Buffer(MAX_MESHES * sizeof(MeshInfo), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
    appCore.m_ssboMeshToCommandMappingMemory = new VulkanWrapper::DeviceMemory(appCore.m_ssboMeshToCommandMapping->vk_handle, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    appCore.m_ssboMeshToCommandMappingMemory->bind(appCore.m_ssboMeshToCommandMapping->vk_handle, 0);
    
    appCore.m_vkCullDescSet = allocateDescSet(vulkanCore.vk_device, appCore.m_descPool->m_vkDescPool, appCore.m_vkCullDescSetLayout);

    updateBufferDescriptorSet(vulkanCore.vk_device, appCore.m_vkCullDescSet, 0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, appCore.m_renderableInfoPool->getDescBufferInfo());
    updateBufferDescriptorSet(vulkanCore.vk_device, appCore.m_vkCullDescSet, 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, { appCore.m_ssboMeshInstanceCount->vk_handle, 0, VK_WHOLE_SIZE });
    updateBufferDescriptorSet(vulkanCore.vk_device, appCore.m_vkCullDescSet, 2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, { appCore.m_ssboActiveMesh->vk_handle, 0, VK_WHOLE_SIZE });
    updateBufferDescriptorSet(vulkanCore.vk_device, appCore.m_vkCullDescSet, 3, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, { appCore.m_ssboMeshToCommandMapping->vk_handle, 0, VK_WHOLE_SIZE });
    updateBufferDescriptorSet(vulkanCore.vk_device, appCore.m_vkCullDescSet, 4, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, { appCore.m_ssboVisibleRenderableInfos->vk_handle, 0, VK_WHOLE_SIZE });
}


void createCompactDrawsPipelineLayout(const VulkanCore& vulkanCore, AppCore& appCore)
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
    };

    const VkDescriptorSetLayoutCreateInfo vk_descSetLayoutCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = ARRAYSIZE(vk_descSetLayoutBindings),
        .pBindings = vk_descSetLayoutBindings,
    };

    VK_CHECK(vkCreateDescriptorSetLayout(vulkanCore.vk_device, &vk_descSetLayoutCreateInfo, nullptr, &appCore.m_vkCompactDrawsDescSetLayout));

    const VkPipelineLayoutCreateInfo vk_pipelineLayoutCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 1,
        .pSetLayouts = &appCore.m_vkCompactDrawsDescSetLayout,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = nullptr
    };

    VK_CHECK(vkCreatePipelineLayout(vulkanCore.vk_device, &vk_pipelineLayoutCreateInfo, nullptr, &appCore.m_vkCompactDrawsPipelineLayout));
}

void createCompactDrawsResources(const VulkanCore& vulkanCore, AppCore& appCore)
{
    createCompactDrawsPipelineLayout(vulkanCore, appCore);

    VulkanWrapper::ShaderModule* compactShaderModule = new VulkanWrapper::ShaderModule();
    compactShaderModule->create("../shaders/spirv/compact-draws-comp.spv");

    const VkPipelineShaderStageCreateInfo vk_compactDrawsShaderStageCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_COMPUTE_BIT,
        .module = compactShaderModule->m_vkShaderModule,
        .pName = "main",
        .pSpecializationInfo = nullptr
    };

    const VkComputePipelineCreateInfo vk_compactPipelineCreateInfo {
        .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
        .stage = vk_compactDrawsShaderStageCreateInfo,
        .layout = appCore.m_vkCompactDrawsPipelineLayout
    };

    VK_CHECK(vkCreateComputePipelines(vulkanCore.vk_device, VK_NULL_HANDLE, 1, &vk_compactPipelineCreateInfo, nullptr, &appCore.m_vkCompactDrawsPipeline));

    delete compactShaderModule;

    appCore.m_ssboFirstInstanceCount       = new VulkanWrapper::Buffer(sizeof(uint32_t), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
    appCore.m_ssboFirstInstanceCountMemory = new VulkanWrapper::DeviceMemory(appCore.m_ssboFirstInstanceCount->vk_handle, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    appCore.m_ssboFirstInstanceCountMemory->bind(appCore.m_ssboFirstInstanceCount->vk_handle, 0);

    appCore.m_ssboDrawCommands       = new VulkanWrapper::Buffer(MAX_MESHES * sizeof(VkDrawIndexedIndirectCommand), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT);
    appCore.m_ssboDrawCommandsMemory = new VulkanWrapper::DeviceMemory(appCore.m_ssboDrawCommands->vk_handle, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    appCore.m_ssboDrawCommandsMemory->bind(appCore.m_ssboDrawCommands->vk_handle, 0);

    appCore.m_vkCompactDrawsDescSet = allocateDescSet(vulkanCore.vk_device, appCore.m_descPool->m_vkDescPool, appCore.m_vkCompactDrawsDescSetLayout);

    updateBufferDescriptorSet(vulkanCore.vk_device, appCore.m_vkCompactDrawsDescSet, 0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, { appCore.m_ssboActiveMesh->vk_handle, 0, VK_WHOLE_SIZE });
    updateBufferDescriptorSet(vulkanCore.vk_device, appCore.m_vkCompactDrawsDescSet, 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, { appCore.m_ssboMeshToCommandMapping->vk_handle, 0, VK_WHOLE_SIZE });
    updateBufferDescriptorSet(vulkanCore.vk_device, appCore.m_vkCompactDrawsDescSet, 2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, appCore.m_meshInfoPool->getDescBufferInfo());
    updateBufferDescriptorSet(vulkanCore.vk_device, appCore.m_vkCompactDrawsDescSet, 3, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, { appCore.m_ssboMeshInstanceCount->vk_handle, 0, VK_WHOLE_SIZE });
    updateBufferDescriptorSet(vulkanCore.vk_device, appCore.m_vkCompactDrawsDescSet, 4, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, { appCore.m_ssboFirstInstanceCount->vk_handle, 0, VK_WHOLE_SIZE });
    updateBufferDescriptorSet(vulkanCore.vk_device, appCore.m_vkCompactDrawsDescSet, 5, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, { appCore.m_ssboDrawCommands->vk_handle, 0, VK_WHOLE_SIZE });
}


void createCompactBindlessPipelineLayout(const VulkanCore& vulkanCore, AppCore& appCore)
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
    };

    const VkDescriptorSetLayoutCreateInfo vk_descSetLayoutCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = ARRAYSIZE(vk_descSetLayoutBindings),
        .pBindings = vk_descSetLayoutBindings,
    };

    VK_CHECK(vkCreateDescriptorSetLayout(vulkanCore.vk_device, &vk_descSetLayoutCreateInfo, nullptr, &appCore.m_vkCompactBindlessDescSetLayout));

    const VkPipelineLayoutCreateInfo vk_pipelineLayoutCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 1,
        .pSetLayouts = &appCore.m_vkCompactBindlessDescSetLayout,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = nullptr
    };

    VK_CHECK(vkCreatePipelineLayout(vulkanCore.vk_device, &vk_pipelineLayoutCreateInfo, nullptr, &appCore.m_vkCompactBindlessPipelineLayout));

}

void createCompactBindlessResources(const VulkanCore& vulkanCore, AppCore& appCore)
{
    createCompactBindlessPipelineLayout(vulkanCore, appCore);

    VulkanWrapper::ShaderModule* compactBindlessShaderModule = new VulkanWrapper::ShaderModule();
    compactBindlessShaderModule->create("../shaders/spirv/compact-bindless-comp.spv");

    const VkPipelineShaderStageCreateInfo vk_compactBindlessShaderStageCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_COMPUTE_BIT,
        .module = compactBindlessShaderModule->m_vkShaderModule,
        .pName = "main",
        .pSpecializationInfo = nullptr
    };

    const VkComputePipelineCreateInfo vk_compactBindlessPipelineCreateInfo {
        .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
        .stage = vk_compactBindlessShaderStageCreateInfo,
        .layout = appCore.m_vkCompactBindlessPipelineLayout
    };

    VK_CHECK(vkCreateComputePipelines(vulkanCore.vk_device, VK_NULL_HANDLE, 1, &vk_compactBindlessPipelineCreateInfo, nullptr, &appCore.m_vkCompactBindlessPipeline));

    delete compactBindlessShaderModule;

    appCore.m_ssboDrawInfos       = new VulkanWrapper::Buffer(MAX_RENDERABLES * sizeof(DrawInfo), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
    appCore.m_ssboDrawInfosMemory = new VulkanWrapper::DeviceMemory(appCore.m_ssboDrawInfos->vk_handle, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    appCore.m_ssboDrawInfosMemory->bind(appCore.m_ssboDrawInfos->vk_handle, 0);

    appCore.m_vkCompactBindlessDescSet = allocateDescSet(vulkanCore.vk_device, appCore.m_descPool->m_vkDescPool, appCore.m_vkCompactBindlessDescSetLayout);

    updateBufferDescriptorSet(vulkanCore.vk_device, appCore.m_vkCompactBindlessDescSet, 0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, { appCore.m_ssboVisibleRenderableInfos->vk_handle, 0, VK_WHOLE_SIZE });
    updateBufferDescriptorSet(vulkanCore.vk_device, appCore.m_vkCompactBindlessDescSet, 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, { appCore.m_ssboMeshToCommandMapping->vk_handle, 0, VK_WHOLE_SIZE });
    updateBufferDescriptorSet(vulkanCore.vk_device, appCore.m_vkCompactBindlessDescSet, 2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, { appCore.m_ssboDrawCommands->vk_handle, 0, VK_WHOLE_SIZE });
    updateBufferDescriptorSet(vulkanCore.vk_device, appCore.m_vkCompactBindlessDescSet, 3, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, { appCore.m_ssboMeshInstanceCount->vk_handle, 0, VK_WHOLE_SIZE });
    updateBufferDescriptorSet(vulkanCore.vk_device, appCore.m_vkCompactBindlessDescSet, 4, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, { appCore.m_ssboDrawInfos->vk_handle, 0, VK_WHOLE_SIZE });
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
        {
            .binding = 2,
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

    const VkPipelineLayoutCreateInfo vk_pipelineLayoutCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 1,
        .pSetLayouts = &appCore.m_vkResetDescSetLayout,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = nullptr
    };

    VK_CHECK(vkCreatePipelineLayout(vulkanCore.vk_device, &vk_pipelineLayoutCreateInfo, nullptr, &appCore.m_vkResetPipelineLayout));
}

void createResetResources(const VulkanCore& vulkanCore, AppCore& appCore)
{
    createResetPipelineLayout(vulkanCore, appCore);

    VulkanWrapper::ShaderModule* resetShaderModule = new VulkanWrapper::ShaderModule();
    resetShaderModule->create("../shaders/spirv/reset-comp.spv");

    const VkPipelineShaderStageCreateInfo vk_resetShaderStageCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_COMPUTE_BIT,
        .module = resetShaderModule->m_vkShaderModule,
        .pName = "main",
        .pSpecializationInfo = nullptr
    };

    const VkComputePipelineCreateInfo vk_resetPipelineCreateInfo {
        .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
        .stage = vk_resetShaderStageCreateInfo,
        .layout = appCore.m_vkResetPipelineLayout
    };

    VK_CHECK(vkCreateComputePipelines(vulkanCore.vk_device, VK_NULL_HANDLE, 1, &vk_resetPipelineCreateInfo, nullptr, &appCore.m_vkResetPipeline));

    delete resetShaderModule;

    appCore.m_vkResetDescSet = allocateDescSet(vulkanCore.vk_device, appCore.m_descPool->m_vkDescPool, appCore.m_vkResetDescSetLayout);

    updateBufferDescriptorSet(vulkanCore.vk_device, appCore.m_vkResetDescSet, 0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, { appCore.m_ssboFirstInstanceCount->vk_handle, 0, VK_WHOLE_SIZE });
    updateBufferDescriptorSet(vulkanCore.vk_device, appCore.m_vkResetDescSet, 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, { appCore.m_ssboVisibleRenderableInfos->vk_handle, 0, VK_WHOLE_SIZE });
    updateBufferDescriptorSet(vulkanCore.vk_device, appCore.m_vkResetDescSet, 2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, { appCore.m_ssboActiveMesh->vk_handle, 0, VK_WHOLE_SIZE });
}


// INIT - GUI


void createGuiRenderpass(const VulkanCore& vulkanCore, AppCore& appCore)
{
    // Create imgui resources (descriptor pool + render pass)

    const VkAttachmentDescription vk_attachmentDescription {
        .format = vulkanCore.vk_swapchainImageFormat,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    };

    const VkAttachmentReference vk_attachmentReference {
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };

    const VkSubpassDescription vk_subpassDescription {
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .inputAttachmentCount = 0,
        .pInputAttachments = nullptr,
        .colorAttachmentCount = 1,
        .pColorAttachments = &vk_attachmentReference,
        .pResolveAttachments = nullptr,
        .pDepthStencilAttachment = nullptr,
        .preserveAttachmentCount = 0,
        .pPreserveAttachments = nullptr
    };

    const VkSubpassDependency vk_subpassDeps [2] {
        {// First dependency at the start of the renderpass
            // Does the transition from final to initial layout
            .srcSubpass = VK_SUBPASS_EXTERNAL,                             // Producer of the dependency
            .dstSubpass = 0,                                               // Consumer is our single subpass that will wait for the execution dependency
            .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, // Match our pWaitDstStageMask when we vkQueueSubmit
            .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, // is a loadOp stage for color attachments
            .srcAccessMask = 0,                                            // semaphore wait already does memory dependency for us
            .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,         // is a loadOp CLEAR access mask for color attachments
            .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT},
        {// Second dependency at the end the renderpass
            // Does the transition from the initial to the final layout
            // Technically this is the same as the implicit subpass dependency, but we are gonna state it explicitly here
            .srcSubpass = 0,                                               // Producer of the dependency is our single subpass
            .dstSubpass = VK_SUBPASS_EXTERNAL,                             // Consumer are all commands outside of the renderpass
            .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, // is a storeOp stage for color attachments
            .dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,          // Do not block any subsequent work
            .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,         // is a storeOp `STORE` access mask for color attachments
            .dstAccessMask = 0,
            .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT
        }
    };

    const VkRenderPassCreateInfo vk_renderpassCreateInfo {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0x0,
        .attachmentCount = 1,
        .pAttachments = &vk_attachmentDescription,
        .subpassCount = 1,
        .pSubpasses = &vk_subpassDescription,
        .dependencyCount = 2,
        .pDependencies = vk_subpassDeps
    };

    VK_CHECK(vkCreateRenderPass(vulkanCore.vk_device, &vk_renderpassCreateInfo, nullptr, &appCore.m_vkImGuiRenderPass));
}

void createGuiFramebuffers(const VulkanCore& vulkanCore, AppCore& appCore)
{
    VkFramebufferCreateInfo vk_framebufferCreateInfo {
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .renderPass = appCore.m_vkImGuiRenderPass,
        .attachmentCount = 1,
        .width = vulkanCore.vk_swapchainExtent.width,
        .height = vulkanCore.vk_swapchainExtent.height,
        .layers = 1
    };

    appCore.m_vkImGuiFramebuffers.resize(vulkanCore.vk_swapchainImages.size());
    for (size_t i = 0; i < appCore.m_vkImGuiFramebuffers.size(); ++i)
    {
        const VkImageView vk_imageView[1] = { vulkanCore.vk_swapchainImageViews[i] };

        vk_framebufferCreateInfo.pAttachments = vk_imageView;

        VK_CHECK(vkCreateFramebuffer(vulkanCore.vk_device, &vk_framebufferCreateInfo, nullptr, &appCore.m_vkImGuiFramebuffers[i]));
    }
}

void createGuiDesciptorPool(const VulkanCore& vulkanCore, AppCore& appCore)
{
    // No idea how many descriptors imgui needs
    std::array<VkDescriptorPoolSize, 11> vk_descPoolSizes {{
        { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
    }};

    const VkDescriptorPoolCreateInfo vk_descPoolCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0x0,
        .maxSets = 2u,
        .poolSizeCount = static_cast<uint32_t>(vk_descPoolSizes.size()),
        .pPoolSizes = vk_descPoolSizes.data(),
    };

    VK_CHECK(vkCreateDescriptorPool(vulkanCore.vk_device, &vk_descPoolCreateInfo, nullptr, &appCore.m_vkImguiDescriptorPool));
}

void guiInit(GLFWwindow* glfw_window, const VulkanCore& vulkanCore, AppCore& appCore)
{
    createGuiRenderpass(vulkanCore, appCore);
    createGuiFramebuffers(vulkanCore, appCore);
    createGuiDesciptorPool(vulkanCore, appCore);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForVulkan(glfw_window, true);
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = vulkanCore.vk_instance;
    init_info.PhysicalDevice = vulkanCore.vk_physicalDevice;
    init_info.Device = vulkanCore.vk_device;
    init_info.QueueFamily = vulkanCore.graphicsQFamIdx;
    init_info.Queue = vulkanCore.vk_graphicsQ;
    init_info.PipelineCache = VK_NULL_HANDLE;
    init_info.DescriptorPool = appCore.m_vkImguiDescriptorPool;
    init_info.Subpass = 0;
    init_info.MinImageCount = 2;
    init_info.ImageCount = static_cast<uint32_t>(vulkanCore.vk_swapchainImages.size());
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    init_info.Allocator = nullptr;
    init_info.CheckVkResultFn = nullptr;
    ImGui_ImplVulkan_Init(&init_info, appCore.m_vkImGuiRenderPass);


    const VkCommandBufferBeginInfo command_buffer_begin_info {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
    };

    VK_CHECK(vkResetCommandPool(vulkanCore.vk_device, appCore.m_cmdPool->m_vkCmdPool, 0x0));
    VK_CHECK(vkBeginCommandBuffer(appCore.m_cmdBuff->m_vkCmdBuff, &command_buffer_begin_info));

    ImGui_ImplVulkan_CreateFontsTexture(appCore.m_cmdBuff->m_vkCmdBuff);

    VK_CHECK(vkEndCommandBuffer(appCore.m_cmdBuff->m_vkCmdBuff));

    const VkSubmitInfo vk_submitInfo {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = &appCore.m_cmdBuff->m_vkCmdBuff
    };

    VK_CHECK(vkQueueSubmit(vulkanCore.vk_graphicsQ, 1, &vk_submitInfo, VK_NULL_HANDLE));
    VK_CHECK(vkDeviceWaitIdle(vulkanCore.vk_device));
    ImGui_ImplVulkan_DestroyFontUploadObjects();
}


// INIT - GENERAL


void createAttachments(const VulkanCore& vulkanCore, AppCore& appCore)
{
    const VkImageCreateInfo vk_depthImageCreateInfo {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = VK_FORMAT_D32_SFLOAT,
        .extent = { vulkanCore.vk_swapchainExtent.width, vulkanCore.vk_swapchainExtent.height,1 },
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices   = nullptr,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    };

    appCore.m_depthImage = new VulkanWrapper::Image(vk_depthImageCreateInfo);
    appCore.m_depthImageMemory = new VulkanWrapper::DeviceMemory(appCore.m_depthImage->vk_handle, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    appCore.m_depthImageMemory->bind(appCore.m_depthImage->vk_handle, 0);

    const VkImageViewCreateInfo vk_imageViewCreateInfo {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = appCore.m_depthImage->vk_handle,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = VK_FORMAT_D32_SFLOAT,
        .components = {
            .r = VK_COMPONENT_SWIZZLE_IDENTITY,
            .g = VK_COMPONENT_SWIZZLE_IDENTITY,
            .b = VK_COMPONENT_SWIZZLE_IDENTITY,
            .a = VK_COMPONENT_SWIZZLE_IDENTITY },
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
        }
    };

    appCore.m_depthImageView = new VulkanWrapper::ImageView(vk_imageViewCreateInfo);
}

void loadMeshes(const VulkanCore& vulkanCore, AppCore& appCore)
{
    // const uint32_t meshSphereID = loadMesh(appCore, "../obj-files/sphere.obj");
    // const uint32_t meshSphereID = loadMesh(appCore, "../obj-files/low_res_isosphere.obj");
    // const uint32_t meshTorusID = loadMesh(appCore, "../obj-files/cube.obj");
    // const uint32_t meshTorusID = loadMesh(appCore, "../obj-files/torus.obj");
    // const uint32_t meshMonkeyID = loadMesh(appCore, "../obj-files/monkey.obj");
    // const uint32_t meshMonkeyID = loadMesh(appCore, "../obj-files/low_res_cylinder.obj");
    const uint32_t meshPlaneID = loadMesh(appCore, "../obj-files/plane.obj");

    const int dim = DIM;
    const int halfDim = dim / 2;
    for (int x = 0; x < dim; x++)
    {
        for (int y = 0; y < dim; y++)
        {
            // const int y_mod = y % 3;
            // uint32_t meshID = UINT32_MAX;
            // switch (y_mod)
            // {
            // case 0:
            //     meshID = meshMonkeyID;
            //     break;
            // case 1:
            //     meshID = meshTorusID;
            //     break;
            // case 2:
            //     meshID = meshSphereID;
            //     break;
            // }
            uint meshID = meshPlaneID;

            for (int z = 0; z < dim; z++)
            {
                const uint32_t objectDataBlockIdx = (uint32_t)appCore.m_objectDataPool->acquireBlock();
                ObjectData& objectData = appCore.m_objectDataPool->getWritableBlock(objectDataBlockIdx);
                objectData.modelMatrix = glm::mat4(1.0f);
                objectData.modelMatrix = glm::translate(objectData.modelMatrix, glm::vec3(x - halfDim, y - halfDim, z - halfDim));
                objectData.modelMatrix = glm::rotate(objectData.modelMatrix, glm::half_pi<float>(), glm::vec3(1.0, 0.0, 0.0)); // glm::vec3(glm::half_pi<double>(), 0.0f, 0.0f));
                objectData.modelMatrix = glm::scale(objectData.modelMatrix, glm::vec3(0.1, 0.1, 0.1));

                const uint32_t renderableInfoBlockIdx = (uint32_t)appCore.m_renderableInfoPool->acquireBlock();
                RenderableInfo& renderableInfo = appCore.m_renderableInfoPool->getWritableBlock(renderableInfoBlockIdx);
                renderableInfo.meshID = meshID;
                renderableInfo.objectID = objectDataBlockIdx;
                LOG("renderable ID %u\n", objectDataBlockIdx);
            }
        }
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

    const VkSamplerCreateInfo vk_samplerCreateInfo {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .magFilter = VK_FILTER_LINEAR,
        .minFilter = VK_FILTER_LINEAR,
        .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
        .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .mipLodBias = 0.0,
        .anisotropyEnable = VK_FALSE,
        .compareEnable = VK_FALSE,
        .minLod = 0.0f,
        .maxLod = 0.0f,
        .unnormalizedCoordinates = VK_FALSE,
    };

    appCore.m_defaultSampler = new VulkanWrapper::Sampler(vk_samplerCreateInfo);

    appCore.m_stagingBuffer = new Core::StagingBuffer((1 << 20) * 50, 1);

    appCore.m_sceneBuffer = new Core::SceneBuffer(sizeof(Vertex), 5000000, 2000000, 5000000);

    appCore.m_objectDataPool     = new Core::BufferPool<ObjectData>(MAX_RENDERABLES, MAX_RENDERABLES);
    appCore.m_renderableInfoPool = new Core::BufferPool<RenderableInfo>(MAX_RENDERABLES, MAX_RENDERABLES); 
    appCore.m_meshInfoPool       = new Core::BufferPool<MeshInfo>(MAX_MESHES, MAX_MESHES); 

    appCore.m_descPool = new VulkanWrapper::DescriptorPool();
    appCore.m_descPool->create(5, {{
                            .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                            .descriptorCount = 1
                        },
                        {
                            .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                            .descriptorCount = 20
                        },
                        {
                            .type = VK_DESCRIPTOR_TYPE_SAMPLER,
                            .descriptorCount = 1
                        },
                        {
                            .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                            .descriptorCount = 1
                        },
                        
                        });

    appCore.m_persistentStagingBuffer = new PersistentStagingBuffer();
    appCore.m_globalUBO = appCore.m_persistentStagingBuffer->registerDeviceBuffer(sizeof(GlobalUBO), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    appCore.m_persistentStagingBuffer->completeRegistration();

    appCore.m_textures[0] = loadTexture("../textures/container.ktx", appCore.m_stagingBuffer, appCore.m_defaultSampler);
    appCore.m_textures[1] = loadTexture("../textures/awesomeface.ktx", appCore.m_stagingBuffer, appCore.m_defaultSampler);
    appCore.m_textures[2] = loadTexture("../textures/wall.ktx", appCore.m_stagingBuffer, appCore.m_defaultSampler);
    appCore.m_textures[3] = loadTexture("../textures/statue.ktx", appCore.m_stagingBuffer, appCore.m_defaultSampler);

    glm::mat4 projMat = glm::perspective(FOV, ((float)windowWidth) / windowHeight, nearZ, farZ);
    glm::mat4 viewMat = glm::mat4(1.0f);
    viewMat = glm::translate(viewMat, camPos);
    glm::mat4 projViewMat = projMat * viewMat;
    appCore.m_globalUBO->update(offsetof(GlobalUBO, projMatrix), sizeof(glm::mat4), &(projMat[0][0]));
    appCore.m_globalUBO->update(offsetof(GlobalUBO, viewMatrix), sizeof(glm::mat4), &(viewMat[0][0]));
    appCore.m_globalUBO->update(offsetof(GlobalUBO, projViewMatrix), sizeof(glm::mat4), &(projViewMat[0][0]));

    createAttachments(vulkanCore, appCore);
    loadMeshes(vulkanCore, appCore);

    createCullResources(vulkanCore, appCore);
    createCompactDrawsResources(vulkanCore, appCore);
    createCompactBindlessResources(vulkanCore, appCore);
    createResetResources(vulkanCore, appCore);
    createGraphicsResources(vulkanCore, appCore);

#ifdef DEBUG
    DebugUtilsEXT::setObjectName(VK_OBJECT_TYPE_BUFFER, (uint64_t)appCore.m_sceneBuffer->getVertexBuffer(), "Vertex Buffer");
    DebugUtilsEXT::setObjectName(VK_OBJECT_TYPE_BUFFER, (uint64_t)appCore.m_sceneBuffer->getIndexBuffer(), "Index Buffer");
    DebugUtilsEXT::setObjectName(VK_OBJECT_TYPE_BUFFER, (uint64_t)appCore.m_objectDataPool->getStagingBufferHandle(), "BufferPool - ObjectData - Staging");
    DebugUtilsEXT::setObjectName(VK_OBJECT_TYPE_BUFFER, (uint64_t)appCore.m_objectDataPool->getStorageBufferHandle(), "BufferPool - ObjectData - Storage");
    DebugUtilsEXT::setObjectName(VK_OBJECT_TYPE_BUFFER, (uint64_t)appCore.m_renderableInfoPool->getStagingBufferHandle(), "BufferPool - RenderableInfo - Staging");
    DebugUtilsEXT::setObjectName(VK_OBJECT_TYPE_BUFFER, (uint64_t)appCore.m_renderableInfoPool->getStorageBufferHandle(), "BufferPool - RenderableInfo - Storage");
    DebugUtilsEXT::setObjectName(VK_OBJECT_TYPE_BUFFER, (uint64_t)appCore.m_ssboVisibleRenderableInfos->vk_handle, "VisibleRenderableInfos");
    DebugUtilsEXT::setObjectName(VK_OBJECT_TYPE_BUFFER, (uint64_t)appCore.m_ssboDrawCommands->vk_handle, "DrawCommands");
    DebugUtilsEXT::setObjectName(VK_OBJECT_TYPE_BUFFER, (uint64_t)appCore.m_ssboDrawInfos->vk_handle, "DrawInfos");
    DebugUtilsEXT::setObjectName(VK_OBJECT_TYPE_BUFFER, (uint64_t)appCore.m_ssboFirstInstanceCount->vk_handle, "FirstInstanceCount");
    DebugUtilsEXT::setObjectName(VK_OBJECT_TYPE_BUFFER, (uint64_t)appCore.m_ssboMeshToCommandMapping->vk_handle, "MeshToCommandMapptig");
    DebugUtilsEXT::setObjectName(VK_OBJECT_TYPE_BUFFER, (uint64_t)appCore.m_ssboMeshInstanceCount->vk_handle, "MeshInstanceCount");
    DebugUtilsEXT::setObjectName(VK_OBJECT_TYPE_BUFFER, (uint64_t)appCore.m_meshInfoPool->getStagingBufferHandle(), "MeshInfoStaging");
    DebugUtilsEXT::setObjectName(VK_OBJECT_TYPE_BUFFER, (uint64_t)appCore.m_meshInfoPool->getStorageBufferHandle(), "MeshInfoStorage");
    DebugUtilsEXT::setObjectName(VK_OBJECT_TYPE_BUFFER, (uint64_t)appCore.m_globalUBO->getBuffer(), "GlobalUBO");
    DebugUtilsEXT::setObjectName(VK_OBJECT_TYPE_BUFFER, (uint64_t)appCore.m_ssboActiveMesh->vk_handle, "ActiveMesh");
#endif

    constexpr VkCommandBufferBeginInfo vk_cmdBeginInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    };

    VK_CHECK(vkResetCommandPool(vulkanCore.vk_device, appCore.m_cmdPool->m_vkCmdPool, 0x0));
    VK_CHECK(vkBeginCommandBuffer(appCore.m_cmdBuff->m_vkCmdBuff, &vk_cmdBeginInfo));

    appCore.m_sceneBuffer->flushQueuedUploads(appCore.m_cmdBuff->m_vkCmdBuff, true);
    appCore.m_objectDataPool->flushDirtyBlocks(appCore.m_cmdBuff->m_vkCmdBuff, true);
    appCore.m_renderableInfoPool->flushDirtyBlocks(appCore.m_cmdBuff->m_vkCmdBuff, true);
    appCore.m_meshInfoPool->flushDirtyBlocks(appCore.m_cmdBuff->m_vkCmdBuff, true);
    appCore.m_globalUBO->flush(appCore.m_cmdBuff->m_vkCmdBuff);
    appCore.m_stagingBuffer->flush(appCore.m_cmdBuff->m_vkCmdBuff);

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


// RUNTIME


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

    uint32_t renderableCount = CURRENT_RENDERABLES;
    vkCmdPushConstants(appCore.m_cmdBuff->m_vkCmdBuff, appCore.m_vkCullPipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(uint32_t), &renderableCount);

    vkCmdDispatch(appCore.m_cmdBuff->m_vkCmdBuff, renderableCount, 1, 1);

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

void appIndirectCompactDraws(const VulkanCore& vulkanCore, AppCore& appCore)
{
    constexpr VkCommandBufferBeginInfo vk_cmdBeginInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    };

    VK_CHECK(vkResetCommandPool(vulkanCore.vk_device, appCore.m_cmdPool->m_vkCmdPool, 0x0));
    VK_CHECK(vkBeginCommandBuffer(appCore.m_cmdBuff->m_vkCmdBuff, &vk_cmdBeginInfo));

    vkCmdBindPipeline(appCore.m_cmdBuff->m_vkCmdBuff, VK_PIPELINE_BIND_POINT_COMPUTE, appCore.m_vkCompactDrawsPipeline);
    
    vkCmdBindDescriptorSets(appCore.m_cmdBuff->m_vkCmdBuff, VK_PIPELINE_BIND_POINT_COMPUTE, appCore.m_vkCompactDrawsPipelineLayout, 0, 1, &appCore.m_vkCompactDrawsDescSet, 0, nullptr);

    vkCmdDispatchIndirect(appCore.m_cmdBuff->m_vkCmdBuff, appCore.m_ssboActiveMesh->vk_handle, 0);

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

void appIndirectCompactBindless(const VulkanCore& vulkanCore, AppCore& appCore)
{
    constexpr VkCommandBufferBeginInfo vk_cmdBeginInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    };

    VK_CHECK(vkResetCommandPool(vulkanCore.vk_device, appCore.m_cmdPool->m_vkCmdPool, 0x0));
    VK_CHECK(vkBeginCommandBuffer(appCore.m_cmdBuff->m_vkCmdBuff, &vk_cmdBeginInfo));

    vkCmdBindPipeline(appCore.m_cmdBuff->m_vkCmdBuff, VK_PIPELINE_BIND_POINT_COMPUTE, appCore.m_vkCompactBindlessPipeline);
    
    vkCmdBindDescriptorSets(appCore.m_cmdBuff->m_vkCmdBuff, VK_PIPELINE_BIND_POINT_COMPUTE, appCore.m_vkCompactBindlessPipelineLayout, 0, 1, &appCore.m_vkCompactBindlessDescSet, 0, nullptr);

    vkCmdDispatchIndirect(appCore.m_cmdBuff->m_vkCmdBuff, appCore.m_ssboVisibleRenderableInfos->vk_handle, 0);

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

void gui(const VulkanCore& vulkanCore, AppCore& appCore)
{
    static const VkCommandBufferBeginInfo vk_cmdBuffBeginInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
    };

    VkRenderPassBeginInfo vk_renderPassBeginInfo {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = appCore.m_vkImGuiRenderPass,
        .framebuffer = appCore.m_vkImGuiFramebuffers[appCore.m_uActiveSwapchainImageIdx],
        .renderArea = {
            .offset = {.x = 0, .y = 0},
            .extent = vulkanCore.vk_swapchainExtent
        },
    };

    vkCmdBeginRenderPass(appCore.m_cmdBuff->m_vkCmdBuff, &vk_renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    // Start the Dear ImGui frame
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (ImGui::Begin("Gui"))
    {
        if (ImGui::CollapsingHeader("Camera"))
        {
            if (ImGui::SliderFloat("FOV", &FOV, 1.0, 89.0, "%.1f"))
            {
                updateProjectionMatrix(appCore);
            }

            if (ImGui::SliderFloat("Near Z Plane", &nearZ, 0.01, 1.9, "%.1f"))
            {
                updateProjectionMatrix(appCore);
            }

            if(ImGui::SliderFloat("Far Z Plane", &farZ, 2.0, 1000.0, "%.1f"))
            {
                updateProjectionMatrix(appCore);
            }

            if (ImGui::InputFloat3("Camera Position", &camPos[0]))
            {
                updateViewMatrix(appCore);
            }
        }
        if (ImGui::CollapsingHeader("Texture"))
        {
            if (ImGui::SliderInt("Texture ID", &activeTextureID, 0, ACTIVE_TEXTURES - 1))
            {
                uint32_t id = static_cast<uint32_t>(activeTextureID);
                appCore.m_globalUBO->update(offsetof(GlobalUBO, texID), sizeof(uint32_t), &id);
                globalUBODirty = true;
            }
        }

    }
    ImGui::End();

    ImGui::Render();
    ImDrawData* draw_data = ImGui::GetDrawData();
    ImGui_ImplVulkan_RenderDrawData(draw_data, appCore.m_cmdBuff->m_vkCmdBuff);

    vkCmdEndRenderPass(appCore.m_cmdBuff->m_vkCmdBuff);
}

void appRender(const VulkanCore& vulkanCore, AppCore& appCore)
{
    VK_CHECK(vkAcquireNextImageKHR(vulkanCore.vk_device, vulkanCore.vk_swapchain, UINT64_MAX, VK_NULL_HANDLE, appCore.m_swapchainImageAcquireFence->m_vkFence, &appCore.m_uActiveSwapchainImageIdx));
    VK_CHECK(vkWaitForFences(vulkanCore.vk_device, 1, &appCore.m_swapchainImageAcquireFence->m_vkFence, VK_TRUE, UINT64_MAX));
    VK_CHECK(vkResetFences(vulkanCore.vk_device, 1, &appCore.m_swapchainImageAcquireFence->m_vkFence));

    appIndirectCull(vulkanCore, appCore);
    appIndirectCompactDraws(vulkanCore, appCore);
    appIndirectCompactBindless(vulkanCore, appCore);

    const VkRenderingAttachmentInfo vk_baseColorAttachmentInfo {
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .imageView = vulkanCore.vk_swapchainImageViews[appCore.m_uActiveSwapchainImageIdx],
        .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .clearValue = { .color = { 0.2f, 0.2f, 0.2f, 1.0f } }
    };

    const VkRenderingAttachmentInfo vk_depthAttachmentInfo {
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .imageView = appCore.m_depthImageView->m_vkImageView,
        .imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .clearValue = { .depthStencil = { 1.0f, 0u } }
    };

    const VkRenderingInfo vk_renderingInfo {
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
        .renderArea = { .offset = { 0, 0 }, .extent = { vulkanCore.vk_swapchainExtent.width, vulkanCore.vk_swapchainExtent.height } },
        .layerCount = 1,
        .colorAttachmentCount = 1,
        .pColorAttachments = &vk_baseColorAttachmentInfo,
        .pDepthAttachment = &vk_depthAttachmentInfo,
        .pStencilAttachment = nullptr,
    };

    const VkCommandBufferBeginInfo vk_cmdBuffBeginInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
    };

    VkCommandBuffer vk_cmdBuff = appCore.m_cmdBuff->m_vkCmdBuff;

    vkResetCommandPool(vulkanCore.vk_device, appCore.m_cmdPool->m_vkCmdPool, 0x0);

    VK_CHECK(vkBeginCommandBuffer(vk_cmdBuff, &vk_cmdBuffBeginInfo));

    // TODO - need to make this a dirty queue flush
    if (globalUBODirty)
    {
        appCore.m_globalUBO->flush(vk_cmdBuff);
        globalUBODirty = false;
    }

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

    vkCmdDrawIndexedIndirectCount(vk_cmdBuff, appCore.m_ssboDrawCommands->vk_handle, 0, appCore.m_ssboActiveMesh->vk_handle, 0, MAX_MESHES, sizeof(VkDrawIndexedIndirectCommand));

    vkCmdEndRendering(vk_cmdBuff);

    if (renderGui)
    {
        gui(vulkanCore, appCore);
    }
    else
    {
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
    }


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

    appIndirectReset(vulkanCore, appCore);
}

// TEARDOWN

void appCleanup(const VulkanCore& vulkanCore, AppCore& appCore)
{
    delete appCore.m_cmdBuff;
    delete appCore.m_cmdPool;
    delete appCore.m_swapchainImageAcquireFence;
    delete appCore.m_descPool;

    for (uint32_t i = 0; i < ARRAY_OF_TEXTURES_SIZE; ++i)
        delete appCore.m_textures[i];

    delete appCore.m_defaultSampler;
    delete appCore.m_stagingBuffer;

    delete appCore.m_sceneBuffer;
    delete appCore.m_objectDataPool;
    delete appCore.m_renderableInfoPool;
    delete appCore.m_meshInfoPool;

    delete appCore.m_globalUBO;
    delete appCore.m_persistentStagingBuffer;

    delete appCore.m_ssboVisibleRenderableInfos;
    delete appCore.m_ssboVisibleRenderableCount;
    delete appCore.m_ssboMeshToCommandMapping;
    delete appCore.m_ssboDrawCommands;
    delete appCore.m_ssboMeshInstanceCount;
    delete appCore.m_ssboFirstInstanceCount;
    delete appCore.m_ssboDrawInfos;
    delete appCore.m_ssboActiveMesh;

    delete appCore.m_ssboMeshInstanceCountMemory     ;
    delete appCore.m_ssboActiveMeshMemory            ;
    delete appCore.m_ssboVisibleRenderableInfosMemory;
    delete appCore.m_ssboVisibleRenderableCountMemory;
    delete appCore.m_ssboMeshToCommandMappingMemory  ;
    delete appCore.m_ssboDrawCommandsMemory          ;
    delete appCore.m_ssboFirstInstanceCountMemory    ;
    delete appCore.m_ssboDrawInfosMemory             ;

    delete appCore.m_depthImage;
    delete appCore.m_depthImageMemory;
    delete appCore.m_depthImageView;

    vkDestroyDescriptorSetLayout(vulkanCore.vk_device, appCore.m_vkCullDescSetLayout, nullptr);
    vkDestroyDescriptorSetLayout(vulkanCore.vk_device, appCore.m_vkCompactDrawsDescSetLayout, nullptr);
    vkDestroyDescriptorSetLayout(vulkanCore.vk_device, appCore.m_vkCompactBindlessDescSetLayout, nullptr);
    vkDestroyDescriptorSetLayout(vulkanCore.vk_device, appCore.m_vkResetDescSetLayout, nullptr);
    vkDestroyDescriptorSetLayout(vulkanCore.vk_device, appCore.m_vkGraphicsDescSetLayout, nullptr);

    vkDestroyPipelineLayout(vulkanCore.vk_device, appCore.m_vkCullPipelineLayout, nullptr);
    vkDestroyPipelineLayout(vulkanCore.vk_device, appCore.m_vkCompactDrawsPipelineLayout, nullptr);
    vkDestroyPipelineLayout(vulkanCore.vk_device, appCore.m_vkCompactBindlessPipelineLayout, nullptr);
    vkDestroyPipelineLayout(vulkanCore.vk_device, appCore.m_vkResetPipelineLayout, nullptr);
    vkDestroyPipelineLayout(vulkanCore.vk_device, appCore.m_vkGraphicsPipelineLayout, nullptr);

    vkDestroyPipeline(vulkanCore.vk_device, appCore.m_vkCullPipeline, nullptr);
    vkDestroyPipeline(vulkanCore.vk_device, appCore.m_vkCompactDrawsPipeline, nullptr);
    vkDestroyPipeline(vulkanCore.vk_device, appCore.m_vkCompactBindlessPipeline, nullptr);
    vkDestroyPipeline(vulkanCore.vk_device, appCore.m_vkResetPipeline, nullptr);
    vkDestroyPipeline(vulkanCore.vk_device, appCore.m_vkGraphicsPipeline, nullptr);

    vkDestroyRenderPass(vulkanCore.vk_device, appCore.m_vkImGuiRenderPass, nullptr);
    vkDestroyDescriptorPool(vulkanCore.vk_device, appCore.m_vkImguiDescriptorPool, nullptr);

    for (uint32_t i = 0; i < appCore.m_vkImGuiFramebuffers.size(); ++i)
        vkDestroyFramebuffer(vulkanCore.vk_device, appCore.m_vkImGuiFramebuffers[i], nullptr);
}

void guiCleanup()
{
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}


int main()
{
    const char* windowName = "Engine";
    bool continueExecuting = true;

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    GLFWwindow* glfw_window = glfwCreateWindow(windowWidth, windowHeight, windowName, nullptr, nullptr);
    assert(glfw_window && "Failed to create window");
    glfwSetKeyCallback(glfw_window, keyCallback);

    const VkPhysicalDeviceVulkan12Features vk_physicalDeviceFeatures12 {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
        .drawIndirectCount = VK_TRUE,
        .shaderSampledImageArrayNonUniformIndexing = VK_TRUE,
        .descriptorBindingPartiallyBound = VK_TRUE,
        .descriptorBindingVariableDescriptorCount = VK_TRUE,
        .runtimeDescriptorArray = VK_TRUE,
    };

    const VkPhysicalDeviceVulkan13Features vk_physicalDeviceFeatures13 {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
        .pNext = (void*)&vk_physicalDeviceFeatures12,
        .synchronization2 = VK_TRUE,
        .dynamicRendering = VK_TRUE
    };

    const VulkanInitInfo vulkanInitInfo {
        .window = glfw_window,
#ifdef DEBUG
        .requestedInstanceLayerNames = { "VK_LAYER_KHRONOS_validation" },
#endif
        .requestedInstanceExtensionNames = { "VK_KHR_surface", "VK_KHR_xcb_surface", VK_EXT_DEBUG_UTILS_EXTENSION_NAME },
        .requestedDeviceExtensionNames = { VK_KHR_SWAPCHAIN_EXTENSION_NAME, "VK_KHR_dynamic_rendering", VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME },
        .requestedSwapchainImageCount = 3,
        .requestedSwapchainImageFormat = VK_FORMAT_R8G8B8_SRGB,
        .requestedSwapchainImageExtent = { windowWidth, windowHeight },
        .requestedSwapchainImagePresentMode = VK_PRESENT_MODE_FIFO_KHR,
        .pDeviceFeatures = (void*)(&vk_physicalDeviceFeatures13)
    };

    VulkanCore vulkanCore = initVulkan(vulkanInitInfo);

    VulkanWrapper::init(vulkanCore.vk_physicalDevice, vulkanCore.vk_device);
    VulkanWrapper::Resource::initResources(vulkanCore.vk_physicalDevice, vulkanCore.vk_device);
    DebugUtilsEXT::init(vulkanCore.vk_instance, vulkanCore.vk_device);


    VkFormatProperties vk_formatProperties;

    AppCore appCore {};
    appIndirectInit(vulkanCore, appCore);

    guiInit(glfw_window, vulkanCore, appCore);

    while (!glfwWindowShouldClose(glfw_window))
    {
        glfwPollEvents();

        appRender(vulkanCore, appCore);

        appSubmit(vulkanCore, appCore);
    }

    VK_CHECK(vkDeviceWaitIdle(vulkanCore.vk_device));

    guiCleanup();
    appCleanup(vulkanCore, appCore);
    cleanupVulkan(vulkanCore);

    glfwDestroyWindow(glfw_window);
    glfwTerminate();
}