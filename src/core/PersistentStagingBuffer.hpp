#ifndef MICA_CORE_PERSISTENT_STAGING_BUFFER_HPP
#define MICA_CORE_PERSISTENT_STAGING_BUFFER_HPP

#include <vector>

#include <vulkan/vulkan.h>

class HostBuffer;
class PersistentDeviceBuffer;

namespace VulkanWrapper
{

class Buffer;
class DeviceMemory;

};


class PersistentStagingBuffer
{
private:
    VulkanWrapper::Buffer*       m_buffer = nullptr;
    VulkanWrapper::DeviceMemory* m_memory = nullptr;

    VkDeviceSize m_vkSize   = 0;
    std::vector<PersistentDeviceBuffer*> m_registeredDeviceBuffers;
public:
    PersistentStagingBuffer();
    ~PersistentStagingBuffer();

    PersistentDeviceBuffer* registerDeviceBuffer(const VkDeviceSize vk_size, const VkBufferUsageFlags vk_bufferUsageFlags, const VkMemoryPropertyFlags vk_memProps);
    void completeRegistration();

    VkBuffer vkBuffer() const;
};

#endif // MICA_CORE_PERSISTENT_STAGING_BUFFER_HPP