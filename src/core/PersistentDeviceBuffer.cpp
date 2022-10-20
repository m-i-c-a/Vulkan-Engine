#include "PersistentDeviceBuffer.hpp"
#include "PersistentStagingBuffer.hpp"
#include "vulkanwrapper/Buffer.hpp"

PersistentDeviceBuffer::PersistentDeviceBuffer(const VkDeviceSize vk_size, const VkBufferUsageFlags vk_bufferUsageFlags, const VkMemoryPropertyFlags vk_memProps) // , const VkBuffer vk_stagingBuffer, const VkDeviceSize vk_stagingOffset, char* pData)
    : GenericBuffer     { vk_size, vk_bufferUsageFlags, vk_memProps }
    , m_vkSize { vk_size }
{
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

    const VkBufferCopy vk_bufferCopy {
        .srcOffset = m_vkStagingOffset + vk_offset,
        .dstOffset = vk_offset,
        .size = vk_size
    };

    vk_bufferCopies.push_back(std::move(vk_bufferCopy));
}

void PersistentDeviceBuffer::flush(const VkCommandBuffer vk_cmdBuff)
{
    vkCmdCopyBuffer(vk_cmdBuff, m_vkStagingBuffer, m_buffer->m_vkBuffer, static_cast<uint32_t>(vk_bufferCopies.size()), vk_bufferCopies.data());

    vk_bufferCopies.clear();
}