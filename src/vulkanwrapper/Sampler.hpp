#ifndef MICA_SAMPLER_HPP
#define MICA_SAMPLER_HPP

#include <vulkan/vulkan.h>

#include "Resource.hpp"

namespace VulkanWrapper
{

struct Sampler : public Resource
{
    VkSampler m_vkSampler = VK_NULL_HANDLE;

    Sampler(const VkSamplerCreateInfo& vk_createInfo);
    ~Sampler();
};

}; // VulkanWrapper

#endif // MICA_SAMPLER_HPP