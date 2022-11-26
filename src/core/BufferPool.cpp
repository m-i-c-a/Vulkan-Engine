#include "BufferPool.hpp"

#include "Defines.hpp"

#include "ShaderStructs.hpp"
#include "vulkanwrapper/Buffer.hpp"
#include "vulkanwrapper/DeviceMemory.hpp"

namespace Core
{

template<class T>
BufferPool<T>::BufferPool(const uint32_t blockCount, const uint32_t dirtyCount, const VkBufferUsageFlags vk_bufferUsage, const VkPipelineStageFlags2 vk_dstStageMask, const VkAccessFlags2 vk_dstAccessMask)
    : m_totalBlocks     { blockCount       }
    , m_vkDstStageMask  { vk_dstStageMask  }
    , m_vkDstAccessMask { vk_dstAccessMask }
{
    m_dirtyBlocks.resize(dirtyCount);

    const VkDeviceSize vk_size = blockCount * sizeof(T);

    m_stagingBuffer = new VulkanWrapper::Buffer(vk_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    m_stagingMemory = new VulkanWrapper::DeviceMemory(m_stagingBuffer->vk_handle, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    m_stagingMemory->bind(m_stagingBuffer->vk_handle, 0);
    m_cpuBlocks = static_cast<T*>(m_stagingMemory->map(0, vk_size));

    m_ssboBuffer = new VulkanWrapper::Buffer(vk_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | vk_bufferUsage);
    m_ssboMemory = new VulkanWrapper::DeviceMemory(m_ssboBuffer->vk_handle, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    m_ssboMemory->bind(m_ssboBuffer->vk_handle, 0);

    for (int i = blockCount - 1; i >= 0; i--)
    {
        m_freeBlocks.push(i);
    }
}

template<class T>
BufferPool<T>::~BufferPool()
{
    m_stagingMemory->unmap();

    delete m_stagingBuffer;
    delete m_ssboBuffer;

    delete m_stagingMemory;
    delete m_ssboMemory;

    m_stagingMemory = nullptr;
}

template<class T>
uint32_t BufferPool<T>::acquireBlock()
{
    uint32_t blockIdx = m_freeBlocks.top();
    m_freeBlocks.pop();
    return blockIdx;
}

template<class T>
const T& BufferPool<T>::getReadableBlock(const uint32_t id) const
{
    return m_cpuBlocks[id];
}

template<class T>
T& BufferPool<T>::getWritableBlock(const uint32_t id)
{
    m_dirtyBlocks[m_activeDirtyBlockCount++] = id;
    return m_cpuBlocks[id];
}

template<class T>
void BufferPool<T>::flushDirtyBlocks(const VkCommandBuffer vk_cmdBuff, const bool externallySynchronized)
{
    constexpr bool isUnifiedGraphicsAndTransferQueue = true;

    std::vector<VkBufferCopy2> vk_bufferCopies(m_activeDirtyBlockCount);

    for (uint32_t i = 0; i < m_activeDirtyBlockCount; ++i)
    {
        const uint32_t ssboIdx = m_dirtyBlocks[i];

        const VkBufferCopy2 vk_bufferCopy2 {
            .sType = VK_STRUCTURE_TYPE_BUFFER_COPY_2,
            .srcOffset = i * sizeof(T),
            .dstOffset = ssboIdx * sizeof(T),
            .size = sizeof(T),
        };

        vk_bufferCopies[i] = vk_bufferCopy2;
    }

    // No need for pre-copy barrier since all host-writes are made visible via submission.

    const VkCopyBufferInfo2 vk_copyBufferInfo {
        .sType = VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2,
        .srcBuffer = m_stagingBuffer->vk_handle,
        .dstBuffer = m_ssboBuffer->vk_handle,
        .regionCount = static_cast<uint32_t>(vk_bufferCopies.size()),
        .pRegions = vk_bufferCopies.data(),
    };

    vkCmdCopyBuffer2(vk_cmdBuff, &vk_copyBufferInfo);

    if (!externallySynchronized)
    {
        if (isUnifiedGraphicsAndTransferQueue)
        {
            const VkBufferMemoryBarrier2 vk_postCopyBufferMemoryBarrier2 {
                .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
                .srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                .srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,
                .dstStageMask = m_vkDstStageMask,
                .dstAccessMask = m_vkDstAccessMask,
                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .buffer = m_ssboBuffer->vk_handle,
                .offset = 0,
                .size = m_totalBlocks * sizeof(T)
            };

            const VkDependencyInfo vk_postCopyDependencyInfo {
                .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
                .dependencyFlags = 0x0,
                .memoryBarrierCount = 0,
                .pMemoryBarriers = nullptr,
                .bufferMemoryBarrierCount = 1,
                .pBufferMemoryBarriers = &vk_postCopyBufferMemoryBarrier2,
                .imageMemoryBarrierCount = 0,
                .pImageMemoryBarriers = nullptr,
            };

            vkCmdPipelineBarrier2(vk_cmdBuff, &vk_postCopyDependencyInfo);
        }
        else
        {
            EXIT("Non-unified transfer/graphics queue is currently not supported!\n");
        }
    }

    m_activeDirtyBlockCount = 0;
}

template<class T>
VkDescriptorBufferInfo BufferPool<T>::getDescBufferInfo() const
{
    const VkDescriptorBufferInfo vk_descBufferInfo {
        .buffer = m_ssboBuffer->vk_handle,
        .offset = 0,
        .range = VK_WHOLE_SIZE,
    };

    return vk_descBufferInfo;
}

#ifdef DEBUG
template<class T>
VkBuffer BufferPool<T>::getStagingBufferHandle() const
{
    return m_stagingBuffer->vk_handle;
}

template<class T>
VkBuffer BufferPool<T>::getStorageBufferHandle() const
{
    return m_ssboBuffer->vk_handle;
}
#endif

template class BufferPool<ObjectData>;
template class BufferPool<GPUDrawCommand>;
template class BufferPool<RenderableInfo>;
template class BufferPool<MeshInfo>;
template class BufferPool<MatData>;
template class BufferPool<DrawData>;

}; // Core
