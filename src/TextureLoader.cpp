#include "TextureLoader.hpp"
#include "Defines.hpp"

#include "core/Texture.hpp"

#include "vulkanwrapper/Buffer.hpp"
#include "vulkanwrapper/Image.hpp"
#include "vulkanwrapper/ImageView.hpp"
#include "vulkanwrapper/Sampler.hpp"

#include <ktx.h>
#include <ktxvulkan.h>

struct TextureInitInfo 
{
    const VkFormat vk_format   = VK_FORMAT_R8G8B8_SNORM;
    const uint32_t mipCount    = 1;
    const uint32_t layerCount  = 1;
    const VkExtent3D vk_extent = {};
    const VkImageType vk_imageType = VK_IMAGE_TYPE_2D;
    const VkImageViewType vk_imageViewType = VK_IMAGE_VIEW_TYPE_2D;
};

void createTexture(const TextureInitInfo& initInfo)
{
    const VkImageCreateInfo vk_imageCreateInfo {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = initInfo.vk_imageType,
        .format = initInfo.vk_format,
        .extent = initInfo.vk_extent,
        .mipLevels = initInfo.mipCount,
        .arrayLayers = initInfo.layerCount,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .initialLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
    };

    VkImageViewCreateInfo vk_imageViewCreateInfo {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .viewType = initInfo.vk_imageViewType,
        .format = initInfo.vk_format,
        .components = {
            .r = VK_COMPONENT_SWIZZLE_IDENTITY,
            .g = VK_COMPONENT_SWIZZLE_IDENTITY,
            .b = VK_COMPONENT_SWIZZLE_IDENTITY,
            .a = VK_COMPONENT_SWIZZLE_IDENTITY },
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = initInfo.mipCount,
            .baseArrayLayer = 0,
            .layerCount = initInfo.layerCount,
        }
    };

    // Choose sampler from global sampler pool based on params

    VulkanWrapper::Sampler* sampler = nullptr;
    Core::Texture* texture = new Core::Texture(vk_imageCreateInfo, vk_imageViewCreateInfo, sampler);
}



bool getImageType(const uint32_t numDimensions, VkImageType& vk_imageType, VkImageViewType& vk_imageViewType)
{
    switch (numDimensions)
    {
        case 1:
            vk_imageType = VK_IMAGE_TYPE_1D;
            vk_imageViewType = VK_IMAGE_VIEW_TYPE_1D;
            break;
        case 2:
            vk_imageType = VK_IMAGE_TYPE_2D;
            vk_imageViewType = VK_IMAGE_VIEW_TYPE_1D;
            break;
        case 3:
            vk_imageType = VK_IMAGE_TYPE_3D;
            vk_imageViewType = VK_IMAGE_VIEW_TYPE_1D;
            break;
        default:
            return false;
    }

    return true;
}

void loadTexture(const char* filename)
{
    ktxTexture* tex = nullptr;
    const ktxResult result = ktxTexture_CreateFromNamedFile(filename, KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &tex);

    if (result != KTX_SUCCESS)
    {
        EXIT("Failed to load texture %s.\n", filename);
    }

    VkImageType vk_imageType         = VK_IMAGE_TYPE_MAX_ENUM;
    VkImageViewType vk_imageViewType = VK_IMAGE_VIEW_TYPE_MAX_ENUM;
    const bool validImageType = getImageType(tex->numDimensions, vk_imageType, vk_imageViewType);

    if (!validImageType)
    {
        EXIT("Failed to load texture %s - invalid # of dimensions %u\n", filename, tex->numDimensions);
    }



    // get staging pointer (incrementing internal atomic staging internals)

    // copy to staging pointer

    const TextureInitInfo initInfo {
        .vk_format = ktxTexture_GetVkFormat(tex),
        .mipCount = static_cast<uint32_t>(tex->numLevels),
        .layerCount = static_cast<uint32_t>(tex->numLayers),
        .vk_extent = {
            .width = static_cast<uint32_t>(tex->baseWidth),
            .height = static_cast<uint32_t>(tex->baseHeight),
            .depth = static_cast<uint32_t>(tex->baseDepth)
        },
        .vk_imageType = vk_imageType,
        .vk_imageViewType = vk_imageViewType
    };

    createTexture(initInfo);


    ktx_uint8_t* data = ktxTexture_GetData(tex);
    ktx_size_t size; // = ktxTexture_GetSize(tex);

    VkBufferImageCopy vk_bufferImageCopy {
        .bufferOffset = 0,
        .bufferRowLength = 0,
        .bufferImageHeight = 0,
        .imageSubresource = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .mipLevel = 0,
            .baseArrayLayer = 0,
            .layerCount = 1,
        },
        .imageOffset = { .x = 0, .y = 0, .z = 0 },
        .imageExtent = initInfo.vk_extent
    };

    ktxTexture_Destroy(ktxTexture);
}
