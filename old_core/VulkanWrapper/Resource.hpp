#ifndef MICA_RESOURCE_HPP
#define MICA_RESOURCE_HPP

#include <vulkan/vulkan.h>

namespace VulkanWrapper
{

struct Resource
{
private:
    static VkPhysicalDeviceMemoryProperties s_vkPhysicalDeviceMemProps; 
protected:

    static uint32_t getHeapIndex(const uint32_t memoryTypeIndices, const VkMemoryPropertyFlags memoryPropertyFlags);
public:
    static VkDevice s_vkDevice;
    static void initResources(VkPhysicalDevice vk_physicalDevice, VkDevice vk_device);
};

}; // VulkanWrapper

#endif // MICA_RESOURCE_HPP
