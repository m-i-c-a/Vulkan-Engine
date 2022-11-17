#ifndef MICA_BUFFER_POOL_HPP
#define MICA_BUFFER_POOL_HPP

#include <vector>
#include <stack>

#include <vulkan/vulkan.h>

namespace VulkanWrapper
{
    class Buffer;
    class DeviceMemory;
};

namespace Core
{

template<class T>
class BufferPool
{
private:
    T* m_cpuBlocks;
    std::vector<uint32_t> m_dirtyBlocks;
    std::stack<uint32_t> m_freeBlocks;

    const uint32_t m_totalBlocks     = 0;
    uint32_t m_activeBlockCount      = 0;
    uint32_t m_activeDirtyBlockCount = 0;

    const VkPipelineStageFlags2 m_vkDstStageMask  = VK_PIPELINE_STAGE_2_NONE;
    const VkAccessFlags2        m_vkDstAccessMask = VK_ACCESS_2_NONE;

    VulkanWrapper::Buffer* m_stagingBuffer = nullptr;
    VulkanWrapper::Buffer* m_ssboBuffer    = nullptr;

    VulkanWrapper::DeviceMemory* m_stagingMemory = nullptr;
    VulkanWrapper::DeviceMemory* m_ssboMemory    = nullptr;
public:
    BufferPool(const uint32_t blockCount, const uint32_t dirtyCount, const VkBufferUsageFlags vk_bufferUsage = 0x0, const VkPipelineStageFlags2 vk_dstStageMask = VK_PIPELINE_STAGE_2_NONE, const VkAccessFlags2 vk_dstAccessMask = VK_ACCESS_2_NONE);
    ~BufferPool();

    uint32_t acquireBlock();
    const T& getReadableBlock(const uint32_t id) const;
    T& getWritableBlock(const uint32_t id);
    void flushDirtyBlocks(const VkCommandBuffer vk_cmdBuff, const bool externallySyncrhonized);

    VkDescriptorBufferInfo getDescBufferInfo() const;

#ifdef DEBUG
    VkBuffer getStagingBufferHandle() const;
    VkBuffer getStorageBufferHandle() const;
#endif
};

}; // Core

#endif // MICA_BUFFER_POOL_HPP