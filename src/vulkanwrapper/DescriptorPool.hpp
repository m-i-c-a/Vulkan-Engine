#ifndef MICA_VULKAN_WRAPPER_DESCRIPTOR_POOL_HPP
#define MICA_VULKAN_WRAPPER_DESCRIPTOR_POOL_HPP

#include <vector>

#include <vulkan/vulkan.h>

#include "Resource.hpp"

namespace VulkanWrapper
{

struct DescriptorPool : public Resource
{
    VkDescriptorPool m_vkDescPool = VK_NULL_HANDLE;

    DescriptorPool() = default;
    ~DescriptorPool();

    void create(const uint32_t maxSets, std::vector<VkDescriptorPoolSize>&& vk_descPoolSizes);
};

}; // VulkanWrapper

#endif // MICA_VULKAN_WRAPPER_DESCRIPTOR_POOL_HPP