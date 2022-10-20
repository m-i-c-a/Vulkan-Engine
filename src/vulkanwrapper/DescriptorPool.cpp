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

    const VkDescriptorPoolCreateInfo vk_descPoolCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .maxSets = maxSets,
        .poolSizeCount = static_cast<uint32_t>(vk_descPoolSizes.size()),
        .pPoolSizes = vk_descPoolSizes.data(),
    };

    VK_CHECK(vkCreateDescriptorPool(s_vkDevice, &vk_descPoolCreateInfo, nullptr, &m_vkDescPool));
}

}; // VulkanWrapper