#include "Resource.hpp"
#include "Defines.hpp"

VkDevice Resource::s_vkDevice = VK_NULL_HANDLE;
VkPhysicalDeviceMemoryProperties Resource::s_vkPhysicalDeviceMemProps = {};

void Resource::initResources(const VkPhysicalDevice vk_physicalDevice, const VkDevice vk_device)
{
 s_vkDevice = vk_device;
 vkGetPhysicalDeviceMemoryProperties(vk_physicalDevice, &s_vkPhysicalDeviceMemProps);
}

uint32_t Resource::getHeapIndex(const uint32_t memoryTypeIndices, const VkMemoryPropertyFlags memoryPropertyFlags)
{
	// Iterate over all memory types available for the device used in this example
	for (uint32_t i = 0; i < s_vkPhysicalDeviceMemProps.memoryTypeCount; i++)
	{
		if (memoryTypeIndices & (1 << i) && (s_vkPhysicalDeviceMemProps.memoryTypes[i].propertyFlags & memoryPropertyFlags) == memoryPropertyFlags)
		{
			return i;
		}
	}

    EXIT("Could not find suitable memory type!");
    return 0;
}
