#include "StagingBuffer.hpp"

void StagingBuffer::uploadToStaging(const VkBuffer vk_dstBuffer, std::vector<QueuedBufferUpdate>&& updates)
{
    std::vector<VkBufferCopy> vk_bufferCopies;
    vk_bufferCopies.reserve(updates.size());

    for (QueuedBufferUpdate& update : updates)
    {
        m_buffer->model.update(m_vkOffset, update.m_vkBufferCopy.size, update.m_pData);

        update.m_vkBufferCopy.srcOffset = m_vkOffset;

        m_vkOffset += update.m_vkBufferCopy.size;

        vk_bufferCopies.push_back( std::move( update.m_vkBufferCopy ) );
    }

    auto iter = m_dstBufferCopies.find(vk_dstBuffer);
    if (iter == m_dstBufferCopies.end())
    {
        m_dstBufferCopies.emplace( vk_dstBuffer, std::move(vk_bufferCopies) );
    }
    else
    {
        iter->second.insert(iter->second.end(),
            std::make_move_iterator(vk_bufferCopies.begin()),
            std::make_move_iterator(vk_bufferCopies.end()));
    }
}

void StagingBuffer::uploadToGPU(const VkCommandBuffer vk_cmdBuff)
{
    for ( auto& [vk_dstBuffer, vk_bufferCopies] : m_dstBufferCopies)
    {
        vkCmdCopyBuffer(vk_cmdBuff, m_buffer->getBufferHandle(), vk_dstBuffer, static_cast<uint32_t>(vk_bufferCopies.size()), vk_bufferCopies.data());
    }

    m_dstBufferCopies.clear();
}