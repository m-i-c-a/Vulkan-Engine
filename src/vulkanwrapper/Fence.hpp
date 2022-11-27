#ifndef MICA_FENCE_HPP
#define MICA_FENCE_HPP

#include <vulkan/vulkan.h>

#include "Resource.hpp"

namespace VulkanWrapper
{

struct Fence : public Resource
{
    VkFence vk_handle = VK_NULL_HANDLE;

    Fence(const VkFenceCreateFlags vk_fenceFlags);
    ~Fence();

    void wait();
    void reset();

};

};

#endif // MICA_FENCE_HPP