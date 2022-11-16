#ifndef MICA_VULKAN_WRAPPER_GENERIC_HPP
#define MICA_VULKAN_WRAPPER_GENERIC_HPP

#include <vulkan/vulkan.h>

namespace VulkanWrapper
{

void init(const VkPhysicalDevice vk_physicalDevice, const VkDevice vk_device);

VkDeviceMemory acquireMemory(const VkBuffer vk_buffer, const VkMemoryPropertyFlags vk_memProps);
VkDeviceMemory acquireMemory(const VkImage vk_image, const VkMemoryPropertyFlags vk_memProps);

void releaseMemory(VkDeviceMemory& vk_memory);

void bindMemory(const VkBuffer vk_buffer, const VkDeviceMemory vk_memory, const VkDeviceSize vk_offset);
void bindMemory(const VkImage vk_image, const VkDeviceMemory vk_memory, const VkDeviceSize vk_offset);

void* mapMemory(const VkDeviceMemory vk_memory, const VkDeviceSize vk_offset, const VkDeviceSize vk_size);
void  unmapMemory(const VkDeviceMemory vk_memory);

};

#endif // MICA_VULKAN_WRAPPER_GENERIC_HPP