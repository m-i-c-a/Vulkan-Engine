#include "Attachment.hpp"
#include "vulkanwrapper/DeviceMemory.hpp"
#include "vulkanwrapper/Image.hpp"
#include "vulkanwrapper/ImageView.hpp"

namespace Core
{

Attachment::Attachment(const VkImageCreateInfo& vk_imageCreateInfo, VkImageViewCreateInfo& vk_imageViewCreateInfo)
{
    m_image = new VulkanWrapper::Image(vk_imageCreateInfo);
    m_memory = new VulkanWrapper::DeviceMemory(m_image->vk_handle, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    m_memory->bind(m_image->vk_handle, 0);

    vk_imageViewCreateInfo.image = m_image->vk_handle;
    m_imageView = new VulkanWrapper::ImageView(vk_imageViewCreateInfo);
}

Attachment::~Attachment()
{
    delete m_image;
    delete m_memory;
    delete m_imageView;

    m_image = nullptr;
    m_memory = nullptr;
    m_imageView = nullptr;
}

VkImage Attachment::getVkImage() const
{
    return m_image->vk_handle;
}

VkImageView Attachment::getVkImageView() const
{
    return m_imageView->m_vkImageView;
}

}; // Core
