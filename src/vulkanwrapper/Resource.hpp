#ifndef MICA_RESOURCE_HPP
#define MICA_RESOURCE_HPP

#include <vulkan/vulkan.h>

namespace VulkanWrapper
{

struct Resource
{
protected:
    static VkPhysicalDeviceMemoryProperties s_vkPhysicalDeviceMemProps; 
    static VkDevice s_vkDevice;
public:
    static void initResources(VkPhysicalDevice vk_physicalDevice, VkDevice vk_device);
};

}; // VulkanWrapper

#endif // MICA_RESOURCE_HPP
