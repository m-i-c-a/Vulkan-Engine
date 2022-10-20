#ifndef MICA_BUFFER_COMMON_HPP
#define MICA_BUFFER_COMMON_HPP

#include <vulkan/vulkan.h>

struct QueuedBufferUpdate
{
    VkBufferCopy m_vkBufferCopy;
    void* m_pData;
};

#endif // MICA_BUFFER_COMMON_HPP