#include "Fence.hpp"
#include "Defines.hpp"

namespace VulkanWrapper
{

Fence::Fence(const VkFenceCreateFlags vk_fenceFlags)
{
    const VkFenceCreateInfo vk_fenceCreateInfo {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = vk_fenceFlags
    };

    VK_CHECK(vkCreateFence(s_vkDevice, &vk_fenceCreateInfo, nullptr, &vk_handle));
}

Fence::~Fence()
{
    vkDestroyFence(s_vkDevice, vk_handle, nullptr);
}

void Fence::wait()
{
    VK_CHECK(vkWaitForFences(s_vkDevice, 1, &vk_handle, VK_TRUE, UINT64_MAX));
}

void Fence::reset()
{
    VK_CHECK(vkResetFences(s_vkDevice, 1, &vk_handle));
}

}; // VulkanWrapper