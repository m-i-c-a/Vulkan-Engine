#ifndef MICA_CORE_ATTACHMENT_HPP
#define MICA_CORE_ATTACHMENT_HPP

#include <vulkan/vulkan.h>

namespace VulkanWrapper
{

class Image;
class DeviceMemory;
class ImageView;

}; // VulkanWrapper

namespace Core
{

class Attachment
{
private:
    VkDeviceMemory m_vkImageMemory           = VK_NULL_HANDLE;
    VkDescriptorImageInfo m_vkDescriptorInfo = {};

    VulkanWrapper::Image*         m_image     = nullptr;
    VulkanWrapper::DeviceMemory*  m_memory    = nullptr;
    VulkanWrapper::ImageView*     m_imageView = nullptr;
public:
    Attachment(const VkImageCreateInfo& vk_imageCreateInfo, VkImageViewCreateInfo& vk_imageViewCreateInfo);
    ~Attachment();

    VkImage     getVkImage() const;
    VkImageView getVkImageView() const;
};

} // Core


#endif // MICA_CORE_ATTACHMENT_HPP