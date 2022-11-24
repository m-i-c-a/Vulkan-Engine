#include <array>

#include "RenderPlan.hpp"


namespace init
{

VkRenderPass forward(const RenderPlanInitInfo& initInfo)
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

};

VkRenderPass loadPlan(const RenderPlanInitInfo& initInfo)
{
    switch (initInfo.plan)
    {
        case SupportedRenderPlan::eForward:
        {
            return init::forward(initInfo);
            break;
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