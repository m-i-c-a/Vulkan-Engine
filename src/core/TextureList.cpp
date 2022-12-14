#include "TextureList.hpp"
#include "Defines.hpp"

#include "core/Texture.hpp"

#include "vulkanwrapper/Sampler.hpp"

VulkanWrapper::Sampler* loadSampler(const SamplerType type)
{
    switch (type)
    {
        case SamplerType::eDefault:
        {
            const VkSamplerCreateInfo vk_createInfo {
                .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
                .magFilter = VK_FILTER_LINEAR,
                .minFilter = VK_FILTER_LINEAR,
                .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
                .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
                .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
                .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
                .mipLodBias = 0.0,
                .anisotropyEnable = VK_FALSE,
                .compareEnable = VK_FALSE,
                .minLod = 0.0f,
                .maxLod = 0.0f,
                .unnormalizedCoordinates = VK_FALSE,
            };

            VulkanWrapper::Sampler* sampler = new VulkanWrapper::Sampler(vk_createInfo);
            return sampler;
        }
    }

    return nullptr;
}

namespace Core
{

TextureList::TextureList(const uint32_t maxTextureCount)
{
    // Load Supported Samplers
    const uint8_t samplerCount = static_cast<uint8_t>(SamplerType::eInvalid);
    for (uint8_t i = 0; i < samplerCount; ++i)
    {
        m_samplers[i] = loadSampler(static_cast<SamplerType>(i));
    }

    // Create Texture List
    m_textures.resize(maxTextureCount);

    // Populate all indices as "available"
    for (uint32_t i = maxTextureCount - 1; i > 0; i--)
    {
        m_availableIndices.push(i);
    }
}

TextureList::~TextureList()
{
    for (uint32_t i = 0; i < m_samplers.size(); ++i)
    {
        delete m_samplers[i];
    }


    for (uint32_t i = 0; i < m_textures.size(); ++i)
    {
        delete m_textures[i];
    }
}

uint32_t TextureList::acquire(const VkImageCreateInfo& vk_imageCreateInfo, VkImageViewCreateInfo& vk_imageViewCreateInfo, const SamplerType samplerType)
{
    Core::Texture* texture = new Core::Texture(vk_imageCreateInfo, vk_imageViewCreateInfo, m_samplers[static_cast<uint8_t>(samplerType)]);

    const uint32_t id = m_availableIndices.top();
    m_availableIndices.pop();

    m_textures[id] = texture;

    return id;
}

}; // Core