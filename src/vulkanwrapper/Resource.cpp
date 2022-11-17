#include "Resource.hpp"
#include "Defines.hpp"

namespace VulkanWrapper
{

VkDevice Resource::s_vkDevice = VK_NULL_HANDLE;
VkPhysicalDeviceMemoryProperties Resource::s_vkPhysicalDeviceMemProps = {};

void Resource::initResources(const VkPhysicalDevice vk_physicalDevice, const VkDevice vk_device)
{
 s_vkDevice = vk_device;
 vkGetPhysicalDeviceMemoryProperties(vk_physicalDevice, &s_vkPhysicalDeviceMemProps);
}

}; // VulkanWrapper