#include "Texture.hpp"
// #include "vulkanwrapper/Generic.hpp"
#include "DeviceMemory.hpp"
#include "vulkanwrapper/Image.hpp"
#include "vulkanwrapper/ImageView.hpp"
#include "vulkanwrapper/Sampler.hpp"

namespace Core
{

Texture::Texture(const VkImageCreateInfo& vk_imageCreateInfo, VkImageViewCreateInfo& vk_imageViewCreateInfo, const VulkanWrapper::Sampler* const sampler)
    : m_sampler { sampler }
{
    m_image = new VulkanWrapper::Image(vk_imageCreateInfo);
    m_memory = new VulkanWrapper::DeviceMemory(m_image->vk_handle, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    m_memory->bind(m_image->vk_handle, 0);

    vk_imageViewCreateInfo.image = m_image->vk_handle;
    m_imageView = new VulkanWrapper::ImageView(vk_imageViewCreateInfo);

    m_vkDescriptorInfo.sampler = m_sampler->m_vkSampler;
    m_vkDescriptorInfo.imageView = m_imageView->m_vkImageView;
    m_vkDescriptorInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
}

Texture::~Texture()
{
    delete m_image;
    delete m_memory;
    delete m_imageView;

    m_image = nullptr;
    m_memory = nullptr;
    m_imageView = nullptr;
    m_sampler = nullptr;
}

void Texture::updateSampler(VulkanWrapper::Sampler* sampler)
{
    m_vkDescriptorInfo.sampler = sampler->m_vkSampler;
    m_sampler = sampler;
}

}; // Core
