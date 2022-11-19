#include "Defines.hpp"
#include "DescriptorPool.hpp"

namespace VulkanWrapper
{

DescriptorPool::~DescriptorPool()
{
    vkDestroyDescriptorPool(s_vkDevice, m_vkDescPool, nullptr);
}

void DescriptorPool::create(const uint32_t maxSets, std::vector<VkDescriptorPoolSize>&& vk_descPoolSizes)
{
    // Allows for descriptors allocated from this pool to have bindings with the 
    // VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT flag set.
    const VkDescriptorPoolCreateFlags vk_flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;

    const VkDescriptorPoolCreateInfo vk_descPoolCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .flags = vk_flags,
        .maxSets = maxSets,
        .poolSizeCount = static_cast<uint32_t>(vk_descPoolSizes.size()),
        .pPoolSizes = vk_descPoolSizes.data(),
    };

    VK_CHECK(vkCreateDescriptorPool(s_vkDevice, &vk_descPoolCreateInfo, nullptr, &m_vkDescPool));
}

}; // VulkanWrapper