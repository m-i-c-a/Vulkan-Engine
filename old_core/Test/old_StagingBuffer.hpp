#ifndef MICA_STAGING_BUFFER_HPP
#define MICA_STAGING_BUFFER_HPP

#include <string.h>

#include <vector>
#include <unordered_map>

#include <vulkan/vulkan.h>

#include "Buffer.hpp"
#include "BufferCommon.hpp"

struct StagingBuffer
{
    Buffer<BufferModel::Host>* m_buffer = nullptr;
    VkDeviceSize m_vkOffset = 0;

    std::unordered_map<VkBuffer, std::vector<VkBufferCopy>> m_dstBufferCopies;

    void uploadToStaging(const VkBuffer vk_dstBuffer, std::vector<QueuedBufferUpdate>&& updates);

    void uploadToGPU(const VkCommandBuffer vk_cmdBuff);
};

#endif // MICA_STAGING_BUFFER_HPP