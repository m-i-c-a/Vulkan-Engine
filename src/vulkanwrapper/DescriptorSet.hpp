#ifndef MICA_VULKAN_WRAPPER_DESCRIPTOR_SET_HPP
#define MICA_VULKAN_WRAPPER_DESCRIPTOR_SET_HPP

#include <vulkan/vulkan.h>

#include "Resource.hpp"

namespace VulkanWrapper
{

struct DescriptorSet : public Resource
{
    VkDescriptorSet vk_handle = VK_NULL_HANDLE;

    DescriptorSet(const VkDescriptorSetAllocateInfo& vk_allocInfo);
};

}; // VulkanWrapper

#endif // MICA_VULKAN_WRAPPER_DESCRIPTOR_POOL_HPP