#ifndef MICA_SCENE_BUFFER_HPP
#define MICA_SCENE_BUFFER_HPP

#include <vector>
#include <stack>
#include <unordered_map>

#include <vulkan/vulkan.h>

#include "Resource.hpp"

// When loading, we could load vertex data directly into staging buffer, bu

class Buffer;
class GPUDrawCommand;
class BufferPool;

struct MeshInfo
{
    uint32_t m_uIndexCount = 0;
    uint32_t m_uFirstIndex = 0;
    int32_t  m_iVertexOffset = 0;
};

class SceneBuffer : public Resource
{
private:
    const VkDeviceSize m_vkVertexStride = 0;
    const VkDeviceSize m_vkStagingBufferSize = 0;
    const VkDeviceSize m_vkVertexBufferSize = 0;
    const VkDeviceSize m_vkIndexBufferSize = 0;
    
    Buffer* m_stagingBuffer = nullptr;
    Buffer* m_vertexBuffer = nullptr;
    Buffer* m_indexBuffer = nullptr;

    VkDeviceSize m_vkStagingBufferOffset = 0;
    VkDeviceSize m_vkVertexBufferOffset = 0;
    VkDeviceSize m_vkIndexBufferOffset = 0;

    void* m_mappedStagingPtr = nullptr;

    std::vector<VkBufferCopy> m_vkVertexBufferCopies;
    std::vector<VkBufferCopy> m_vkIndexBufferCopies;


    std::unordered_map<uint32_t, MeshInfo> m_meshInfos;


    int32_t queueVertexUpload(const VkDeviceSize vk_size, const void* pData); 
    uint32_t queueIndexUpload(const VkDeviceSize vk_size, const uint32_t* pData);

public:
    SceneBuffer(const VkDeviceSize vk_vertexStride, const VkDeviceSize vk_vertexBufferSize, const VkDeviceSize vk_indexBufferSize, const VkDeviceSize vk_stagingBufferSize);
    ~SceneBuffer();

    MeshInfo queueUpload(const uint32_t id, const VkDeviceSize vk_vertexSize, const void* pVertexData, const VkDeviceSize vk_indexSize, const uint32_t* pIndexData);

    void flushQueuedUploads(const VkCommandBuffer vk_cmdBuff);

    const MeshInfo& getMeshInfo(const uint32_t id) const;

    const VkBuffer getVertexBuffer() const;
    const VkBuffer getIndexBuffer() const;
};

#endif // MICA_SCENE_BUFFER_HPP