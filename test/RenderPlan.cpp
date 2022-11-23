#include "RenderPlan.hpp"


void addAttachment()
{
    // Can vary from run to run
    VkFormat                        format;
    VkSampleCountFlagBits           samples;


    VkAttachmentDescription2 vk_attachmentDescription {
        VkStructureType                 sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2,
        const void*                     pNext;
        VkAttachmentDescriptionFlags    flags;
        VkFormat                        format;
        VkSampleCountFlagBits           samples;
        VkAttachmentLoadOp              loadOp;
        VkAttachmentStoreOp             storeOp;
        VkAttachmentLoadOp              stencilLoadOp;
        VkAttachmentStoreOp             stencilStoreOp;
        VkImageLayout                   initialLayout;
        VkImageLayout                   finalLayout;
    };
}


RenderPlan::RenderPlan()
{
    // All populated from a call to getRenderPlanInfo(Type)
    // Type = FORWARD , DEFERRED
    std::vector<VkAttachmentDescription2> vk_attachmentDescriptions;
    std::vector<VkSubpassDescription2> vk_subpassDescriptions;
    std::vector<VkSubpassDependency2> vk_subpassDependencies;


    const VkRenderPassCreateInfo2 vk_createInfo {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2,
        // .pNext = ,
        // .flags = ,
        .attachmentCount = static_cast<uint32_t>(vk_attachmentDescriptions.size()),
        .pAttachments = vk_attachmentDescriptions.data(),
        .subpassCount = static_cast<uint32_t>(vk_subpassDescriptions.size()),
        .pSubpasses = vk_subpassDescriptions.data(),
        .dependencyCount = static_cast<uint32_t>(vk_subpassDependencies.size()),
        .pDependencies = vk_subpassDependencies.data(),
        // .correlatedViewMaskCount = ,
        // .pCorrelatedViewMasks = ,
    };

    VkRenderPassCreateInfo a;
}


void RenderPlan::execute(const VkCommandBuffer vk_cmdBuff)
{
    VkRenderPass vk_renderPass;
    VkFramebuffer vk_framebuffer;
    VkRect2D vk_renderArea;

    const VkRenderPassBeginInfo vk_renderPassBeginInfo {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = vk_renderPass,
        .framebuffer = vk_framebuffer,
        .renderArea = vk_renderArea,
        // .clearValueCount = ,
        // .pClearValues = ,
    };

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