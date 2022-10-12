#ifndef MICA_VULKAN_CORE_HPP
#define MICA_VULKAN_CORE_HPP

#include <vector>

#include <vulkan/vulkan.h> 

class GLFWwindow;

struct VulkanInitInfo
{
    GLFWwindow* window = nullptr;
    std::vector<const char*> requestedInstanceLayerNames {};
    std::vector<const char*> requestedInstanceExtensionNames {};
    std::vector<const char*> requestedDeviceExtensionNames {};
    uint32_t requestedSwapchainImageCount;
    VkFormat requestedSwapchainImageFormat;
    VkExtent2D requestedSwapchainImageExtent;
    VkPresentModeKHR requestedSwapchainImagePresentMode;
    void* pDeviceFeatures = nullptr;
};

struct VulkanCore
{
    VkInstance vk_instance = VK_NULL_HANDLE;
    VkSurfaceKHR vk_surface = VK_NULL_HANDLE;
    VkPhysicalDevice vk_physicalDevice = VK_NULL_HANDLE;
    uint32_t graphicsQFamIdx = UINT32_MAX;
    VkDevice vk_device = VK_NULL_HANDLE;
    VkQueue vk_graphicsQ = VK_NULL_HANDLE;
    VkSwapchainKHR vk_swapchain = VK_NULL_HANDLE;
    std::vector<VkImage> vk_swapchainImages {};
    std::vector<VkImageView> vk_swapchainImageViews {};
    VkFormat vk_swapchainImageFormat = VK_FORMAT_MAX_ENUM;
    VkExtent2D vk_swapchainExtent = { 0, 0 };
};

VulkanCore initVulkan(const VulkanInitInfo& vulkanInitInfo);
void cleanupVulkan(const VulkanCore& vulkanCore);

#endif // MICA_VULKAN_CORE_HPP