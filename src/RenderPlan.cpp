#include <array>
#include <numeric>

#include "RenderPlan.hpp"
#include "Defines.hpp"

#include "vulkanwrapper/RenderPass.hpp"
#include "vulkanwrapper/Framebuffer.hpp"

#include "core/Attachment.hpp"

namespace init
{

std::vector<VkImageView> getAttachmentImageViews(std::vector<Core::Attachment*> attachments)
{
    std::vector<VkImageView> vk_imageViews;
    vk_imageViews.reserve(attachments.size());

    for (const auto& attachment : attachments)
    {
        vk_imageViews.push_back(attachment->getVkImageView());
    }

    return vk_imageViews;
}

VulkanWrapper::RenderPass* forward_createRenderPass(const RenderPlan::InitInfo& initInfo)
{
    // Attachments + Attachment References + Dependencies

    const std::array<VkAttachmentDescription2, 2> vk_attachmentDescriptions {{
        {   // DEFAULT COLOR
            .sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2,
            .format = initInfo.vk_attachmentFormats[0], 
            .samples = initInfo.vk_sampleCount,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
        },
        {   // DEPTH
            .sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2,
            .format = initInfo.vk_attachmentFormats[1], 
            .samples = initInfo.vk_sampleCount,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        }
    }};

    const std::array<VkAttachmentReference2, 2> vk_attachmentReferences {{
        {    // DEFAULT COLOR
            .sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2,
            .attachment = 0,
            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        },
        {   // DEPTH
            .sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2,
            .attachment = 1,
            .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
        }
    }};

    const std::array<VkSubpassDependency2, 2> vk_subpassDependencies {{
        {    // Implicit dependency (vkCmdBeginRenderPass)
            .sType = VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY_2,
            .srcSubpass = VK_SUBPASS_EXTERNAL,
            .dstSubpass = 0,
            .srcStageMask = VK_PIPELINE_STAGE_NONE,
            .dstStageMask = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
            .srcAccessMask = 0,
            .dstAccessMask = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT |
                            VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
                            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
                            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
            .dependencyFlags = 0,
        },
        {   // Implicit dependency (vkCmdEndRenderPass)
            .sType = VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY_2,
            .srcSubpass = 0,
            .dstSubpass = VK_SUBPASS_EXTERNAL,
            .srcStageMask = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_NONE,
            .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
                             VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
            .dstAccessMask = 0,
            .dependencyFlags = 0,
        },
    }};

    // Subpass Description - 0

    const std::array<VkAttachmentReference2, 1> vk_subpass0ColorAttachments {{
        vk_attachmentReferences[0]
    }};

    const VkAttachmentReference2 vk_subpass0DepthAttachment = vk_attachmentReferences[1];

    const VkSubpassDescription2 vk_subpass0Description {
        .sType = VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2,
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .viewMask = 0x0,
        .inputAttachmentCount = 0,
        .pInputAttachments = nullptr,
        .colorAttachmentCount = static_cast<uint32_t>(vk_subpass0ColorAttachments.size()),
        .pColorAttachments = vk_subpass0ColorAttachments.data(),
        .pResolveAttachments = nullptr,
        .pDepthStencilAttachment = &vk_subpass0DepthAttachment,
        .preserveAttachmentCount = 0,
        .pPreserveAttachments = nullptr,
    };

    // RenderPass 

    const std::array<VkSubpassDescription2, 1> vk_subpassDescriptions {{
        vk_subpass0Description
    }}; 

    const VkRenderPassCreateInfo2 vk_createInfo {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2,
        .attachmentCount = static_cast<uint32_t>(vk_attachmentDescriptions.size()),
        .pAttachments = vk_attachmentDescriptions.data(),
        .subpassCount = static_cast<uint32_t>(vk_subpassDescriptions.size()),
        .pSubpasses = vk_subpassDescriptions.data(),
        .dependencyCount = static_cast<uint32_t>(vk_subpassDependencies.size()),
        .pDependencies = vk_subpassDependencies.data(),
    };

    VulkanWrapper::RenderPass* renderPass = new VulkanWrapper::RenderPass(vk_createInfo);
    return renderPass;
}

std::vector<Core::Attachment*> forward_createAttachments(const RenderPlan::InitInfo& initInfo)
{
    std::vector<Core::Attachment*> attachments;

    { // DEFAULT COLOR

        const VkImageCreateInfo vk_imageCreateInfo {
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .imageType = VK_IMAGE_TYPE_2D,
            .format = initInfo.vk_attachmentFormats[0],
            .extent = { initInfo.vk_framebufferDimensions.width, initInfo.vk_framebufferDimensions.height, 1 },
            .mipLevels = 1,
            .arrayLayers = 1,
            .samples = initInfo.vk_sampleCount,
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices   = nullptr,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
        };

        VkImageViewCreateInfo vk_imageViewCreateInfo {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = VK_NULL_HANDLE,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = initInfo.vk_attachmentFormats[0],
            .components = {
                .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                .a = VK_COMPONENT_SWIZZLE_IDENTITY },
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            }
        };

        Core::Attachment* attachment = new Core::Attachment(vk_imageCreateInfo, vk_imageViewCreateInfo);
        attachments.push_back(attachment);
    }

