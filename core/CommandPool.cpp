#include "CommandPool.hpp"
#include "Defines.hpp"


void CommandPool::create(const VkCommandPoolCreateFlags vk_cmdPoolFlags, const uint32_t qFamIdx)
{
    const VkCommandPoolCreateInfo vk_cmdPoolCreateInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = vk_cmdPoolFlags,
        .queueFamilyIndex = qFamIdx 
    };

    VK_CHECK(vkCreateCommandPool(s_vkDevice, &vk_cmdPoolCreateInfo, nullptr, &m_vkCmdPool));
}

CommandPool::~CommandPool()
{
    vkDestroyCommandPool(s_vkDevice, m_vkCmdPool, nullptr);
}