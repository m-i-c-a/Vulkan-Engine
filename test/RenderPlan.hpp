#include <vulkan/vulkan.h>

#include <vector>

struct DrawInfo
{

};

struct RenderThread
{

};

struct RenderAttachment
{

};

struct Pass
{
    // color attachment(s)
    // depth attachment
    // stencil attachment

    // pre dependencies
    // post dependencies


    VkSubpassBeginInfo vk_subpassBeginInfo;
    VkSubpassEndInfo   vk_subpassEndInfo;
};

struct RenderPlan
{
    void addPass();
    void addColorAttachment();
    void addDepthAttachment();


    void registerPipeline(const uint32_t pipelineID, const VkPipeline vk_pipeline);

    void addDraw(const DrawInfo& drawInfo);

    void execute(const VkCommandBuffer vk_cmdBuff);


    RenderPlan();
};

/*

Command pools are externally synchronized, meaning that a command pool must not be used 
concurrently in multiple threads. That includes use via recording commands on any command 
buffers allocated from the pool, as well as operations that allocate, free, and reset 
command buffers or the pool itself.



VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT specifies that each recording of the command 
buffer will only be submitted once, and the command buffer will be reset and recorded 
again between each submission.

VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT specifies that a secondary command buffer
is considered to be entirely inside a render pass. If this is a primary command buffer,
then this bit is ignored.



If a command buffer is in the invalid, or executable state, and the command buffer was 
allocated from a command pool with the VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT 
flag set, then vkBeginCommandBuffer implicitly resets the command buffer, behaving as if
vkResetCommandBuffer had been called with VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT
not set. After the implicit reset, commandBuffer is moved to the recording state.

If any command buffer submitted to this queue is in the executable state, it is moved to
the pending state. Once execution of all submissions of a command buffer complete, it
moves from the pending state, back to the executable state. If a command buffer was 
recorded with the VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT flag, it instead moves back
to the invalid state.




VK_SUBPASS_CONTENTS_INLINE specifies that the contents of the subpass will be recorded
inline in the primary command buffer, and secondary command buffers must not be executed
within the subpass.

VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS specifies that the contents are recorded in
secondary command buffers that will be called from the primary command buffer, and
vkCmdExecuteCommands is the only valid command on the command buffer until
vkCmdNextSubpass or vkCmdEndRenderPass.




*/