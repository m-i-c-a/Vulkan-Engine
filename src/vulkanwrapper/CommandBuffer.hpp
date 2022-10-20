#ifndef MICA_COMMAND_BUFFER_HPP
#define MICA_COMMAND_BUFFER_HPP

#include <vulkan/vulkan.h>

#include "Resource.hpp"

namespace VulkanWrapper
{

struct CommandBuffer : public Resource
{
    VkCommandBuffer m_vkCmdBuff = VK_NULL_HANDLE;

    CommandBuffer() = default;

    void create(const VkCommandPool vk_cmdPool);
};

};

#endif // MICA_COMMAND_BUFFER_HPP