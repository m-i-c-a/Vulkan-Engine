#include "Fence.hpp"
#include "Defines.hpp"

void Fence::create(const VkFenceCreateFlags vk_fenceFlags)
{
    const VkFenceCreateInfo vk_fenceCreateInfo {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = vk_fenceFlags
    };

    VK_CHECK(vkCreateFence(s_vkDevice, &vk_fenceCreateInfo, nullptr, &m_vkFence));
}