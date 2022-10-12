#include "ImageView.hpp"
#include "Defines.hpp"

void ImageView::create(const VkImage vk_image, const VkImageViewType vk_imageViewType, const VkFormat vk_imageViewFormat, const VkImageSubresourceRange& vk_imageSubresourceRange)
{
    const VkImageViewCreateInfo vk_imageViewCreateInfo {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = vk_image,
        .viewType = vk_imageViewType,
        .format = vk_imageViewFormat,
        .components = {
            .r = VK_COMPONENT_SWIZZLE_IDENTITY,
            .g = VK_COMPONENT_SWIZZLE_IDENTITY,
            .b = VK_COMPONENT_SWIZZLE_IDENTITY,
            .a = VK_COMPONENT_SWIZZLE_IDENTITY },
        .subresourceRange = vk_imageSubresourceRange
    };

    VK_CHECK(vkCreateImageView(s_vkDevice, &vk_imageViewCreateInfo, nullptr, &m_vkImageView));
}