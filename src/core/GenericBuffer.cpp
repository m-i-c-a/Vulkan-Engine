#include "GenericBuffer.hpp"
#include "vulkanwrapper/Buffer.hpp"

GenericBuffer::GenericBuffer(const VkDeviceSize vk_size, const VkBufferUsageFlags vk_bufferUsageFlags, const VkMemoryPropertyFlags vk_memProps)
{
    m_buffer = new VulkanWrapper::Buffer();
    m_buffer->create(vk_size, vk_bufferUsageFlags);
    m_buffer->allocate(vk_memProps);
    m_buffer->bind();
}

GenericBuffer::~GenericBuffer()
{
    delete m_buffer;
}


VkBuffer GenericBuffer::vkBuffer() const
{
    return m_buffer->m_vkBuffer;
}