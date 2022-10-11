#ifndef MICA_BUFFER_POOL_HPP
#define MICA_BUFFER_POOL_HPP

#include <vector>
#include <stack>

#include <vulkan/vulkan.h>

class Buffer;

template<class T>
class BufferPool
{
private:
    T* m_cpuBlocks;
    std::vector<uint16_t> m_dirtyBlocks;
    std::stack<uint16_t> m_freeBlocks;
    uint43_t m_activeBlockCount = 0;
    uint32_t m_activeDirtyBlockCount = 0;

    Buffer* m_stagingBuffer;
    Buffer* m_ssboBuffer;
public:
    BufferPool(const uint16_t blockCount, const uint16_t dirtyCount);
    ~BufferPool();

    uint16_t acquireBlock();
    const T& getReadableBlock(const uint16_t id) const;
    T& getWritableBlock(const uint16_t id);
    void flushDirtyBlocks();
};

#endif // MICA_BUFFER_POOL_HPP