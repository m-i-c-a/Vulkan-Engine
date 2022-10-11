#ifndef MICA_BUFFER_HPP
#define MICA_BUFFER_HPP

#include <vulkan/vulkan.h>

#include "Resource.hpp"

struct Buffer : public Resource
{
    VkBuffer m_vkBuffer;
    VkDeviceMemory m_vkDeviceMemory;

    Buffer(const VkDeviceSize vk_size, const VkBufferUsageFlags vk_usage, const VkMemoryPropertyFlags vk_memProps);
    ~Buffer();

    void* map();
    void unmap();
};

#endif // MICA_BUFFER_HPP