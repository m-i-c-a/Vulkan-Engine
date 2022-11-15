#include "Sampler.hpp"
#include "Defines.hpp"

namespace VulkanWrapper
{

Sampler::Sampler(const VkSamplerCreateInfo& vk_createInfo)
{
    VK_CHECK(vkCreateSampler(s_vkDevice, &vk_createInfo, nullptr, &m_vkSampler));
}

Sampler::~Sampler()
{
    vkDestroySampler(s_vkDevice, m_vkSampler, nullptr);
}

};