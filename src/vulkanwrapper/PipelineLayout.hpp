#ifndef MICA_VULKAN_WRAPPER_PIPELINE_LAYOUT_HPP
#define MICA_VULKAN_WRAPPER_PIPELINE_LAYOUT_HPP

#include <vulkan/vulkan.h>

#include "Resource.hpp"

namespace VulkanWrapper
{

struct PipelineLayout : public Resource
{
    VkPipelineLayout vk_handle = VK_NULL_HANDLE;

    PipelineLayout(const VkPipelineLayoutCreateInfo& vk_createInfo);
    ~PipelineLayout();
};

}; // VulkanWrapper

#endif // MICA_VULKAN_WRAPPER_PIPELINE_LAYOUT_HPP