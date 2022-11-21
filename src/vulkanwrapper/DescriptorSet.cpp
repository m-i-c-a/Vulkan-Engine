#include "Defines.hpp"
#include "DescriptorSet.hpp"

namespace VulkanWrapper
{

DescriptorSet::DescriptorSet(const VkDescriptorSetAllocateInfo& vk_allocInfo)
{
    VK_CHECK(vkAllocateDescriptorSets(s_vkDevice, &vk_allocInfo, &vk_handle));
}

}; // VulkanWrapper