#include "RenderPass.hpp"
#include "Defines.hpp"

namespace VulkanWrapper
{

RenderPass::RenderPass(const VkRenderPassCreateInfo2& vk_createInfo)
{
    VK_CHECK(vkCreateRenderPass2(s_vkDevice, &vk_createInfo, nullptr, &vk_handle));
}

RenderPass::~RenderPass()
{
    vkDestroyRenderPass(s_vkDevice, vk_handle, nullptr);
}

}; // VulkanWrapper