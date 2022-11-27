#ifndef MICA_COMMAND_POOL_HPP
#define MICA_COMMAND_POOL_HPP

#include <vulkan/vulkan.h>

#include "Resource.hpp"

namespace VulkanWrapper
{

struct CommandPool : public Resource
{
    VkCommandPool vk_handle = VK_NULL_HANDLE;

    CommandPool(const VkCommandPoolCreateFlags vk_cmdPoolFlags, const uint32_t qFamIdx);
    ~CommandPool();

    void reset(const VkCommandPoolResetFlags vk_flags);
};

};

#endif // MICA_COMMAND_POOL_HPP