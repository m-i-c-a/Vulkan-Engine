#include "ImageView.hpp"
#include "Defines.hpp"

namespace VulkanWrapper
{

ImageView::ImageView(const VkImageViewCreateInfo& vk_createInfo)
{
    VK_CHECK(vkCreateImageView(s_vkDevice, &vk_createInfo, nullptr, &m_vkImageView));
}

ImageView::~ImageView()
{
    vkDestroyImageView(s_vkDevice, m_vkImageView, nullptr);
}

}; // VulkanWrapper