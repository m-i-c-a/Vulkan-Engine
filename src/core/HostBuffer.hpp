#ifndef MICA_CORE_HOST_BUFFER_HPP
#define MICA_CORE_HOST_BUFFER_HPP

#include <vulkan/vulkan.h>

#include "GenericBuffer.hpp"

class PersistentStagingBuffer;

class HostBuffer : public GenericBuffer
{
private:
    friend class PersistentStagingBuffer;
    void* m_mappedPtr = nullptr;
public:
    HostBuffer(const VkDeviceSize vk_size, const VkBufferUsageFlags vk_bufferUsageFlags, const VkMemoryPropertyFlags vk_memProps);
    ~HostBuffer();

    void update(const VkDeviceSize vk_offset, const VkDeviceSize vk_size, const void* pData);
};

#endif // MICA_CORE_HOST_BUFFER_HPP