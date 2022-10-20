#include <string.h>

#include "BufferModel.hpp"
#include "VulkanWrapper/Buffer.hpp"

namespace BufferModel
{

// DEVICE

void Device::init(const VkDevice vk_device, VulkanWrapper::Buffer* buffer)
{
    m_vkBuffer = buffer->m_vkBuffer;
}

void Device::destroy(const VkDevice vk_device, VulkanWrapper::Buffer* buffer)
{

}

// void Device::queueUpdate(const VkDeviceSize vk_offset, const VkDeviceSize vk_size, void* pData)
// {
//     const VkBufferCopy vk_bufferCopy {
//         .srcOffset = 0,
//         .dstOffset = vk_offset,
//         .size = vk_size
//     };

//     QueuedBufferUpdate update {
//         .m_vkBufferCopy = vk_bufferCopy,
//         .m_pData = pData
//     };

//     m_queuedUpdates.push_back(std::move(update));
// }

// void Device::flushQueuedUpdates(StagingBuffer* stagingBuffer)
// {
//     stagingBuffer->uploadToStaging(m_vkBuffer, std::move(m_queuedUpdates));
// }

// HOST

void Host::init(const VkDevice vk_device, VulkanWrapper::Buffer* buffer)
{
    m_mappedPtr = buffer->map();
}

void Host::destroy(const VkDevice vk_device, VulkanWrapper::Buffer* buffer)
{
    buffer->unmap();
}

void Host::update(const VkDeviceSize vk_offset, const VkDeviceSize vk_size, const void* pData)
{
    memcpy(m_mappedPtr + vk_offset, pData, vk_size);
}

}; // BufferModel
