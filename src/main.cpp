#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "Defines.hpp"

#include "VulkanCore.hpp"
#include "RenderPlan.hpp"

#include "vulkanwrapper/CommandBuffer.hpp"
#include "vulkanwrapper/CommandPool.hpp"
#include "vulkanwrapper/DescriptorPool.hpp"
#include "vulkanwrapper/DescriptorSet.hpp"
#include "vulkanwrapper/DescriptorSetLayout.hpp"

#include "core/BufferPool.hpp"

#include "debug-utils/DebugUtilsEXT.hpp"


struct MatData
{
    float x;
};

struct DrawData
{
    uint32_t matID;
};

// struct DrawInfo
// {
//     float x;
// };

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

VulkanWrapper::DescriptorPool createDescriptorPool();
VulkanWrapper::DescriptorSetLayout createDescriptorSetLayout();
VulkanWrapper::DescriptorSet allocateDescriptorSet(const VkDescriptorPool vk_pool, const VkDescriptorSetLayout vk_layout, void* pNext);

constexpr uint32_t WINDOW_WIDTH         = 300;
constexpr uint32_t WINDOW_HEIGHT        = 300;

constexpr uint32_t MAX_RENDERABLE_COUNT = 1;
constexpr uint32_t MAX_MATERIAL_COUNT   = 1;
constexpr uint32_t MAX_PIPELINE_COUNT   = 1;
constexpr uint32_t MAX_TEXTURES         = 1;

constexpr VkDescriptorSetVariableDescriptorCountAllocateInfo vk_descriptorSetVariableDescriptorCountAllocInfo {
    .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO_EXT,
    .descriptorSetCount = 1,
    .pDescriptorCounts = &MAX_TEXTURES,
};

void kickoff(GLFWwindow* glfw_window, const VulkanCore& vulkanCore)
{
    const RenderPlan::InitInfo rendererInitInfo {
        .plan = RenderPlanType::eForward,
        .vk_sampleCount = VK_SAMPLE_COUNT_1_BIT,
        .vk_attachmentFormats = { 
            VK_FORMAT_R8G8B8A8_UNORM, // DEFUALT COLOR
            VK_FORMAT_D32_SFLOAT    // DEPTH
        },
        .vk_clearValues = { 
            { .color = { 0.0, 0.0, 0.0, 0.0 } },
            { .depthStencil = { 0.0, 0u }     }
        },
        .vk_framebufferDimensions = vulkanCore.vk_swapchainExtent
    };

    RenderPlan renderer(rendererInitInfo);

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

    // Create Global Layout and set in renderer


#if 0
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
#endif

    while (!glfwWindowShouldClose(glfw_window))
    {
        glfwPollEvents();
    }

    VK_CHECK(vkDeviceWaitIdle(vulkanCore.vk_device));
}


int main()
{
    const char* windowName = "Engine";

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    GLFWwindow* glfw_window = glfwCreateWindow(static_cast<int>(WINDOW_WIDTH), static_cast<int>(WINDOW_HEIGHT), windowName, nullptr, nullptr);
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
        .requestedSwapchainImageExtent = { WINDOW_WIDTH, WINDOW_HEIGHT },
        .requestedSwapchainImagePresentMode = VK_PRESENT_MODE_FIFO_KHR,
        .pDeviceFeatures = (void*)(&vk_physicalDeviceFeatures13)
    };

    VulkanCore vulkanCore = initVulkan(vulkanInitInfo);

    VulkanWrapper::Resource::initResources(vulkanCore.vk_physicalDevice, vulkanCore.vk_device);
#ifdef DEBUG
    DebugUtilsEXT::init(vulkanCore.vk_instance, vulkanCore.vk_device);
#endif

    kickoff(glfw_window, vulkanCore);

    cleanupVulkan(vulkanCore);

    glfwDestroyWindow(glfw_window);
    glfwTerminate();
}



void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    switch (key)
    {
        case GLFW_KEY_ESCAPE:
            // if (action == GLFW_PRESS)
                // renderGui = !renderGui;
            break;
        default:
            break;
    };
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

VulkanWrapper::DescriptorSet allocateDescriptorSet(const VkDescriptorPool vk_pool, const VkDescriptorSetLayout vk_layout, void* pNext)
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