#ifndef MICA_VULKAN_WRAPPER_DESCRIPTOR_POOL_HPP
#define MICA_VULKAN_WRAPPER_DESCRIPTOR_POOL_HPP

#include <vulkan/vulkan.h>

#include "Resource.hpp"

namespace VulkanWrapper
{

struct DescriptorPool : public Resource
{
    VkDescriptorPool vk_handle = VK_NULL_HANDLE;

    DescriptorPool(const VkDescriptorPoolCreateInfo& vk_createInfo);
    ~DescriptorPool();
};

}; // VulkanWrapper

#endif // MICA_VULKAN_WRAPPER_DESCRIPTOR_POOL_HPP