    { // DEPTH

        const VkImageCreateInfo vk_imageCreateInfo {
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .imageType = VK_IMAGE_TYPE_2D,
            .format = initInfo.vk_attachmentFormats[1],
            .extent = { initInfo.vk_framebufferDimensions.width, initInfo.vk_framebufferDimensions.height, 1 },
            .mipLevels = 1,
            .arrayLayers = 1,
            .samples = initInfo.vk_sampleCount,
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices   = nullptr,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        };

        VkImageViewCreateInfo vk_imageViewCreateInfo {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = VK_NULL_HANDLE,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = initInfo.vk_attachmentFormats[1],
            .components = {
                .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                .a = VK_COMPONENT_SWIZZLE_IDENTITY },
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            }
        };
    
        Core::Attachment* attachment = new Core::Attachment(vk_imageCreateInfo, vk_imageViewCreateInfo);
        attachments.push_back(attachment);
    }

    return attachments;
}

VulkanWrapper::Framebuffer* forward_createFramebuffers(const RenderPlan::InitInfo& initInfo, const VkRenderPass vk_renderPass, const std::vector<VkImageView> vk_attachmentImageViews)
{
    const VkFramebufferCreateInfo vk_createInfo {
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .renderPass = vk_renderPass,
        .attachmentCount = static_cast<uint32_t>(vk_attachmentImageViews.size()),
        .pAttachments = vk_attachmentImageViews.data(),
        .width = initInfo.vk_framebufferDimensions.width,
        .height = initInfo.vk_framebufferDimensions.height,
        .layers = 1,
    };   

    VulkanWrapper::Framebuffer* framebuffer = new VulkanWrapper::Framebuffer(vk_createInfo);
    return framebuffer;
};

}; // init

RenderPlan::CoreResources RenderPlan::loadPlan(const InitInfo& initInfo)
{
    RenderPlan::CoreResources coreResources {};

    switch (initInfo.plan)
    {
        case RenderPlanType::eForward:
        {
            coreResources.renderPass  = init::forward_createRenderPass(initInfo);
            coreResources.attachments = init::forward_createAttachments(initInfo);

            std::vector<VkImageView> vk_attachmentImageViews = init::getAttachmentImageViews(coreResources.attachments);
            coreResources.framebuffer = init::forward_createFramebuffers(initInfo, coreResources.renderPass->vk_handle, vk_attachmentImageViews);

            return coreResources;
        }
    };

    return coreResources;
}

void RenderPlan::unloadPlan(const RenderPlanType plan, RenderPlan::CoreResources& resources)
{
    switch (plan)
    {
        case RenderPlanType::eForward:
        {
            for (Core::Attachment* attachment : resources.attachments)
            {
                delete attachment;
                attachment = nullptr;
            }

            delete resources.framebuffer;
            delete resources.renderPass;

            resources.renderPass = nullptr;
            resources.framebuffer = nullptr;
        }
    };
}



