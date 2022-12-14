#ifndef MICA_ARRAY_OF_TEXTURES_HPP
#define MICA_ARRAY_OF_TEXTURES_HPP

#include <vulkan/vulkan.h>

#include <vector>
#include <array>
#include <stack>

namespace VulkanWrapper
{

class Sampler;

}; // VulkanWrapper

namespace Core
{

    class Texture;

}; // Core

enum class SamplerType
{
    eDefault        = 0,
    eInvalid
};

namespace Core
{

class TextureList 
{
private:
    std::vector<Core::Texture*> m_textures;
    std::array<VulkanWrapper::Sampler*, static_cast<uint32_t>(SamplerType::eInvalid)> m_samplers;

    std::stack<uint32_t> m_availableIndices;
public:
    TextureList(const uint32_t maxTextureCount);
    ~TextureList();

    uint32_t acquire(const VkImageCreateInfo& vk_imageCreateInfo, VkImageViewCreateInfo& vk_imageViewCreateInfo, const SamplerType samplerType);
    void release(const uint32_t id);
};

} // Core

#endif // MICA_ARRAY_OF_TEXTURES