#ifndef MICA_VULKAN_WRAPPER_RENDER_PASS_HPP
#define MICA_VULKAN_WRAPPER_RENDER_PASS_HPP

#include <vulkan/vulkan.h>

#include "Resource.hpp"

namespace VulkanWrapper
{

struct RenderPass : public Resource
{
    VkRenderPass vk_handle = VK_NULL_HANDLE;

    RenderPass(const VkRenderPassCreateInfo2& vk_createInfo);
    ~RenderPass();
};

}; // VulkanWrapper

#endif // MICA_VULKAN_WRAPPER_RENDER_PASS_HPP