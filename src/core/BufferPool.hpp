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
    std::vector<uint32_t> m_dirtyBlocks;
    std::stack<uint32_t> m_freeBlocks;
    uint32_t m_activeBlockCount = 0;
    uint32_t m_activeDirtyBlockCount = 0;

    VulkanWrapper::Buffer* m_stagingBuffer = nullptr;
    VulkanWrapper::Buffer* m_ssboBuffer = nullptr;
public:
    BufferPool(const uint32_t blockCount, const uint32_t dirtyCount, const VkBufferUsageFlags vk_bufferUsage = 0x0);
    ~BufferPool();

    uint32_t acquireBlock();
    const T& getReadableBlock(const uint32_t id) const;
    T& getWritableBlock(const uint32_t id);
    void flushDirtyBlocks(const VkCommandBuffer vk_cmdBuff);

    VkDescriptorBufferInfo getDescBufferInfo() const;

#ifdef DEBUG
    VkBuffer getStagingBufferHandle() const;
    VkBuffer getStorageBufferHandle() const;
#endif
};

#endif // MICA_BUFFER_POOL_HPP