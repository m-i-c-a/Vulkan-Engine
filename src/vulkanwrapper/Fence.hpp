#ifndef MICA_FENCE_HPP
#define MICA_FENCE_HPP

#include <vulkan/vulkan.h>

#include "Resource.hpp"

namespace VulkanWrapper
{

struct Fence : public Resource
{
    VkFence m_vkFence = VK_NULL_HANDLE;

    Fence() = default;
    ~Fence();

    void create(const VkFenceCreateFlags vk_fenceFlags);
};

};

#endif // MICA_FENCE_HPP