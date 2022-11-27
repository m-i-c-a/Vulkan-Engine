#include "CommandPool.hpp"
#include "Defines.hpp"


namespace VulkanWrapper
{

CommandPool::CommandPool(const VkCommandPoolCreateFlags vk_flags, const uint32_t qFamIdx)
{
    const VkCommandPoolCreateInfo vk_cmdPoolCreateInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = vk_flags,
        .queueFamilyIndex = qFamIdx 
    };

    VK_CHECK(vkCreateCommandPool(s_vkDevice, &vk_cmdPoolCreateInfo, nullptr, &vk_handle));
}

CommandPool::~CommandPool()
{
    vkDestroyCommandPool(s_vkDevice, vk_handle, nullptr);
}

void CommandPool::reset(const VkCommandPoolResetFlags vk_flags)
{
    VK_CHECK(vkResetCommandPool(s_vkDevice, vk_handle, vk_flags));
}

};