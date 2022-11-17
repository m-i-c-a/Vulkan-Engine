#include "Defines.hpp"
#include "PersistentStagingBuffer.hpp"
#include "PersistentDeviceBuffer.hpp"

#include "vulkanwrapper/Buffer.hpp"
#include "vulkanwrapper/DeviceMemory.hpp"

PersistentStagingBuffer::PersistentStagingBuffer()
{
}

PersistentStagingBuffer::~PersistentStagingBuffer()
{
    m_memory->unmap();

    delete m_buffer;
    delete m_memory;

    m_buffer = nullptr;
    m_memory = nullptr;
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
    m_buffer = new VulkanWrapper::Buffer(m_vkSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    m_memory = new VulkanWrapper::DeviceMemory(m_buffer->vk_handle, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    m_memory->bind(m_buffer->vk_handle, 0);

    void* mapptedPtr = m_memory->map(0, m_vkSize);

    VkDeviceSize vk_offset = 0;
    for (PersistentDeviceBuffer* deviceBuffer : m_registeredDeviceBuffers)
    {
        deviceBuffer->setStagingInfo(m_buffer->vk_handle, vk_offset, (char*)mapptedPtr + vk_offset);
        vk_offset += deviceBuffer->m_vkSize;
    }

    m_registeredDeviceBuffers.clear();
}

VkBuffer PersistentStagingBuffer::vkBuffer() const
{
    return m_buffer->vk_handle;
}