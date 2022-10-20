#include "CommandBuffer.hpp"
#include "Defines.hpp"


namespace VulkanWrapper
{

void CommandBuffer::create(const VkCommandPool vk_cmdPool)
{
    const VkCommandBufferAllocateInfo vk_cmdBuffAllocInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = vk_cmdPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };

    VK_CHECK(vkAllocateCommandBuffers(s_vkDevice, &vk_cmdBuffAllocInfo, &m_vkCmdBuff));
}

};