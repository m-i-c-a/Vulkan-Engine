#ifndef MICA_BUFFER_HPP
#define MICA_BUFFER_HPP

#include <vulkan/vulkan.h>

#include "Resource.hpp"

namespace VulkanWrapper
{

struct Buffer : public Resource
{
    VkBuffer m_vkBuffer;
    VkDeviceMemory m_vkDeviceMemory;

    Buffer() = default;
    ~Buffer();

    void create(const VkDeviceSize vk_size, const VkBufferUsageFlags vk_usage); 
    void allocate(const VkMemoryPropertyFlags vk_memProps);
    void bind();

    void* map();
    void  unmap();
};

}; // VulkanWrapper

#endif // MICA_BUFFER_HPP