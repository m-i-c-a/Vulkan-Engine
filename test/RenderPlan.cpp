#include <array>
#include <numeric>

#include "RenderPlan.hpp"


namespace init
{

VkRenderPass forward_createRenderPass(const RenderPlanInitInfo& initInfo)
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
            .initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
        },
        {   // DEPTH
            .sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2,
            .format = initInfo.vk_attachmentFormats[1], 
            .samples = initInfo.vk_sampleCount,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
            .finalLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
        }
    }};

    const std::array<VkAttachmentReference2, 2> vk_attachmentReferences {{
        {    // DEFAULT COLOR
            .sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2,
            .attachment = 0,
            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        },
        {   // DEPTH
            .sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2,
            .attachment = 1,
            .layout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
            .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
        }
    }};

    const std::array<VkSubpassDependency2, 2> vk_subpassDependencies {{
        {    // Implicit dependency (vkCmdBeginRenderPass)
            .sType = VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY_2,
            .srcSubpass = VK_SUBPASS_EXTERNAL,
            .dstSubpass = 0,
            .srcStageMask = VK_PIPELINE_STAGE_NONE,
            .dstStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
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
            .srcStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
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

    VkDevice vk_device;

    VkRenderPass vk_renderPass = VK_NULL_HANDLE;
    vkCreateRenderPass2(vk_device, &vk_createInfo, nullptr, &vk_renderPass);
    return vk_renderPass;
}

std::vector<Core::Attachment> forward_createAttachments(const RenderPlanInitInfo& initInfo)
{
    std::vector<Core::Attachment> attachments;

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
            .usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices   = nullptr,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        };

        const VkImageViewCreateInfo vk_imageViewCreateInfo {
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
    
        attachments.emplace_back(vk_imageCreateInfo, vk_imageViewCreateInfo);
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

        const VkImageViewCreateInfo vk_imageViewCreateInfo {
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
    
        attachments.emplace_back(vk_imageCreateInfo, vk_imageViewCreateInfo);
    }

    return attachments;
}

std::vector<VkFramebuffer> forward_createFramebuffers(const RenderPlanInitInfo& initInfo, const VkRenderPass vk_renderPass, const std::vector<Core::Attachment>& attachments)
{
    const std::vector<VkImageView> vk_attachmentImageViews = [&](){
        std::vector<VkImageView> vk_imageViews;
        vk_imageViews.reserve(attachments.size());

        for (const auto& attachment : attachments)
        {
            vk_imageViews.push_back(attachment.getVkImageView());
        }

        return vk_imageViews;
    }();

    const VkFramebufferCreateInfo vk_framebufferCreateInfo {
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .renderPass = vk_renderPass,
        .attachmentCount = static_cast<uint32_t>(vk_attachmentImageViews.size()),
        .pAttachments = vk_attachmentImageViews.data(),
        .width = initInfo.vk_framebufferDimensions.width,
        .height = initInfo.vk_framebufferDimensions.height,
        .layers = 1,
    };

    VkDevice vk_device;

    VkFramebuffer vk_framebuffer;
    vkCreateFramebuffer(vk_device, &vk_framebufferCreateInfo, nullptr, &vk_framebuffer);
    return vk_framebuffer;
}

};

VkRenderPass loadPlan(const RenderPlanInitInfo& initInfo)
{
    switch (initInfo.plan)
    {
        case SupportedRenderPlan::eForward:
        {
            const VkRenderPass vk_renderPass = init::forward_createRenderPass(initInfo);
            const std::vector<Core::Attachment> attachments = init::forward_createAttachments(initInfo);
            const VkFramebuffer vk_framebuffer = init::forward_createFramebuffers(initInfo, vk_renderPass, attachments);
        }
    };
}





RenderPlan::RenderPlan(const RenderPlanInitInfo& initInfo)
    : vk_clearValues { initInfo.vk_clearValues }
{
    vk_renderPass = loadPlan(initInfo);

    vk_renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    vk_renderPassBeginInfo.renderPass = vk_renderPass;
    vk_renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(vk_clearValues.size());
    vk_renderPassBeginInfo.pClearValues = vk_clearValues.data();


}


void RenderPlan::execute(const VkCommandBuffer vk_cmdBuff, const VkFramebuffer vk_framebuffer, const VkRect2D vk_renderArea)
{
    vk_renderPassBeginInfo.framebuffer = vk_framebuffer;
    vk_renderPassBeginInfo.renderArea = vk_renderArea;

    const VkSubpassBeginInfo vk_subpassBeginInfo {
        .sType = VK_STRUCTURE_TYPE_SUBPASS_BEGIN_INFO,
        .contents = VK_SUBPASS_CONTENTS_INLINE,
    };

    const VkSubpassEndInfo vk_subpassEndInfo {
        .sType = VK_STRUCTURE_TYPE_SUBPASS_END_INFO
    };

    vkCmdBeginRenderPass2(vk_cmdBuff, &vk_renderPassBeginInfo, &vk_subpassBeginInfo);

    vkCmdNextSubpass2(vk_cmdBuff, &vk_subpassBeginInfo, &vk_subpassEndInfo);

    vkCmdEndRenderPass2(vk_cmdBuff, &vk_subpassEndInfo);
}