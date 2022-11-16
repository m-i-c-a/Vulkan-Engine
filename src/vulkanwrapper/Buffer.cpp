#include "Buffer.hpp"
#include "Defines.hpp"

namespace VulkanWrapper
{

Buffer::Buffer(const VkDeviceSize vk_size, const VkBufferUsageFlags vk_usage)
{
    const VkBufferCreateInfo vk_createInfo {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = vk_size,
        .usage = vk_usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };

    VK_CHECK(vkCreateBuffer(s_vkDevice, &vk_createInfo, nullptr, &vk_handle));
}

Buffer::~Buffer()
{
    vkDestroyBuffer(s_vkDevice, vk_handle, nullptr);
    vk_handle = VK_NULL_HANDLE;
}

}; // VulkanWrapper