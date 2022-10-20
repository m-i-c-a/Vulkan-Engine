#ifndef MICA_BUFFER_POOL_HPP
#define MICA_BUFFER_POOL_HPP

#include <vector>
#include <stack>

#include <vulkan/vulkan.h>

namespace VulkanWrapper
{
    class Buffer;
};

template<class T>
class BufferPool
{
private:
    T* m_cpuBlocks;
    std::vector<uint16_t> m_dirtyBlocks;
    std::stack<uint16_t> m_freeBlocks;
    uint32_t m_activeBlockCount = 0;
    uint32_t m_activeDirtyBlockCount = 0;

    VulkanWrapper::Buffer* m_stagingBuffer = nullptr;
    VulkanWrapper::Buffer* m_ssboBuffer = nullptr;
public:
    BufferPool(const uint16_t blockCount, const uint16_t dirtyCount, const VkBufferUsageFlags vk_bufferUsage = 0x0);
    ~BufferPool();

    uint16_t acquireBlock();
    const T& getReadableBlock(const uint16_t id) const;
    T& getWritableBlock(const uint16_t id);
    void flushDirtyBlocks(const VkCommandBuffer vk_cmdBuff);

    VkDescriptorBufferInfo getDescBufferInfo() const;
};

#endif // MICA_BUFFER_POOL_HPP