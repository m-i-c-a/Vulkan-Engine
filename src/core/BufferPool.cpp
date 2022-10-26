#include "BufferPool.hpp"
#include "ShaderStructs.hpp"
#include "vulkanwrapper/Buffer.hpp"

template<class T>
BufferPool<T>::BufferPool(const uint16_t blockCount, const uint16_t dirtyCount, const VkBufferUsageFlags vk_bufferUsage)
{
    m_dirtyBlocks.resize(dirtyCount);

    const VkDeviceSize size = blockCount * sizeof(T);

    m_stagingBuffer = new VulkanWrapper::Buffer();
    m_stagingBuffer->create(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    m_stagingBuffer->allocate(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT); // TODO - flush instead
    m_stagingBuffer->bind();

    m_cpuBlocks = (T*)(m_stagingBuffer->map());

    m_ssboBuffer = new VulkanWrapper::Buffer();
    m_ssboBuffer->create(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | vk_bufferUsage);
    m_ssboBuffer->allocate(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    m_ssboBuffer->bind();

    for (int i = blockCount - 1; i >= 0; i--)
    {
        m_freeBlocks.push(i);
    }
}

template<class T>
BufferPool<T>::~BufferPool()
{
    m_stagingBuffer->unmap();
    delete m_stagingBuffer;
    delete m_ssboBuffer;
}

template<class T>
uint16_t BufferPool<T>::acquireBlock()
{
    uint16_t blockIdx = m_freeBlocks.top();
    m_freeBlocks.pop();
    return blockIdx;
}

template<class T>
const T& BufferPool<T>::getReadableBlock(const uint16_t id) const
{
    return m_cpuBlocks[id];
}

template<class T>
T& BufferPool<T>::getWritableBlock(const uint16_t id)
{
    m_dirtyBlocks[m_activeDirtyBlockCount++] = id;
    return m_cpuBlocks[id];
}

template<class T>
void BufferPool<T>::flushDirtyBlocks(const VkCommandBuffer vk_cmdBuff)
{
    std::vector<VkBufferCopy> vk_bufferCopies(m_activeDirtyBlockCount);

    for (uint16_t i = 0; i < m_activeDirtyBlockCount; ++i)
    {
        const uint16_t ssboIdx = m_dirtyBlocks[i];

        VkBufferCopy vk_bufferCopy {
            .srcOffset = i * sizeof(T),
            .dstOffset = ssboIdx * sizeof(T),
            .size = sizeof(T),
        };

        vk_bufferCopies[i] = vk_bufferCopy;
    }

    // might need pipeline barrier here...

    vkCmdCopyBuffer(vk_cmdBuff, m_stagingBuffer->m_vkBuffer, m_ssboBuffer->m_vkBuffer, vk_bufferCopies.size(), vk_bufferCopies.data()); 

    m_activeDirtyBlockCount = 0;
}

template<class T>
VkDescriptorBufferInfo BufferPool<T>::getDescBufferInfo() const
{
    const VkDescriptorBufferInfo vk_descBufferInfo {
        .buffer = m_ssboBuffer->m_vkBuffer,
        .offset = 0,
        .range = VK_WHOLE_SIZE,
    };

    return vk_descBufferInfo;
}

#ifdef DEBUG
template<class T>
VkBuffer BufferPool<T>::getStagingBufferHandle() const
{
    return m_stagingBuffer->m_vkBuffer;
}

template<class T>
VkBuffer BufferPool<T>::getStorageBufferHandle() const
{
    return m_ssboBuffer->m_vkBuffer;
}
#endif

template class BufferPool<ObjectData>;
template class BufferPool<GPUDrawCommand>;
template class BufferPool<RenderableInfo>;
template class BufferPool<MeshInfo>;