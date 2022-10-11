#include <vector>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "Defines.hpp"
#include "VulkanCore.hpp"

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


void appInit(const VulkanCore& vulkanCore)
{

}

void appRender(const VulkanCore& vulkanCore)
{
    const VkRenderingInfo vk_renderingInfo {
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
        .renderArea = { .offset = { 0, 0 }, .extent = { vulkanCore.vk_swapchainExtent.width, vulkanCore.vk_swapchainExtent.height } },
        .layerCount = 1,
        .viewMask = 0,
        .colorAttachmentCount = 1,
        .pColorAttachments = nullptr,
        .pDepthAttachment = nullptr,
        .pStencilAttachment = nullptr,
    };

    // vkCmdBeginRendering();
    // vkCmdEndRendering();
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

    const VulkanInitInfo vulkanInitInfo {
        .window = glfw_window,
        .requestedInstanceLayerNames = { "VK_LAYER_KHRONOS_validation" },
        .requestedInstanceExtensionNames = { "VK_KHR_surface", "VK_KHR_xcb_surface" },
        .requestedDeviceExtensionNames = { VK_KHR_SWAPCHAIN_EXTENSION_NAME },
        .requestedSwapchainImageCount = 3,
        .requestedSwapchainImageFormat = VK_FORMAT_R8G8B8_SRGB,
        .requestedSwapchainImageExtent = { windowWidth, windowHeight },
        .requestedSwapchainImagePresentMode = VK_PRESENT_MODE_FIFO_KHR
    };

    VulkanCore vulkanCore = initVulkan(vulkanInitInfo);

    appInit(vulkanCore);

    while (!glfwWindowShouldClose(glfw_window))
    {
        glfwPollEvents();

        appRender(vulkanCore);

        glfwSwapBuffers(glfw_window);
    }

    cleanupVulkan(vulkanCore);

    glfwDestroyWindow(glfw_window);
    glfwTerminate();
}