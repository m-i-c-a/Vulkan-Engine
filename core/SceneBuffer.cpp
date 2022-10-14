#include <string.h>

#include "SceneBuffer.hpp"
#include "Buffer.hpp"
#include "Defines.hpp"

SceneBuffer::SceneBuffer(const VkDeviceSize vk_vertexStride, const VkDeviceSize vk_vertexBufferSize, const VkDeviceSize vk_indexBufferSize, const VkDeviceSize vk_stagingBufferSize)
    : m_vkVertexStride      { vk_vertexStride }
    , m_vkStagingBufferSize { vk_stagingBufferSize }
    , m_vkVertexBufferSize  { vk_vertexBufferSize }
    , m_vkIndexBufferSize   { vk_indexBufferSize }
{
    m_stagingBuffer = new Buffer();
    m_stagingBuffer->create(m_vkStagingBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    m_stagingBuffer->allocate(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    m_stagingBuffer->bind();

    m_vertexBuffer = new Buffer();
    m_vertexBuffer->create(m_vkVertexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    m_vertexBuffer->allocate(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    m_vertexBuffer->bind();

    m_indexBuffer = new Buffer();
    m_indexBuffer->create(m_vkIndexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
    m_indexBuffer->allocate(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    m_indexBuffer->bind();

    m_mappedStagingPtr = m_stagingBuffer->map();
}

SceneBuffer::~SceneBuffer()
{
    delete m_stagingBuffer;
    delete m_vertexBuffer;
    delete m_indexBuffer;
}

int32_t SceneBuffer::queueVertexUpload(const VkDeviceSize vk_size, void* pData)
{
#ifdef DEBUG
    if (m_vkStagingBufferOffset + vk_size > m_vkStagingBufferSize)
    {
        EXIT("ERROR - no space left in scene staging buffer!\n");
    }

    if (m_vkVertexBufferOffset + vk_size > m_vkVertexBufferSize)
    {
        EXIT("ERROR - no space left in scene vertex buffer!\n");
    }

    assert(m_vkStagingBufferOffset % m_vkVertexStride == 0);
    assert(m_vkVertexBufferOffset % m_vkVertexStride == 0);
#endif

    VkBufferCopy vk_buffCopy {
        .srcOffset = m_vkStagingBufferOffset,
        .dstOffset = m_vkVertexBufferOffset,
        .size = vk_size
    };

    m_vkVertexBufferCopies.push_back(std::move(vk_buffCopy));

    memcpy((char*)m_mappedStagingPtr + m_vkStagingBufferOffset, pData, vk_size);
    m_vkStagingBufferOffset += vk_size;

    int32_t vertexOffset = m_vkVertexBufferOffset / m_vkVertexStride;
    m_vkVertexBufferOffset += vk_size;

    return vertexOffset;
}

uint32_t SceneBuffer::queueIndexUpload(const VkDeviceSize vk_size, uint32_t* pData)
{
#ifdef DEBUG
    if (m_vkStagingBufferOffset + vk_size > m_vkStagingBufferSize)
    {
        EXIT("ERROR - no space left in scene staging buffer!\n");
    }

    if (m_vkIndexBufferOffset + vk_size > m_vkIndexBufferSize)
    {
        EXIT("ERROR - no space left in scene index buffer!\n");
    }

    assert(m_vkStagingBufferOffset % m_vkVertexStride == 0);
    assert(m_vkIndexBufferOffset % sizeof(uint32_t) == 0);
#endif

    VkBufferCopy vk_buffCopy {
        .srcOffset = m_vkStagingBufferOffset,
        .dstOffset = m_vkIndexBufferOffset,
        .size = vk_size
    };

    m_vkIndexBufferCopies.push_back(std::move(vk_buffCopy));

    memcpy((char*)m_mappedStagingPtr + m_vkStagingBufferOffset, pData, vk_size);
    m_vkStagingBufferOffset += vk_size;

    int32_t firstIndex = m_vkIndexBufferOffset / sizeof(uint32_t);
    m_vkIndexBufferOffset += vk_size;

    return firstIndex;
}



void SceneBuffer::flushQueuedUploads(const VkCommandBuffer vk_cmdBuff)
{
#if DEBUG
    assert(m_vkVertexBufferCopies.size() == m_vkIndexBufferCopies.size());
#endif

    if (m_vkVertexBufferCopies.empty())
        return;

    vkCmdCopyBuffer(vk_cmdBuff, m_stagingBuffer->m_vkBuffer, m_vertexBuffer->m_vkBuffer, static_cast<uint32_t>(m_vkVertexBufferCopies.size()), m_vkVertexBufferCopies.data());
    m_vkVertexBufferCopies.clear();

    vkCmdCopyBuffer(vk_cmdBuff, m_stagingBuffer->m_vkBuffer, m_indexBuffer->m_vkBuffer, static_cast<uint32_t>(m_vkIndexBufferCopies.size()), m_vkIndexBufferCopies.data());
    m_vkIndexBufferCopies.clear();
}