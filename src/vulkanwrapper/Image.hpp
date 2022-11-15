#ifndef MICA_IMAGE_HPP
#define MICA_IMAGE_HPP

#include <vulkan/vulkan.h>

#include "Resource.hpp"

namespace VulkanWrapper
{

struct Image : public Resource
{
    VkImage m_vkImage = VK_NULL_HANDLE;
    VkDeviceMemory m_vkDeviceMemory = VK_NULL_HANDLE;

    Image() = default;
    Image(const VkImageCreateInfo& vk_createInfo);
    ~Image();

    void create(const VkImageType vk_imageType, const VkFormat vk_imageFormat, const VkExtent3D vk_imageExtent, const uint32_t arrayLayerCount, const VkSampleCountFlagBits vk_sampleCount, const VkImageUsageFlags vk_imageUsage, const uint32_t mipLevelCount, const VkImageLayout vk_initialLayout);
    void allocate(const VkMemoryPropertyFlags vk_memProps);
    void bind();
};

}; // VulkanWrapper

#endif // MICA_IMAGE_HPP