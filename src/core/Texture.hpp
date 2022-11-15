#ifndef MICA_TEXTURE_HPP
#define MICA_TEXTURE_HPP

#include <vulkan/vulkan.h>

namespace VulkanWrapper
{

class Image;
class ImageView;
class Sampler;

}; // VulkanWrapper

namespace Core
{

class Texture
{
private:
    VkDeviceMemory m_vkImageMemory           = VK_NULL_HANDLE;
    VkDescriptorImageInfo m_vkDescriptorInfo = {};

    VulkanWrapper::Image*         m_image     = nullptr;
    VulkanWrapper::ImageView*     m_imageView = nullptr;
    const VulkanWrapper::Sampler* m_sampler   = nullptr;
public:
    Texture(const VkImageCreateInfo& vk_imageCreateInfo, VkImageViewCreateInfo& vk_imageViewCreateInfo, const VulkanWrapper::Sampler* const sampler);
    ~Texture();

    void updateSampler(VulkanWrapper::Sampler* sampler);
    const VkDescriptorImageInfo* const viewDescriptorInfo() const { return &m_vkDescriptorInfo; }
};

} // Core

/*

This is the first class I have written in the "Core" namespace. The design philosphy is that that all classes in core
should be able to handle all possible scenarios. This fuels why we are passing in raw vulkan create info structures. 
Most of the common cases may have similar create infos, so we could "assume" some state to be the same across all texture.
I don't want these assumptions. They are limiting and can cause problems later on.

Basically, I want to be able to create whatever I want by just specifying what I want to create and then deal with
none of the creation logic.

*/

#endif // MICA_TEXTURE_HPP