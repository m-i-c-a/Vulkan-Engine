#include "Defines.hpp"
#include "DescriptorPool.hpp"

namespace VulkanWrapper
{

DescriptorPool::DescriptorPool(const VkDescriptorPoolCreateInfo& vk_createInfo)
{

    VK_CHECK(vkCreateDescriptorPool(s_vkDevice, &vk_createInfo, nullptr, &vk_handle));
}

DescriptorPool::~DescriptorPool()
{
    vkDestroyDescriptorPool(s_vkDevice, vk_handle, nullptr);
}

}; // VulkanWrapper