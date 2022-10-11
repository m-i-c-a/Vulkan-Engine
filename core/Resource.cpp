#include "Resource.hpp"
#include "../Defines.hpp"

VkDevice Resource::s_vkDevice = VK_NULL_HANDLE;
VkPhysicalDeviceMemoryProperties Resource::s_vkPhysicalDeviceMemProps = {};

void Resource::initResources(const VkPhysicalDevice vk_physicalDevice, const VkDevice vk_device)
{
 s_vkDevice = vk_device;
 vkGetPhysicalDeviceMemoryProperties(vk_physicalDevice, &s_vkPhysicalDeviceMemProps);
}

uint32_t getHeapIndex(const uint32_t memoryTypeIndices, const VkMemoryPropertyFlags memoryPropertyFlags, VkPhysicalDeviceMemoryProperties memoryProperties)
{
	// Iterate over all memory types available for the device used in this example
	for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
	{
		if (memoryTypeIndices & (1 << i) && (memoryProperties.memoryTypes[i].propertyFlags & memoryPropertyFlags) == memoryPropertyFlags)
		{
			return i;
		}
	}

    EXIT("Could not find suitable memory type!");
    return 0;
}
