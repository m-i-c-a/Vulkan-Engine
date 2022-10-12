#ifndef MICA_RESOURCE_HPP
#define MICA_RESOURCE_HPP

#include <vulkan/vulkan.h>

struct Resource
{
private:
    static VkPhysicalDeviceMemoryProperties s_vkPhysicalDeviceMemProps; 
protected:
    static VkDevice s_vkDevice;

    static uint32_t getHeapIndex(const uint32_t memoryTypeIndices, const VkMemoryPropertyFlags memoryPropertyFlags);
public:
    static void initResources(VkPhysicalDevice vk_physicalDevice, VkDevice vk_device);
};


#endif // MICA_RESOURCE_HPP
