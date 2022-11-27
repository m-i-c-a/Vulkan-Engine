#include "PipelineLayout.hpp"
#include "Defines.hpp"

namespace VulkanWrapper
{

PipelineLayout::PipelineLayout(const VkPipelineLayoutCreateInfo& vk_createInfo)
{
    VK_CHECK(vkCreatePipelineLayout(s_vkDevice, &vk_createInfo, nullptr, &vk_handle));
}

PipelineLayout::~PipelineLayout()
{
    vkDestroyPipelineLayout(s_vkDevice, vk_handle, nullptr);
}

}; // VulkanWrapper