#include "DeviceMemory.hpp"
#include "Defines.hpp"

namespace VulkanWrapper
{

VkDeviceMemory DeviceMemory::allocate(const VkDeviceSize vk_size, const uint32_t memoryType, const VkMemoryPropertyFlags vk_memProps)
{
    const VkMemoryAllocateInfo vk_allocInfo {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = vk_size,
        .memoryTypeIndex = getHeapIndex(memoryType, vk_memProps)
    };

    VkDeviceMemory vk_memory = VK_NULL_HANDLE;
    VK_CHECK(vkAllocateMemory(s_vkDevice, &vk_allocInfo, nullptr, &vk_memory));
    return vk_memory;
}

DeviceMemory::DeviceMemory(const VkBuffer vk_buffer, const VkMemoryPropertyFlags vk_memProps)
{
    VkMemoryRequirements vk_memReqs;
    vkGetBufferMemoryRequirements(s_vkDevice, vk_buffer, &vk_memReqs);

    vk_handle = allocate(vk_memReqs.size, vk_memReqs.memoryTypeBits, vk_memProps);
}

DeviceMemory::DeviceMemory(const VkImage vk_image, const VkMemoryPropertyFlags vk_memProps)
{
    VkMemoryRequirements vk_memReqs;
    vkGetImageMemoryRequirements(s_vkDevice, vk_image, &vk_memReqs);

    vk_handle = allocate(vk_memReqs.size, vk_memReqs.memoryTypeBits, vk_memProps);
}

DeviceMemory::~DeviceMemory()
{
    vkFreeMemory(s_vkDevice, vk_handle, nullptr);
    vk_handle = VK_NULL_HANDLE;
}

void DeviceMemory::bind(const VkBuffer vk_buffer, const VkDeviceSize vk_offset)
{
    VK_CHECK(vkBindBufferMemory(s_vkDevice, vk_buffer, vk_handle, vk_offset));
}

void DeviceMemory::bind(const VkImage vk_image, const VkDeviceSize vk_offset)
{
    VK_CHECK(vkBindImageMemory(s_vkDevice, vk_image, vk_handle, vk_offset));
}

void* DeviceMemory::map(const VkDeviceSize vk_offset, const VkDeviceSize vk_size)
{
    void* pData = nullptr;
    VK_CHECK(vkMapMemory(s_vkDevice, vk_handle, vk_offset, vk_size, 0x0, &pData));
    return pData;
}

void DeviceMemory::unmap()
{
    vkUnmapMemory(s_vkDevice, vk_handle);
}

}; // VulkanWrapper