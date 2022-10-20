#ifndef MICA_CORE_STAGING_BUFFER_HPP
#define MICA_CORE_STAGING_BUFFER_HPP

#include <string.h>

#include <vector>
#include <unordered_map>
#include <atomic>

#include <vulkan/vulkan.h>

class HostBuffer;
class PersistentDeviceBuffer;

class PersistentStagingBuffer
{
private:
    HostBuffer*  m_buffer   = nullptr;
    VkDeviceSize m_vkSize   = 0;
    std::vector<PersistentDeviceBuffer*> m_registeredDeviceBuffers;
public:
    PersistentStagingBuffer();
    ~PersistentStagingBuffer();

    PersistentDeviceBuffer* registerDeviceBuffer(const VkDeviceSize vk_size, const VkBufferUsageFlags vk_bufferUsageFlags, const VkMemoryPropertyFlags vk_memProps);
    void completeRegistration();

    VkBuffer vkBuffer() const;
};

#endif // MICA_STAGING_BUFFER_HPP