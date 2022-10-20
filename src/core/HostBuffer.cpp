#include "HostBuffer.hpp"
#include "vulkanwrapper/Buffer.hpp"

HostBuffer::HostBuffer(const VkDeviceSize vk_size, const VkBufferUsageFlags vk_bufferUsageFlags, const VkMemoryPropertyFlags vk_memProps)
    : GenericBuffer(vk_size, vk_bufferUsageFlags, vk_memProps)
{
    m_mappedPtr = m_buffer->map();
}

HostBuffer::~HostBuffer()
{
    m_buffer->unmap();
}