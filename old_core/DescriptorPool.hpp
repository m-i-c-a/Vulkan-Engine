#ifndef MICA_DESCRIPTOR_POOL_HPP
#define MICA_DESCRIPTOR_POOL_HPP

#include <vector>

#include <vulkan/vulkan.h>

#include "Resource.hpp"

struct DescriptorPool : public Resource
{
    VkDescriptorPool m_vkDescPool = VK_NULL_HANDLE;

    DescriptorPool() = default;
    ~DescriptorPool();

    void create(const uint32_t maxSets, std::vector<VkDescriptorPoolSize>&& vk_descPoolSizes);
};

#endif // MICA_DESCRIPTOR_POOL_HPP