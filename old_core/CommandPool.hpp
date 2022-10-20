#ifndef MICA_COMMAND_POOL_HPP
#define MICA_COMMAND_POOL_HPP

#include <vulkan/vulkan.h>

#include "Resource.hpp"

struct CommandPool : public Resource
{
    VkCommandPool m_vkCmdPool = VK_NULL_HANDLE;

    CommandPool() = default;
    ~CommandPool();

    void create(const VkCommandPoolCreateFlags vk_cmdPoolFlags, const uint32_t qFamIdx);
};

#endif // MICA_COMMAND_POOL_HPP