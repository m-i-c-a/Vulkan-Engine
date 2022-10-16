#include "BufferPool.hpp"
#include "Buffer.hpp"
#include "ShaderStructs.hpp"

template<class T>
BufferPool<T>::BufferPool(const uint16_t blockCount, const uint16_t dirtyCount)
{
    m_dirtyBlocks.resize(dirtyCount);

    const VkDeviceSize size = blockCount * sizeof(T);

    m_stagingBuffer = new Buffer();
    m_stagingBuffer->create(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    m_stagingBuffer->allocate(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    m_stagingBuffer->bind();

    m_cpuBlocks = (T*)(m_stagingBuffer->map());

    m_ssboBuffer = new Buffer();
    m_ssboBuffer->create(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
    m_ssboBuffer->allocate(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    m_ssboBuffer->bind();

    for (uint16_t i = blockCount - 1; i > 0; i--)
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
void BufferPool<T>::flushDirtyBlocks()
{
    // need to flush mapped memory here

    for (uint16_t i = 0; i < m_activeDirtyBlockCount; ++i)
    {
        // create copy command and send off to staging buffer
    }

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

template class BufferPool<ObjectData>;