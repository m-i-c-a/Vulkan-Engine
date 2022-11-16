#include "SceneBuffer.hpp"

#include "Defines.hpp"

#include "vulkanwrapper/Buffer.hpp"
#include "vulkanwrapper/DeviceMemory.hpp"

#include <string.h>

namespace Core
{

SceneBuffer::SceneBuffer(const VkDeviceSize vk_vertexStride, const VkDeviceSize vk_vertexBufferSize, const VkDeviceSize vk_indexBufferSize, const VkDeviceSize vk_stagingBufferSize)
    : m_vkVertexStride      { vk_vertexStride }
    , m_vkStagingBufferSize { vk_stagingBufferSize }
    , m_vkVertexBufferSize  { vk_vertexBufferSize }
    , m_vkIndexBufferSize   { vk_indexBufferSize }
{
    m_stagingBuffer = new VulkanWrapper::Buffer(m_vkStagingBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    m_stagingMemory = new VulkanWrapper::DeviceMemory(m_stagingBuffer->vk_handle, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    m_stagingMemory->bind(m_stagingBuffer->vk_handle, 0);

    m_vertexBuffer = new VulkanWrapper::Buffer(m_vkVertexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    m_vertexMemory = new VulkanWrapper::DeviceMemory(m_vertexBuffer->vk_handle, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    m_vertexMemory->bind(m_vertexBuffer->vk_handle, 0);

    m_indexBuffer = new VulkanWrapper::Buffer(m_vkIndexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
    m_indexMemory = new VulkanWrapper::DeviceMemory(m_indexBuffer->vk_handle, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    m_indexMemory->bind(m_indexBuffer->vk_handle, 0);

    m_mappedStagingPtr = m_stagingMemory->map(0, m_vkStagingBufferSize);
}

SceneBuffer::~SceneBuffer()
{
    m_stagingMemory->unmap();
    m_mappedStagingPtr = nullptr;

    delete m_stagingBuffer;
    delete m_vertexBuffer;
    delete m_indexBuffer;

    delete m_stagingMemory;
    delete m_vertexMemory;
    delete m_indexMemory;
}

int32_t SceneBuffer::queueVertexUpload(const VkDeviceSize vk_size, const void* pData)
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

    if (m_vkVertexBufferOffset != 0)
    {
        assert(m_vkVertexBufferOffset % m_vkVertexStride == 0);
    }
#endif

    VkBufferCopy2 vk_buffCopy {
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

uint32_t SceneBuffer::queueIndexUpload(const VkDeviceSize vk_size, const uint32_t* pData)
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

    assert(m_vkIndexBufferOffset % sizeof(uint32_t) == 0);
#endif

    VkBufferCopy2 vk_buffCopy {
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



void SceneBuffer::flushQueuedUploads(const VkCommandBuffer vk_cmdBuff, const bool externallySynchronized)
{
#if DEBUG
    assert(m_vkVertexBufferCopies.size() == m_vkIndexBufferCopies.size());
#endif

    constexpr bool isUnifiedGraphicsAndTransferQueue = true;

    if (m_vkVertexBufferCopies.empty())
        return;


    // If there is a semaphore signal + wait between this being submitted and
    // the vertex buffer being used, then skip this pipeline barrier.

    if (!externallySynchronized)
    {
        if (isUnifiedGraphicsAndTransferQueue)
        {
            const VkBufferMemoryBarrier2 vk_postCopyBarriers[2] {
                {
                    .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
                    .srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                    .srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,
                    .dstStageMask = VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT,
                    .dstAccessMask = VK_ACCESS_2_MEMORY_READ_BIT,
                    .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                    .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                    .buffer = m_vertexBuffer->vk_handle,
                    .offset = 0,
                    .size = m_vkVertexBufferSize,
                },
                {
                    .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
                    .srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                    .srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,
                    .dstStageMask = VK_PIPELINE_STAGE_2_INDEX_INPUT_BIT,
                    .dstAccessMask = VK_ACCESS_2_MEMORY_READ_BIT,
                    .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                    .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                    .buffer = m_indexBuffer->vk_handle,
                    .offset = 0,
                    .size = m_vkVertexBufferSize,
                }
            }; 

            const VkDependencyInfoKHR vk_dependencyInfo = {
                .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
                .dependencyFlags = 0x0,
                .memoryBarrierCount = 0,
                .pMemoryBarriers = nullptr,
                .bufferMemoryBarrierCount = 2,
                .pBufferMemoryBarriers = vk_postCopyBarriers,
                .imageMemoryBarrierCount = 0,
                .pImageMemoryBarriers = nullptr,
            };

            vkCmdPipelineBarrier2KHR(vk_cmdBuff, &vk_dependencyInfo);
        }
        else
        {
            EXIT("Non-unified transfer/graphics queue is currently not supported!\n");
        }
    }

    const VkCopyBufferInfo2 vk_vertexCopyInfo {
        .sType = VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2,
        .srcBuffer = m_stagingBuffer->vk_handle,
        .dstBuffer = m_vertexBuffer->vk_handle,
        .regionCount = static_cast<uint32_t>(m_vkVertexBufferCopies.size()),
        .pRegions = m_vkVertexBufferCopies.data(),
    };

    const VkCopyBufferInfo2 vk_indexCopyInfo {
        .sType = VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2,
        .srcBuffer = m_stagingBuffer->vk_handle,
        .dstBuffer = m_indexBuffer->vk_handle,
        .regionCount = static_cast<uint32_t>(m_vkIndexBufferCopies.size()),
        .pRegions = m_vkIndexBufferCopies.data(),
    };

    vkCmdCopyBuffer2(vk_cmdBuff, &vk_vertexCopyInfo);
    vkCmdCopyBuffer2(vk_cmdBuff, &vk_indexCopyInfo);

    m_vkVertexBufferCopies.clear();
    m_vkIndexBufferCopies.clear();
}


MeshInfo SceneBuffer::queueUpload(const uint32_t id, const VkDeviceSize vk_vertexSize, const void* pVertexData, const VkDeviceSize vk_indexSize, const uint32_t* pIndexData)
{
    const auto iter = m_meshInfos.find(id);
    if (iter == m_meshInfos.end())
    {
        const MeshInfo meshInfo {
            .m_uIndexCount = static_cast<uint32_t>(vk_indexSize) / static_cast<uint32_t>(sizeof(uint32_t)),
            .m_uFirstIndex = queueIndexUpload(vk_indexSize, pIndexData),
            .m_iVertexOffset = queueVertexUpload(vk_vertexSize, pVertexData)
        };

        m_meshInfos.emplace(id, meshInfo);

        return std::move(meshInfo);
    }

    EXIT("Are you dumb? You are attempting to load the same mesh twice!\n");

    return {};
}


const MeshInfo& SceneBuffer::getMeshInfo(const uint32_t id) const
{
    return {};
}


const VkBuffer SceneBuffer::getVertexBuffer() const { return m_vertexBuffer->vk_handle; }
const VkBuffer SceneBuffer::getIndexBuffer() const { return m_indexBuffer->vk_handle; }

}; // Core