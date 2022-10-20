#ifndef MICA_CORE_PERSISTENT_DEVICE_BUFFER_HPP
#define MICA_CORE_PERSISTENT_DEVICE_BUFFER_HPP

#include <vector>

#include <vulkan/vulkan.h>

#include "GenericBuffer.hpp"

class PersistentStagingBuffer;

class PersistentDeviceBuffer : public GenericBuffer
{
private:
    friend class PersistentStagingBuffer;

    VkBuffer     m_vkStagingBuffer = VK_NULL_HANDLE;
    VkDeviceSize m_vkStagingOffset = 0;
    char*        m_pStagingData    = nullptr;

    std::vector<VkBufferCopy> vk_bufferCopies;

    PersistentDeviceBuffer(const VkDeviceSize vk_size, const VkBufferUsageFlags vk_bufferUsageFlags, const VkMemoryPropertyFlags vk_memProps); //, const VkBuffer vk_stagingBuffer, const VkDeviceSize vk_stagingOffset, char* pData);
    void setStagingInfo(const VkBuffer vk_stagingBuffer, const VkDeviceSize vk_stagingOffset, char* pData);
public:
    const VkDeviceSize m_vkSize = 0;

    void update(const VkDeviceSize vk_offset, const VkDeviceSize vk_size, const void* pData);
    void flush(const VkCommandBuffer vk_cmdBuff);
};

#endif // MICA_CORE_PERSISTENT_DEVICE_BUFFER_HPP