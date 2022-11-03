#ifndef MICA_IMAGE_VIEW_HPP
#define MICA_IMAGE_VIEW_HPP

#include <vulkan/vulkan.h>

#include "Resource.hpp"

namespace VulkanWrapper
{

struct ImageView : public Resource
{
    VkImageView m_vkImageView;

    ImageView() = default;
    ~ImageView();

    void create(const VkImage vk_image, const VkImageViewType vk_imageViewType, const VkFormat vk_imageViewFormat, const VkImageSubresourceRange& vk_imageSubresourceRange);
};

}; // VulkanWrapper

#endif // MICA_IMAGE_VIEW_HPP