RenderPlan::RenderPlan(const InitInfo& initInfo)
    : plan { initInfo.plan }
    , coreResources { loadPlan(initInfo) }
    , vk_clearValues { initInfo.vk_clearValues }
{
    vk_renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    vk_renderPassBeginInfo.pNext = nullptr;
    vk_renderPassBeginInfo.renderPass = coreResources.renderPass->vk_handle;
    vk_renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(vk_clearValues.size());
    vk_renderPassBeginInfo.pClearValues = vk_clearValues.data();
}

RenderPlan::~RenderPlan()
{
   unloadPlan(plan, coreResources); 
}

void RenderPlan::registerPipeline(const uint32_t pipelineID, const VkPipeline vk_pipeline)
{
    auto iter = pipelineBins.find(pipelineID);
    if (iter != pipelineBins.end())
    {
        EXIT("Attemting to add the same material twice to the renderer.\n");
    }

    const RenderBin_Pipeline bin {
        .pipelineID  = pipelineID,
        .vk_pipeline = vk_pipeline,
    };

    auto pipeIter = pipelineBins.insert({ pipelineID, std::move(bin) });
    
    // auto matIter = materialBins.find(materialID);
    // if (matIter == materialBins.end())
    // {
    //     EXIT("Attempting to add pipeline to renderer when associated material does not exist!\n");
    // }

    // matIter->second.pipelineBins.insert({ materialID, &pipeIter.first->second });
}

void RenderPlan::addDraw(const DrawInfo& drawInfo)
{
    auto iter = pipelineBins.find(drawInfo.pipelineID);
    if (iter == pipelineBins.end())
    {
        EXIT("Attempting to add renderable to a non-registered pipeline!\n");
    }

    iter->second.renderables.push_back({
        .indexCount = drawInfo.indexCount,
        .instanceCount = drawInfo.instanceCount,
        .firstIndex = drawInfo.firstIndex,
        .vertexOffset = drawInfo.vertexOffset,
        .firstInstance = drawInfo.drawID,
        .vertexCount = drawInfo.vertexCount,
    });
}


void RenderPlan::execute(const VkCommandBuffer vk_cmdBuff, const VkRect2D vk_renderArea)
{
    vk_renderPassBeginInfo.framebuffer = coreResources.framebuffer->vk_handle;
    vk_renderPassBeginInfo.renderArea = vk_renderArea;

    const VkSubpassBeginInfo vk_subpassBeginInfo {
        .sType = VK_STRUCTURE_TYPE_SUBPASS_BEGIN_INFO,
        .contents = VK_SUBPASS_CONTENTS_INLINE,
    };

    const VkSubpassEndInfo vk_subpassEndInfo {
        .sType = VK_STRUCTURE_TYPE_SUBPASS_END_INFO
    };

    vkCmdBeginRenderPass2(vk_cmdBuff, &vk_renderPassBeginInfo, &vk_subpassBeginInfo);

    // bind 0

    for (auto& [pipelineID, pipelineBin] : pipelineBins) // active pipelines which made it past cull
    {
        if (pipelineBin.renderables.empty())
        {
            continue;
        }

        // bind pipeline

        vkCmdBindPipeline(vk_cmdBuff, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineBin.vk_pipeline);

        for (const Renderable& renderable : pipelineBin.renderables) // active renderables which made it past cull
        {
            // draw

            if (renderable.indexCount != 0)
            {
                vkCmdDrawIndexed(vk_cmdBuff, 
                                renderable.indexCount,
                                renderable.instanceCount,  // 1 for all non-instanced models
                                renderable.firstIndex,
                                renderable.vertexOffset,
                                renderable.firstInstance); // index into structure containing draw + mat ID
            }
            else
            {
                vkCmdDraw(vk_cmdBuff,
                        renderable.vertexCount,
                        renderable.instanceCount,  // 1 for all non-instanced models
                        renderable.vertexOffset,
                        renderable.firstInstance); // index into structure containing draw + mat ID
            }
        }

        pipelineBin.renderables.clear();
    }

    vkCmdEndRenderPass2(vk_cmdBuff, &vk_subpassEndInfo);
}


VkImage RenderPlan::getAttachmentImage(const uint32_t idx)
{
    return coreResources.attachments[idx]->getVkImage();
}



VkRenderPass RenderPlan::getRenderPass() const
{
    return coreResources.renderPass->vk_handle;
}