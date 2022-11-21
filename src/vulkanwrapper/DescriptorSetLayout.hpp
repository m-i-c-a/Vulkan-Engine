#ifndef MICA_VULKAN_WRAPPER_DESCRIPTOR_SET_LAYOUT_HPP
#define MICA_VULKAN_WRAPPER_DESCRIPTOR_SET_LAYOUT_HPP

#include <vulkan/vulkan.h>

#include "Resource.hpp"

namespace VulkanWrapper
{

struct DescriptorSetLayout : public Resource
{
    VkDescriptorSetLayout vk_handle = VK_NULL_HANDLE;

    DescriptorSetLayout(const VkDescriptorSetLayoutCreateInfo& vk_createInfo);
    ~DescriptorSetLayout();
};

}; // VulkanWrapper

#endif // MICA_VULKAN_WRAPPER_DESCRIPTOR_POOL_HPP