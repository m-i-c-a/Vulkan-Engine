#ifndef MICA_CORE_STAGING_BUFFER_HPP 
#define MICA_CORE_STAGING_BUFFER_HPP

#include <vulkan/vulkan.h>

#include <vector>
#include <unordered_map> 
#include <atomic>

namespace VulkanWrapper
{

class Buffer;
class DeviceMemory;

}; // VulkanWrapper

namespace Core
{

class StagingBuffer
{
private:
    const VkDeviceSize m_vkSize = 0;
    const uint32_t m_batchCount = 0;

    std::atomic<VkDeviceSize> m_vkOffset = 0;
    std::atomic<uint32_t> m_batchIdx     = 0;

    struct UploadBatch
    {
        std::vector<VkBufferMemoryBarrier2> m_vkPreCopyBufferMemoryBarriers;
        std::vector<VkBufferMemoryBarrier2> m_vkPostCopyBufferMemoryBarriers;
        std::unordered_map<VkBuffer, std::vector<VkBufferCopy2>> m_vkBufferCopyMap;

        std::vector<VkImageMemoryBarrier2> m_vkPreCopyImageMemoryBarriers;
        std::vector<VkImageMemoryBarrier2> m_vkPostCopyImageMemoryBarriers;
        std::unordered_map<VkImage, std::vector<VkBufferImageCopy2>> m_vkBufferImageCopyMap;
    };

    std::vector<UploadBatch> m_batches;

    VulkanWrapper::Buffer*       m_buffer = nullptr;
    VulkanWrapper::DeviceMemory* m_memory = nullptr;
    void* m_pMappedData                   = nullptr;


public:

    StagingBuffer(const VkDeviceSize vk_size, const uint32_t batchCount);
    ~StagingBuffer();

    struct BarrierInfo
    {
        VkImageLayout         vk_currentImageLayout     = VK_IMAGE_LAYOUT_MAX_ENUM;
        VkImageLayout         vk_imageLayoutAfterUpload = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL;
        VkPipelineStageFlags2 vk_srcStageMask           = VK_PIPELINE_STAGE_2_NONE;
        VkAccessFlags2        vk_srcAccessMask          = VK_ACCESS_2_NONE;
        VkPipelineStageFlags2 vk_dstStageMask           = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
        VkAccessFlags2        vk_dstAccessMask          = VK_ACCESS_2_SHADER_READ_BIT;
    };

    void uploadData(const VkImage vk_image, VkBufferImageCopy2 vk_bufferImageCopy2, const BarrierInfo& barrierInfo, const VkDeviceSize vk_size, void* data);
    void uploadData(const VkBuffer vk_buffer, VkBufferCopy2 vk_bufferCopy2, const void* const data);

    void flush(const VkCommandBuffer vk_cmdBuff);
};

}; // Core

#endif // MICA_CORE_STAGING_BUFFER_HPP