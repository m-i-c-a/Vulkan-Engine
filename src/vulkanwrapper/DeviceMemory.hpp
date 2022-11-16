#ifndef MICA_VULKAN_WRAPPER_DEVICE_MEMORY_HPP
#define MICA_VULKAN_WRAPPER_DEVICE_MEMORY_HPP

#include <vulkan/vulkan.h>

#include "Resource.hpp"

namespace VulkanWrapper
{

struct DeviceMemory : public Resource
{
private:
    static VkDeviceMemory allocate(const VkDeviceSize vk_size, const uint32_t memoryType, const VkMemoryPropertyFlags vk_memProps);
public:
    VkDeviceMemory vk_handle = VK_NULL_HANDLE;

    DeviceMemory(const VkBuffer vk_buffer, const VkMemoryPropertyFlags vk_memProps);
    DeviceMemory(const VkImage vk_image, const VkMemoryPropertyFlags vk_memProps);
    ~DeviceMemory();

    void bind(const VkBuffer vk_buffer, const VkDeviceSize vk_offset);
    void bind(const VkImage vk_image, const VkDeviceSize vk_offset);

    void* map(const VkDeviceSize vk_offset, const VkDeviceSize vk_size);
    void  unmap();
};

}; // VulkanWrapper

#endif // MICA_VULKAN_WRAPPER_DEVICE_MEMORY_HPP