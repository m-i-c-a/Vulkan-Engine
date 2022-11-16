#include "Image.hpp"
#include "Defines.hpp"

namespace VulkanWrapper
{

Image::Image(const VkImageCreateInfo& vk_createInfo)
{
    VK_CHECK(vkCreateImage(s_vkDevice, &vk_createInfo, nullptr, &vk_handle));
}

Image::~Image()
{
    vkDestroyImage(s_vkDevice, vk_handle, nullptr);
}

}; // VulkanWrapper