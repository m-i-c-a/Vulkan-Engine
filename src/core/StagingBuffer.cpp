#include "StagingBuffer.hpp"

#include "Defines.hpp"

#include "vulkanwrapper/Buffer.hpp"
#include "vulkanwrapper/DeviceMemory.hpp"

#include <string.h>

namespace Core
{

StagingBuffer::StagingBuffer(const VkDeviceSize vk_size, const uint32_t batchCount)
    : m_vkSize     { vk_size }
    , m_batchCount { batchCount }
{
    m_buffer = new VulkanWrapper::Buffer(vk_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    m_memory = new VulkanWrapper::DeviceMemory(m_buffer->vk_handle, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    m_pMappedData = m_memory->map(0, vk_size);

    m_batches.resize(batchCount);
}

StagingBuffer::~StagingBuffer()
{
    m_memory->unmap();
    m_pMappedData = nullptr;

    delete m_memory;
    delete m_buffer;
}


void StagingBuffer::uploadData(const VkBuffer vk_buffer, VkBufferCopy2 vk_bufferCopy2, const void* const data)
{
    VkPipelineStageFlags2 vk_srcStageMask = VK_PIPELINE_STAGE_2_NONE;
    VkAccessFlags2 vk_srcAccessMask = VK_ACCESS_2_NONE;
    VkPipelineStageFlags2 vk_dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    VkAccessFlags2 vk_dstAccessMask = VK_ACCESS_2_MEMORY_READ_BIT;

    constexpr bool isUnifiedGraphicsAndTransferQueue = true;

    const VkDeviceSize vk_availableSize = std::max(0ul, m_vkSize - m_vkOffset.fetch_add(vk_bufferCopy2.size));  
    const VkDeviceSize vk_localOffset = m_vkSize - vk_availableSize;

    const bool willProcessEntireUpload = vk_availableSize >= vk_bufferCopy2.size;
    const bool willProcessPartialUpload = !willProcessEntireUpload && vk_availableSize > 0;
    const bool cannotProcessUpload = vk_availableSize == 0;

    if (willProcessEntireUpload)
    {
        memcpy((uint8_t*)m_pMappedData + vk_localOffset, data, vk_bufferCopy2.size);

        vk_bufferCopy2.srcOffset = vk_localOffset;

        if (isUnifiedGraphicsAndTransferQueue)
        {
            const VkBufferMemoryBarrier2 vk_preCopyBufferMemoryBarrier2 {
                .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
                .srcStageMask = vk_srcStageMask,
                .srcAccessMask = vk_srcAccessMask,
                .dstStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                .dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT,
                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .buffer = vk_buffer,
                .offset = vk_bufferCopy2.dstOffset,
                .size = vk_bufferCopy2.size,
            };

            const VkBufferMemoryBarrier2 vk_postCopyBufferMemoryBarrier2 {
                .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
                .srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                .srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,
                .dstStageMask = vk_dstStageMask,
                .dstAccessMask = vk_dstAccessMask,
                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .buffer = vk_buffer,
                .offset = vk_bufferCopy2.dstOffset,
                .size = vk_bufferCopy2.size,
            };

            // --- Critical Section Begin

            auto& activeBatch = m_batches[m_batchIdx];

            activeBatch.m_vkPreCopyBufferMemoryBarriers.push_back(std::move(vk_preCopyBufferMemoryBarrier2));
            activeBatch.m_vkPostCopyBufferMemoryBarriers.push_back(std::move(vk_postCopyBufferMemoryBarrier2));

            auto& copies = activeBatch.m_vkBufferCopyMap[vk_buffer];
            copies.push_back(vk_bufferCopy2);

            // --- Critical Section End
        }
        else
        {
            EXIT("Non-unified transfer/graphics queue is currently not supported!\n");
        }

        return;
    }

    if (willProcessPartialUpload)
    {
        EXIT("Partial upload currently not supported!\n");
        return;
    }

    if (cannotProcessUpload)
    {
        EXIT("Queuig uploads currently not supported!\n");
        return;
    }
}

void StagingBuffer::uploadData(const VkImage vk_image, VkBufferImageCopy2 vk_bufferImageCopy2, const VkDeviceSize vk_size, const void* const data)
{
    VkImageLayout vk_currentImageLayout = VK_IMAGE_LAYOUT_MAX_ENUM;
    VkImageLayout vk_imageLayoutAfterUpload = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL;
    VkPipelineStageFlags2 vk_srcStageMask = VK_PIPELINE_STAGE_2_NONE;
    VkAccessFlags2 vk_srcAccessMask = VK_ACCESS_2_NONE;
    VkPipelineStageFlags2 vk_dstStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
    VkAccessFlags2 vk_dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT;

    constexpr bool isUnifiedGraphicsAndTransferQueue = true;

    const VkDeviceSize vk_availableSize = std::max(0ul, m_vkSize - m_vkOffset.fetch_add(vk_size));  
    const VkDeviceSize vk_localOffset = m_vkSize - vk_availableSize;

    const bool willProcessEntireUpload = vk_availableSize >= vk_size;
    const bool willProcessPartialUpload = !willProcessEntireUpload && vk_availableSize > 0;
    const bool cannotProcessUpload = vk_availableSize == 0;

    if (willProcessEntireUpload)
    {
        memcpy((uint8_t*)m_pMappedData + vk_localOffset, data, vk_size);

        vk_bufferImageCopy2.bufferOffset = vk_localOffset;

        if (isUnifiedGraphicsAndTransferQueue)
        {
            const VkImageSubresourceRange vk_imageSubresourceRange {
                .aspectMask = vk_bufferImageCopy2.imageSubresource.aspectMask,
                .baseMipLevel = vk_bufferImageCopy2.imageSubresource.mipLevel,
                .levelCount = 1,
                .baseArrayLayer = vk_bufferImageCopy2.imageSubresource.baseArrayLayer,
                .layerCount = vk_bufferImageCopy2.imageSubresource.layerCount,
            };

            const VkImageMemoryBarrier2 vk_preCopyImageMemoryBarrier2 {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
                .srcStageMask = vk_srcStageMask,
                .srcAccessMask = vk_srcAccessMask,
                .dstStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                .dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT,
                .oldLayout = vk_currentImageLayout,
                .newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .image = vk_image,
                .subresourceRange = vk_imageSubresourceRange,
            };

            const VkImageMemoryBarrier2 vk_postCopyImageMemoryBarrier2 {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
                .srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                .srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,
                .dstStageMask = vk_dstStageMask,
                .dstAccessMask = vk_dstAccessMask,
                .oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                .newLayout = vk_imageLayoutAfterUpload,
                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .image = vk_image,
                .subresourceRange = vk_imageSubresourceRange,
            };

            // --- Critical Section Begin

            auto& activeBatch = m_batches[m_batchIdx];

            activeBatch.m_vkPreCopyImageMemoryBarriers.push_back(std::move(vk_preCopyImageMemoryBarrier2));
            activeBatch.m_vkPostCopyImageMemoryBarriers.push_back(std::move(vk_postCopyImageMemoryBarrier2));

            auto& copies = activeBatch.m_vkBufferImageCopyMap[vk_image];
            copies.push_back(vk_bufferImageCopy2);

            // --- Critical Section End
        }
        else
        {
            EXIT("Non-unified transfer/graphics queue is currently not supported!\n");
        }

        return;
    }

    if (willProcessPartialUpload)
    {
        EXIT("Partial upload currently not supported!\n");
        return;
    }

    if (cannotProcessUpload)
    {
        EXIT("Queuig uploads currently not supported!\n");
        return;
    }
}

void StagingBuffer::flush(const VkCommandBuffer vk_cmdBuff)
{
    // --- Critical Section Begin

    if (m_batches[m_batchIdx].m_vkBufferImageCopyMap.empty())
        return;

    const uint32_t oldBatchIdx = m_batchIdx.fetch_add(1) % m_batchCount; 

    // --- Critical Section End 

    auto& activeBatch = m_batches[oldBatchIdx];

    const VkDependencyInfo vk_preCopyDependencyInfo {
        .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        .dependencyFlags = 0x0,
        .memoryBarrierCount = 0,
        .pMemoryBarriers = nullptr,
        .bufferMemoryBarrierCount = 0,
        .pBufferMemoryBarriers = nullptr,
        .imageMemoryBarrierCount = static_cast<uint32_t>(activeBatch.m_vkPreCopyImageMemoryBarriers.size()),
        .pImageMemoryBarriers = activeBatch.m_vkPreCopyImageMemoryBarriers.data(),
    };

    const VkDependencyInfo vk_postCopyDependencyInfo {
        .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        .dependencyFlags = 0x0,
        .memoryBarrierCount = 0,
        .pMemoryBarriers = nullptr,
        .bufferMemoryBarrierCount = 0,
        .pBufferMemoryBarriers = nullptr,
        .imageMemoryBarrierCount = static_cast<uint32_t>(activeBatch.m_vkPostCopyImageMemoryBarriers.size()),
        .pImageMemoryBarriers = activeBatch.m_vkPostCopyImageMemoryBarriers.data(),
    };

    vkCmdPipelineBarrier2(vk_cmdBuff, &vk_preCopyDependencyInfo);

    for (const auto& [vk_image, copies] : activeBatch.m_vkBufferImageCopyMap)
    {
        const VkCopyBufferToImageInfo2 vk_copyInfo {
            .sType = VK_STRUCTURE_TYPE_COPY_BUFFER_TO_IMAGE_INFO_2,
            .srcBuffer = m_buffer->vk_handle,
            .dstImage = vk_image,
            .dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            .regionCount = static_cast<uint32_t>(copies.size()),
            .pRegions = copies.data(),
        };

        vkCmdCopyBufferToImage2(vk_cmdBuff, &vk_copyInfo);
    }

    vkCmdPipelineBarrier2(vk_cmdBuff, &vk_postCopyDependencyInfo);

    activeBatch.m_vkBufferImageCopyMap.clear();
}

}; // Core