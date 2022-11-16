#if 0

#include "Buffer_Old.hpp"
#include "Defines.hpp"

namespace VulkanWrapper
{

Buffer::~Buffer()
{
    vkDestroyBuffer(s_vkDevice, m_vkBuffer, nullptr);
    vkFreeMemory(s_vkDevice, m_vkDeviceMemory, nullptr);
}

void Buffer::create(const VkDeviceSize vk_size, const VkBufferUsageFlags vk_usage)
{
    const VkBufferCreateInfo vk_bufferCreateInfo {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = vk_size,
        .usage = vk_usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };

    VK_CHECK(vkCreateBuffer(s_vkDevice, &vk_bufferCreateInfo, nullptr, &m_vkBuffer));
}

void Buffer::allocate(const VkMemoryPropertyFlags vk_memProps)
{
    VkMemoryRequirements vk_memReqs;
    vkGetBufferMemoryRequirements(s_vkDevice, m_vkBuffer, &vk_memReqs);

    const VkMemoryAllocateInfo vk_allocInfo {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = vk_memReqs.size,
        .memoryTypeIndex = getHeapIndex(vk_memReqs.memoryTypeBits, vk_memProps)
    };

    VK_CHECK(vkAllocateMemory(s_vkDevice, &vk_allocInfo, nullptr, &m_vkDeviceMemory));
}

void Buffer::bind()
{
    VK_CHECK(vkBindBufferMemory(s_vkDevice, m_vkBuffer, m_vkDeviceMemory, 0));
}

void* Buffer::map()
{
    uint8_t* mappedPtr;
    VK_CHECK(vkMapMemory(s_vkDevice, m_vkDeviceMemory, 0, VK_WHOLE_SIZE, 0, (void**)&mappedPtr));
    return mappedPtr;
}

void Buffer::unmap()
{
    vkUnmapMemory(s_vkDevice, m_vkDeviceMemory);
}

}; // VulkanWrapper

#endif