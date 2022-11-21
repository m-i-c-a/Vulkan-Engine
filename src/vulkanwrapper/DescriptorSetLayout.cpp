#include "Defines.hpp"
#include "DescriptorSetLayout.hpp"

namespace VulkanWrapper
{

DescriptorSetLayout::DescriptorSetLayout(const VkDescriptorSetLayoutCreateInfo& vk_createInfo)
{
    VK_CHECK(vkCreateDescriptorSetLayout(s_vkDevice, &vk_createInfo, nullptr, &vk_handle));
}

DescriptorSetLayout::~DescriptorSetLayout()
{
    vkDestroyDescriptorSetLayout(s_vkDevice, vk_handle, nullptr);
}

}; // VulkanWrapper