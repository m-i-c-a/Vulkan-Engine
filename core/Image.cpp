#include "Image.hpp"
#include "Defines.hpp"

void Image::create(const VkImageType vk_imageType, const VkFormat vk_imageFormat, const VkExtent3D vk_imageExtent, const uint32_t arrayLayerCount, const VkSampleCountFlagBits vk_sampleCount, const VkImageUsageFlags vk_imageUsage, const uint32_t mipLevelCount)
{
    const VkImageCreateInfo vk_imageCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = vk_imageType,
        .format = vk_imageFormat,
        .extent = vk_imageExtent,
        .mipLevels = mipLevelCount,
        .arrayLayers = arrayLayerCount,
        .samples = vk_sampleCount,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = vk_imageUsage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
    };

    VK_CHECK(vkCreateImage(s_vkDevice, &vk_imageCreateInfo, nullptr, &m_vkImage));
}

void Image::allocate(const VkMemoryPropertyFlags vk_memProps)
{
    VkMemoryRequirements vk_memReqs;
    vkGetImageMemoryRequirements(s_vkDevice, m_vkImage, &vk_memReqs);

    const VkMemoryAllocateInfo vk_allocInfo = { 
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = vk_memReqs.size,
        .memoryTypeIndex = getHeapIndex(vk_memReqs.memoryTypeBits, vk_memProps)
    };

    VK_CHECK(vkAllocateMemory(s_vkDevice, &vk_allocInfo, nullptr, &m_vkDeviceMemory));
}

void Image::bind()
{
    VK_CHECK(vkBindImageMemory(s_vkDevice, m_vkImage, m_vkDeviceMemory, 0));
}