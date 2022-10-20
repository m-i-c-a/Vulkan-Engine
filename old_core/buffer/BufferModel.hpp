#include <vulkan/vulkan.h>

#include <vector>

#include "Buffer.hpp"
#include "BufferCommon.hpp"

class StagingBuffer;

namespace VulkanWrapper
{
    class Buffer;
};

namespace BufferModel
{

class Device
{
private:
    friend class Buffer;
    static const VkMemoryPropertyFlags vk_memProps = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    VkBuffer m_vkBuffer = VK_NULL_HANDLE;
    std::vector<QueuedBufferUpdate> m_queuedUpdates;
public:
    void queueUpdate(const VkDeviceSize vk_offset, const VkDeviceSize vk_size, const void* pData);
    bool hasUpdatesQueued() const;
    void flushQueuedUpdates(StagingBuffer* stagingBuffer);

    void init(const VkDevice vk_device, VulkanWrapper::Buffer* buffer);
    void destroy(const VkDevice vk_device, VulkanWrapper::Buffer* buffer);
};

struct Host
{
    friend class Buffer;
    static const VkMemoryPropertyFlags vk_memProps = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT  |
                                                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT ;

    void* m_mappedPtr = nullptr;

    void update(const VkDeviceSize vk_offset, const VkDeviceSize vk_size, const void* pData);
    
    void init(const VkDevice vk_device, VulkanWrapper::Buffer* buffer);
    void destroy(const VkDevice vk_device, VulkanWrapper::Buffer* buffer);
};

}; // BufferModel
