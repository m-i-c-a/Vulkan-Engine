#ifndef MICA_IMAGE_HPP
#define MICA_IMAGE_HPP

#include <vulkan/vulkan.h>

#include "Resource.hpp"

namespace VulkanWrapper
{

struct Image : public Resource
{
    VkImage vk_handle = VK_NULL_HANDLE;

    Image(const VkImageCreateInfo& vk_createInfo);
    ~Image();
};

}; // VulkanWrapper

#endif // MICA_IMAGE_HPP