#include <vector>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "Defines.hpp"
#include "VulkanCore.hpp"
#include "core/CommandBuffer.hpp"
#include "core/CommandPool.hpp"
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
frag = processShader() // get reflection info

// a program is a valid collection of shaders
// pipeline layout is formed by the union of the two shaders
program = createProgram(vert, frag)

// a pipeline is a program + some additional state

program is the pipeline layout formed by the union of all the reflection info
*/

#define BINDLESS_VERTEX_BUFFER

struct Vertex
{
};

struct Model
{

    uint32_t indexCount;
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

    VkPipelineLayout m_vkPipelineLayout = VK_NULL_HANDLE;
    VkPipeline m_vkPipeline = VK_NULL_HANDLE;
};

void appInit(const VulkanCore& vulkanCore, AppCore& appCore)
{
    CommandPool* cmdPool = new CommandPool();
    cmdPool->create(0x0, vulkanCore.graphicsQFamIdx);

    CommandBuffer* cmdBuff = new CommandBuffer();
    cmdBuff->create(cmdPool->m_vkCmdPool);

    Fence* fence = new Fence();
    fence->create(0x0);

    SceneBuffer* sceneBuffer = new SceneBuffer(sizeof(Vertex), 5000000, 2000000, 5000000);

    const VkPipelineLayoutCreateInfo vk_pipelineLayoutCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 0,
        .pSetLayouts = nullptr,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = nullptr,
    };

    VkPipelineLayout vk_pipelineLayout = VK_NULL_HANDLE;
    VK_CHECK(vkCreatePipelineLayout(vulkanCore.vk_device, &vk_pipelineLayoutCreateInfo, nullptr, &vk_pipelineLayout));

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

#ifdef BINDLESS_VERTEX_BUFFER
    const VkPipelineVertexInputStateCreateInfo vk_vertexInputStateCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = 0,
        .pVertexBindingDescriptions = 0,
        .vertexAttributeDescriptionCount = 0,
        .pVertexAttributeDescriptions = 0,
    };
#else
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
            .location = 0,
            .binding = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
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
#endif

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
        .layout = vk_pipelineLayout,
        .renderPass = VK_NULL_HANDLE,
        .subpass = 0,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = 0,
    };

    VkPipeline vk_pipeline = VK_NULL_HANDLE;
    VK_CHECK(vkCreateGraphicsPipelines(vulkanCore.vk_device, VK_NULL_HANDLE, 1, &vk_graphicsPipelineCreateInfo, nullptr, &vk_pipeline));

    appCore.m_cmdPool = cmdPool;
    appCore.m_cmdBuff = cmdBuff;
    appCore.m_swapchainImageAcquireFence = fence;
    appCore.m_vkPipelineLayout = vk_pipelineLayout;
    appCore.m_vkPipeline = vk_pipeline;
    appCore.m_sceneBuffer = sceneBuffer;

    delete fragmentShaderModule;
    delete vertexShaderModule;
}

void appCleanup(const VulkanCore& vulkanCore, AppCore& appCore)
{
    delete appCore.m_cmdBuff;
    delete appCore.m_cmdPool;
    delete appCore.m_swapchainImageAcquireFence;
    delete appCore.m_sceneBuffer;

    vkDestroyPipelineLayout(vulkanCore.vk_device, appCore.m_vkPipelineLayout, nullptr);
    vkDestroyPipeline(vulkanCore.vk_device, appCore.m_vkPipeline, nullptr);
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

    // vkCmdDrawIndexed(vk_cmdBuff,  )

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
        .requestedDeviceExtensionNames = { VK_KHR_SWAPCHAIN_EXTENSION_NAME }, //, "VK_KHR_dynamic_rendering" },
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