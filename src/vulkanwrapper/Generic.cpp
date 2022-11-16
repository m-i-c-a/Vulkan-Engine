#include "Generic.hpp"
#include "Resource.hpp"
#include "Defines.hpp"

static VkDevice s_vkDevice                 = VK_NULL_HANDLE;
static VkPhysicalDevice s_vkPhysicalDevice = VK_NULL_HANDLE;
static VkPhysicalDeviceMemoryProperties s_vkPhysicalDeviceMemProps;

static uint32_t getHeapIndex(const uint32_t memoryTypeIndices, const VkMemoryPropertyFlags memoryPropertyFlags)
{
	for (uint32_t i = 0; i < s_vkPhysicalDeviceMemProps.memoryTypeCount; i++)
	{
		if (memoryTypeIndices & (1 << i) && (s_vkPhysicalDeviceMemProps.memoryTypes[i].propertyFlags & memoryPropertyFlags) == memoryPropertyFlags)
		{
			return i;
		}
	}

    EXIT("Could not find suitable memory type!");
    return 0;
}


namespace VulkanWrapper
{

void init(const VkPhysicalDevice vk_physicalDevice, const VkDevice vk_device)
{
    s_vkPhysicalDevice = vk_physicalDevice;
    s_vkDevice = vk_device;

    vkGetPhysicalDeviceMemoryProperties(s_vkPhysicalDevice, &s_vkPhysicalDeviceMemProps);
}

VkDeviceMemory acquireMemory(const VkBuffer vk_buffer, const VkMemoryPropertyFlags vk_memProps)
{
    VkMemoryRequirements vk_memReqs;
    vkGetBufferMemoryRequirements(s_vkDevice, vk_buffer, &vk_memReqs);

    const VkMemoryAllocateInfo vk_allocInfo {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = vk_memReqs.size,
        .memoryTypeIndex = getHeapIndex(vk_memReqs.memoryTypeBits, vk_memProps)
    };

    VkDeviceMemory vk_memory = VK_NULL_HANDLE;
    VK_CHECK(vkAllocateMemory(s_vkDevice, &vk_allocInfo, nullptr, &vk_memory));
    return vk_memory;
}

VkDeviceMemory acquireMemory(const VkImage vk_image, const VkMemoryPropertyFlags vk_memProps)
{
    VkMemoryRequirements vk_memReqs;
    vkGetImageMemoryRequirements(s_vkDevice, vk_image, &vk_memReqs);

    const VkMemoryAllocateInfo vk_allocInfo = { 
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = vk_memReqs.size,
        .memoryTypeIndex = getHeapIndex(vk_memReqs.memoryTypeBits, vk_memProps)
    };

    VkDeviceMemory vk_memory = VK_NULL_HANDLE;
    VK_CHECK(vkAllocateMemory(s_vkDevice, &vk_allocInfo, nullptr, &vk_memory));
    return vk_memory;
}

void releaseMemory(VkDeviceMemory& vk_memory)
{
    vkFreeMemory(s_vkDevice, vk_memory, nullptr);
    vk_memory = VK_NULL_HANDLE;
}

void bindMemory(const VkBuffer vk_buffer, const VkDeviceMemory vk_memory, const VkDeviceSize vk_offset)
{
    VK_CHECK(vkBindBufferMemory(s_vkDevice, vk_buffer, vk_memory, vk_offset));
}

void bindMemory(const VkImage vk_image, const VkDeviceMemory vk_memory, const VkDeviceSize vk_offset)
{
    VK_CHECK(vkBindImageMemory(s_vkDevice, vk_image, vk_memory, vk_offset));
}

void* mapMemory(const VkDeviceMemory vk_memory, const VkDeviceSize vk_offset, const VkDeviceSize vk_size)
{
    void* pData = nullptr;
    VK_CHECK(vkMapMemory(s_vkDevice, vk_memory, vk_offset, vk_size, 0x0, &pData));
    return pData;
}

void unmapMemory(const VkDeviceMemory vk_memory)
{
    vkUnmapMemory(s_vkDevice, vk_memory);
}

}; // VulkanWrapper