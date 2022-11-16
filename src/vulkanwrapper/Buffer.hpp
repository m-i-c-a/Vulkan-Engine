#ifndef MICA_BUFFER_TEST_HPP
#define MICA_BUFFER_TEST_HPP

#include <vulkan/vulkan.h>

#include "Resource.hpp"

namespace VulkanWrapper
{

struct Buffer : public Resource
{
    VkBuffer vk_handle = VK_NULL_HANDLE;

    Buffer(const VkDeviceSize vk_size, const VkBufferUsageFlags vk_usage); 
    ~Buffer();
};

}; // VulkanWrapper

#endif // MICA_BUFFER_HPP