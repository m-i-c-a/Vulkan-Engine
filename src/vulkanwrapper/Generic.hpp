#ifndef MICA_VULKAN_WRAPPER_GENERIC_HPP
#define MICA_VULKAN_WRAPPER_GENERIC_HPP

#include <vulkan/vulkan.h>

namespace VulkanWrapper
{

VkDeviceMemory acquireMemory(const VkBuffer vk_buffer);
VkDeviceMemory acquireMemory(const VkImage vk_image);

void releaseMemory(VkDeviceMemory& vk_memory); // set to vk null handle please

void bindMemory(const VkBuffer vk_buffer, const VkDeviceMemory vk_memory);
void bindMemory(const VkImage vk_image, const VkDeviceMemory vk_memory, const VkDeviceSize vk_offset);

};

#endif // MICA_VULKAN_WRAPPER_GENERIC_HPP