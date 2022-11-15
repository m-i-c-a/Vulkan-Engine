#ifndef MICA_IMAGE_VIEW_HPP
#define MICA_IMAGE_VIEW_HPP

#include <vulkan/vulkan.h>

#include "Resource.hpp"

namespace VulkanWrapper
{

struct ImageView : public Resource
{
    VkImageView m_vkImageView = VK_NULL_HANDLE;

    ImageView(const VkImageViewCreateInfo& vk_createInfo);
    ~ImageView();
};

}; // VulkanWrapper

#endif // MICA_IMAGE_VIEW_HPP