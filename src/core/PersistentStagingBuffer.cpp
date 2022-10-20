#include "Defines.hpp"
#include "PersistentStagingBuffer.hpp"
#include "PersistentDeviceBuffer.hpp"
#include "HostBuffer.hpp"

PersistentStagingBuffer::PersistentStagingBuffer()
{
}

PersistentStagingBuffer::~PersistentStagingBuffer()
{
    delete m_buffer;
}

PersistentDeviceBuffer* PersistentStagingBuffer::registerDeviceBuffer(const VkDeviceSize vk_size, const VkBufferUsageFlags vk_bufferUsageFlags, const VkMemoryPropertyFlags vk_memProps)
{
    PersistentDeviceBuffer* deviceBuffer = new PersistentDeviceBuffer(vk_size, vk_bufferUsageFlags, vk_memProps);
    m_registeredDeviceBuffers.push_back(deviceBuffer);
    m_vkSize += vk_size;

    return deviceBuffer;
}

void PersistentStagingBuffer::completeRegistration()
{
    // TODO - need to change mem props to visible only and implement flushing
    m_buffer = new HostBuffer(m_vkSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    VkDeviceSize vk_offset = 0;
    for (PersistentDeviceBuffer* deviceBuffer : m_registeredDeviceBuffers)
    {
        deviceBuffer->setStagingInfo(m_buffer->vkBuffer(), vk_offset, (char*)m_buffer->m_mappedPtr + vk_offset);
        vk_offset += deviceBuffer->m_vkSize;
    }

    m_registeredDeviceBuffers.clear();
}

VkBuffer PersistentStagingBuffer::vkBuffer() const
{
    return m_buffer->vkBuffer();
}