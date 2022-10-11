#include "BufferPool.hpp"
#include "Buffer.hpp"

template<class T>
BufferPool<T>::BufferPool(const uint16_t blockCount, const uint16_t dirtyCount)
{
    m_cpuBlocks.resize(blockCount);
    m_dirtyBlocks.resize(dirtyCount);

    const VkDeviceSize size = blockCount * sizeof(T);

    m_stagingBuffer = new Buffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    m_ssboBuffer = new Buffer(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    m_cpuBlocks = (T*)(m_stagingBuffer->map());

    for (uint16_t i = blockCount - 1; i >= 0; i--)
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
T& BufferPool<T>::getBlock(const uint16_t id)
{
    m_activeDirtyBlockCount[m_activeDirtyBlockCount++] = id;
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