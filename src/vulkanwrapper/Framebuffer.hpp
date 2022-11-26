#ifndef MICA_VULKAN_WRAPPER_FRAMEBUFFER_HPP
#define MICA_VULKAN_WRAPPER_FRAMEBUFFER_HPP

#include <vulkan/vulkan.h>

#include "Resource.hpp"

namespace VulkanWrapper
{

struct Framebuffer : public Resource
{
    VkFramebuffer vk_handle = VK_NULL_HANDLE;

    Framebuffer(const VkFramebufferCreateInfo& vk_createInfo);
    ~Framebuffer();
};

}; // VulkanWrapper

#endif // MICA_VULKAN_WRAPPER_FRAMEBUFFER_HPP