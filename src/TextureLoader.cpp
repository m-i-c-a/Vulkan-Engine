#include "core/BufferPool.hpp"
#include "core/PersistentDeviceBuffer.hpp"
#include "core/PersistentStagingBuffer.hpp"
// #include "core/SceneBuffer.hpp"
#include "core/Texture.hpp"

#include "vulkanwrapper/Buffer.hpp"
#include "vulkanwrapper/CommandBuffer.hpp"
#include "vulkanwrapper/CommandPool.hpp"
#include "vulkanwrapper/DescriptorPool.hpp"
#include "vulkanwrapper/Fence.hpp"
#include "vulkanwrapper/Image.hpp"
#include "vulkanwrapper/ImageView.hpp"
#include "vulkanwrapper/Sampler.hpp"
#include "vulkanwrapper/ShaderModule.hpp"

void createTexture()
{
    const VkFormat vk_format   = VK_FORMAT_R8G8B8_SNORM;
    const uint32_t mipCount    = 1;
    const uint32_t layerCount  = 1;
    const VkExtent3D vk_extent = {};
    const VkImageType vk_imageType = VK_IMAGE_TYPE_2D;
    const VkImageViewType vk_imageViewType = VK_IMAGE_VIEW_TYPE_2D;

    const VkImageCreateInfo vk_imageCreateInfo {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = vk_imageType,
        .format = vk_format,
        .extent = vk_extent,
        .mipLevels = mipCount,
        .arrayLayers = layerCount,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .initialLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
    };

    VkImageViewCreateInfo vk_imageViewCreateInfo {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .viewType = vk_imageViewType,
        .format = vk_format,
        .components = {
            .r = VK_COMPONENT_SWIZZLE_IDENTITY,
            .g = VK_COMPONENT_SWIZZLE_IDENTITY,
            .b = VK_COMPONENT_SWIZZLE_IDENTITY,
            .a = VK_COMPONENT_SWIZZLE_IDENTITY },
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = mipCount,
            .baseArrayLayer = 0,
            .layerCount = layerCount,
        }
    };

    // Choose sampler from global sampler pool based on params

    VulkanWrapper::Sampler* sampler;
    Core::Texture* texture = new Core::Texture(vk_imageCreateInfo, vk_imageViewCreateInfo, sampler);
}


void loadTexture(const char* filename)
{
    // read file data / add to staging

    // once file data is processed 
    
    createTexture();
}
