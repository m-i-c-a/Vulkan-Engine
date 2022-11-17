#include "PersistentDeviceBuffer.hpp"

#include "Defines.hpp"

#include "PersistentStagingBuffer.hpp"
#include "vulkanwrapper/Buffer.hpp"
#include "vulkanwrapper/DeviceMemory.hpp"

#include <string.h>

PersistentDeviceBuffer::PersistentDeviceBuffer(const VkDeviceSize vk_size, const VkBufferUsageFlags vk_bufferUsageFlags, const VkMemoryPropertyFlags vk_memProps) // , const VkBuffer vk_stagingBuffer, const VkDeviceSize vk_stagingOffset, char* pData)
    : m_vkSize { vk_size }
{
    m_buffer = new VulkanWrapper::Buffer(m_vkSize, vk_bufferUsageFlags);
    m_memory = new VulkanWrapper::DeviceMemory(m_buffer->vk_handle, vk_memProps);
    m_memory->bind(m_buffer->vk_handle, 0);
}

PersistentDeviceBuffer::~PersistentDeviceBuffer()
{
    delete m_buffer;
    delete m_memory;

    m_buffer = nullptr;
    m_memory = nullptr;
}

void PersistentDeviceBuffer::setStagingInfo(const VkBuffer vk_stagingBuffer, const VkDeviceSize vk_stagingOffset, char* pData)
{
    m_vkStagingBuffer = vk_stagingBuffer;
    m_vkStagingOffset = vk_stagingOffset;
    m_pStagingData    = pData;
}

void PersistentDeviceBuffer::update(const VkDeviceSize vk_offset, const VkDeviceSize vk_size, const void* pData)
{
    memcpy(m_pStagingData + vk_offset, pData, vk_size);

    const VkBufferCopy2 vk_bufferCopy {
        .sType = VK_STRUCTURE_TYPE_BUFFER_COPY_2,
        .srcOffset = m_vkStagingOffset + vk_offset,
        .dstOffset = vk_offset,
        .size = vk_size
    };

    vk_bufferCopies.push_back(std::move(vk_bufferCopy));
}

void PersistentDeviceBuffer::flush(const VkCommandBuffer vk_cmdBuff)
{
#ifdef DEBUG
    if (vk_bufferCopies.size() == 0)
        LOG("WARNING - Trying to flush a buffer which is not dirty! This should never happen!\n");
#endif

    const VkCopyBufferInfo2 vk_copyBufferInfo {
        .sType = VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2,
        .srcBuffer = m_vkStagingBuffer,
        .dstBuffer = m_buffer->vk_handle,
        .regionCount = static_cast<uint32_t>(vk_bufferCopies.size()),
        .pRegions = vk_bufferCopies.data(),
    };

    vkCmdCopyBuffer2(vk_cmdBuff, &vk_copyBufferInfo);

    vk_bufferCopies.clear();
}

VkBuffer PersistentDeviceBuffer::getBuffer() const
{
    return m_buffer->vk_handle;
}