#include "Pipeline.hpp"
#include "Defines.hpp"

namespace VulkanWrapper
{

Pipeline::Pipeline(const VkGraphicsPipelineCreateInfo& vk_createInfo)
{
    VK_CHECK(vkCreateGraphicsPipelines(s_vkDevice, VK_NULL_HANDLE, 1, &vk_createInfo, nullptr, &vk_handle));
}

Pipeline::~Pipeline()
{
    vkDestroyPipeline(s_vkDevice, vk_handle, nullptr);
}

}; // VulkanWrapper