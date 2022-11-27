#include "CommandBuffer.hpp"
#include "Defines.hpp"


namespace VulkanWrapper
{

CommandBuffer::CommandBuffer(const VkCommandPool vk_cmdPool)
{
    const VkCommandBufferAllocateInfo vk_cmdBuffAllocInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = vk_cmdPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };

    VK_CHECK(vkAllocateCommandBuffers(s_vkDevice, &vk_cmdBuffAllocInfo, &vk_handle));
}

void CommandBuffer::begin(const VkCommandBufferUsageFlags vk_flags)
{
    const VkCommandBufferBeginInfo vk_beginInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = vk_flags,
        .pInheritanceInfo = nullptr,
    };

    VK_CHECK(vkBeginCommandBuffer(vk_handle, &vk_beginInfo));
}

void CommandBuffer::end()
{
    vkEndCommandBuffer(vk_handle);
}

};