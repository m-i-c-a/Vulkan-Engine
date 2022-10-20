#ifndef MICA_CORE_GENERIC_BUFFER_HPP
#define MICA_CORE_GENERIC_BUFFER_HPP

#include <vulkan/vulkan.h>

namespace VulkanWrapper
{
    struct Buffer;
}

class GenericBuffer
{
private:
protected:
    VulkanWrapper::Buffer* m_buffer = nullptr;
public:
    GenericBuffer(const VkDeviceSize vk_size, const VkBufferUsageFlags vk_bufferUsageFlags, const VkMemoryPropertyFlags vk_memProps);
    ~GenericBuffer();

    VkBuffer vkBuffer() const;
    VkDeviceSize vkSize() const;
};

#endif // MICA_CORE_GENERIC_BUFFER_HPP