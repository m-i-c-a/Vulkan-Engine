#ifndef MICA_VULKAN_WRAPPER_PIPELINE_HPP
#define MICA_VULKAN_WRAPPER_PIPELINE_HPP

#include <vulkan/vulkan.h>

#include "Resource.hpp"

namespace VulkanWrapper
{

struct Pipeline : public Resource
{
    VkPipeline vk_handle = VK_NULL_HANDLE;

    Pipeline(const VkGraphicsPipelineCreateInfo& vk_createInfo);
    ~Pipeline();
};

}; // VulkanWrapper

#endif // MICA_VULKAN_WRAPPER_PIPELINE_HPP