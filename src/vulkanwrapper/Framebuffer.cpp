#include "Framebuffer.hpp"
#include "Defines.hpp"

namespace VulkanWrapper
{

Framebuffer::Framebuffer(const VkFramebufferCreateInfo& vk_createInfo)
{
    VK_CHECK(vkCreateFramebuffer(s_vkDevice, &vk_createInfo, nullptr, &vk_handle));
}

Framebuffer::~Framebuffer()
{
    vkDestroyFramebuffer(s_vkDevice, vk_handle, nullptr);
}

}; // VulkanWrapper