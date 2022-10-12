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

struct AppCore
{
    CommandPool* m_cmdPool = nullptr;
    CommandBuffer* m_cmdBuff = nullptr;
    Fence* m_swapchainImageAcquireFence = nullptr;
    uint32_t m_uActiveSwapchainImageIdx = 0;

    Image* m_baseColorAttachmentImage = nullptr;
    ImageView* m_baseColorAttachmentImageView = nullptr;
};

void appInit(const VulkanCore& vulkanCore, AppCore& appCore)
{
    CommandPool* cmdPool = new CommandPool();
    cmdPool->create(0x0, vulkanCore.graphicsQFamIdx);

    CommandBuffer* cmdBuff = new CommandBuffer();
    cmdBuff->create(cmdPool->m_vkCmdPool);

    Fence* fence = new Fence();
    fence->create(0x0);

    appCore.m_cmdPool = cmdPool;
    appCore.m_cmdBuff = cmdBuff;
    appCore.m_swapchainImageAcquireFence = fence;
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

    cleanupVulkan(vulkanCore);

    glfwDestroyWindow(glfw_window);
    glfwTerminate();
}