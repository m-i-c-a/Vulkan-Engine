#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "Defines.hpp"

#include "VulkanCore.hpp"
#include "RenderPlan.hpp"
#include "Loader.hpp"

#include "vulkanwrapper/CommandBuffer.hpp"
#include "vulkanwrapper/CommandPool.hpp"
#include "vulkanwrapper/DescriptorPool.hpp"
#include "vulkanwrapper/DescriptorSet.hpp"
#include "vulkanwrapper/DescriptorSetLayout.hpp"
#include "vulkanwrapper/Fence.hpp"
#include "vulkanwrapper/PipelineLayout.hpp"
#include "vulkanwrapper/Pipeline.hpp"
#include "vulkanwrapper/ShaderModule.hpp"

#include "core/BufferPool.hpp"
#include "core/PersistentDeviceBuffer.hpp"
#include "core/PersistentStagingBuffer.hpp"
#include "core/SceneBuffer.hpp"
#include "core/TextureList.hpp"

#include "debug-utils/DebugUtilsEXT.hpp"

struct ModelInfo 
{
    uint32_t m_uMeshID     = 0;
    uint32_t m_uIndexCount = 0;
    uint32_t m_uFirstIndex = 0;
    int32_t  m_iVertexOffset = 0;
};

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

VulkanWrapper::DescriptorPool createDescriptorPool();
VulkanWrapper::DescriptorSetLayout createDescriptorSetLayout();
VulkanWrapper::DescriptorSet allocateDescriptorSet(const VkDescriptorPool vk_pool, const VkDescriptorSetLayout vk_layout, void* pNext);
VulkanWrapper::PipelineLayout createPipelineLayout(const VkDescriptorSetLayout vk_descSetLayout);
VulkanWrapper::Pipeline createPipeline(const VkPipelineLayout vk_pipelineLayout, const VkExtent2D vk_viewportExtent, const VkRenderPass vk_renderPass);

RenderPlan::DrawInfo loadModel(Core::SceneBuffer& sceneBuffer, Core::BufferPool<MatData>& matBufferPool,  Core::BufferPool<DrawData>& drawBufferPool, Core::TextureList& textureList, const std::string& filename);
void blit(const VkCommandBuffer vk_cmdBuff, const VkImage vk_srcImage, const VkImage vk_dstImage, const VkOffset3D vk_dimensions);
void updateBufferDescriptorSet(const VkDevice vk_device, const VkDescriptorSet vk_descSet, const uint32_t descBinding, const VkDescriptorType vk_descType, VkDescriptorBufferInfo&& vk_descBufferInfo);

void submit();

constexpr uint32_t WINDOW_WIDTH         = 600;
constexpr uint32_t WINDOW_HEIGHT        = 600;

constexpr uint32_t MAX_RENDERABLE_COUNT = 1;
constexpr uint32_t MAX_MATERIAL_COUNT   = 1;
constexpr uint32_t MAX_PIPELINE_COUNT   = 1;
constexpr uint32_t MAX_TEXTURES         = 1;

constexpr float NEAR_Z = 0.1f;
constexpr float FAR_Z = 100.0f;

static float FOV = 45.0f;
static glm::vec3 camPos { 0.0f, 0.0f, -5.0f };

constexpr VkDescriptorSetVariableDescriptorCountAllocateInfo vk_descriptorSetVariableDescriptorCountAllocInfo {
    .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO_EXT,
    .descriptorSetCount = 1,
    .pDescriptorCounts = &MAX_TEXTURES,
};

void kickoff(GLFWwindow* glfw_window, const VulkanCore& vulkanCore)
{
    uint32_t swapchainImageIdx = 0;

    const RenderPlan::InitInfo rendererInitInfo {
        .plan = RenderPlanType::eForward,
        .vk_sampleCount = VK_SAMPLE_COUNT_1_BIT,
        .vk_attachmentFormats = { 
            VK_FORMAT_B8G8R8A8_UNORM, // DEFUALT COLOR
            VK_FORMAT_D32_SFLOAT      // DEPTH
        },
        .vk_clearValues = { 
            { .color = { 0.1, 0.1, 0.1, 0.0 } },
            { .depthStencil = { 1.0, 0u }     }
        },
        .vk_framebufferDimensions = vulkanCore.vk_swapchainExtent
    };

    RenderPlan renderer(rendererInitInfo);

    // Create Global Vulkan Resources
    VulkanWrapper::Fence          fence(0x0);
    VulkanWrapper::CommandPool    cmdPool(0x0, 0);
    VulkanWrapper::CommandBuffer  cmdBuff(cmdPool.vk_handle);
    VulkanWrapper::DescriptorPool descPool = createDescriptorPool();
    VulkanWrapper::DescriptorSetLayout descSetLayout = createDescriptorSetLayout();
    VulkanWrapper::DescriptorSet descSet = allocateDescriptorSet(descPool.vk_handle, descSetLayout.vk_handle, (void*)&vk_descriptorSetVariableDescriptorCountAllocInfo);

    PersistentStagingBuffer persistent_stagingBuffer;
    PersistentDeviceBuffer* globalUBO = persistent_stagingBuffer.registerDeviceBuffer(sizeof(GlobalUBO), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    persistent_stagingBuffer.completeRegistration();

    Core::TextureList textureList(MAX_TEXTURES);

    // // Create / Define Resources for Uber Material State
    Core::BufferPool<MatData> matBufferPool(MAX_MATERIAL_COUNT, 
                                            MAX_MATERIAL_COUNT, 
                                            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                            VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT,
                                            VK_ACCESS_2_MEMORY_READ_BIT);

    // Create / Define Resources for Uber Draw State
    Core::BufferPool<DrawData> drawBufferPool(MAX_RENDERABLE_COUNT,
                                              MAX_RENDERABLE_COUNT,
                                              VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                              VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT,
                                              VK_ACCESS_2_MEMORY_READ_BIT);

    // Update Descriptor Sets
    updateBufferDescriptorSet(vulkanCore.vk_device, descSet.vk_handle, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, { globalUBO->getBuffer(), 0, VK_WHOLE_SIZE });
    updateBufferDescriptorSet(vulkanCore.vk_device, descSet.vk_handle, 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, drawBufferPool.getDescBufferInfo());
    updateBufferDescriptorSet(vulkanCore.vk_device, descSet.vk_handle, 2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, matBufferPool.getDescBufferInfo());

    // Create Global Vertex / Index Buffer
    Core::SceneBuffer sceneBuffer(sizeof(Vertex), 5000000, 2000000, 5000000);

    // Create Pipeline
    uint32_t pipelineID = 0;
    VulkanWrapper::PipelineLayout pipelineLayout = createPipelineLayout(descSetLayout.vk_handle);
    VulkanWrapper::Pipeline pipeline = createPipeline(pipelineLayout.vk_handle, vulkanCore.vk_swapchainExtent, renderer.getRenderPass());

    // Register created Pipeline with renderer
    renderer.registerPipeline(pipelineID, pipeline.vk_handle);

    // Load Model 
    const RenderPlan::DrawInfo drawInfo = loadModel(sceneBuffer, matBufferPool, drawBufferPool, textureList, "../obj-files/torus.obj");

    // Init globalUBO
    glm::mat4 projMat = glm::perspective(FOV, ((float)WINDOW_WIDTH) / (float)WINDOW_HEIGHT, NEAR_Z, FAR_Z);
    glm::mat4 viewMat = glm::mat4(1.0f);
    viewMat = glm::translate(viewMat, camPos);
    glm::mat4 projViewMat = projMat * viewMat;
    globalUBO->update(offsetof(GlobalUBO, projViewMatrix), sizeof(glm::mat4), &(projViewMat[0][0]));

    cmdBuff.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    sceneBuffer.flushQueuedUploads(cmdBuff.vk_handle, true);
    drawBufferPool.flushDirtyBlocks(cmdBuff.vk_handle, true);
    globalUBO->flush(cmdBuff.vk_handle);

    cmdBuff.end();

    const VkSubmitInfo vk_submitInfo {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = &cmdBuff.vk_handle,
    };

    VK_CHECK(vkQueueSubmit(vulkanCore.vk_graphicsQ, 1, &vk_submitInfo, VK_NULL_HANDLE));
    VK_CHECK(vkQueueWaitIdle(vulkanCore.vk_graphicsQ));

    while (!glfwWindowShouldClose(glfw_window))
    {
        glfwPollEvents();

        VK_CHECK(vkAcquireNextImageKHR(vulkanCore.vk_device, vulkanCore.vk_swapchain, UINT64_MAX, VK_NULL_HANDLE, fence.vk_handle, &swapchainImageIdx));
        fence.wait();
        fence.reset();

        renderer.addDraw(drawInfo);

        cmdPool.reset(0x0);

        cmdBuff.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

        const VkBuffer vk_vertexBuffer = sceneBuffer.getVertexBuffer();
        const VkDeviceSize offsets[1] = { 0 };
        vkCmdBindVertexBuffers(cmdBuff.vk_handle, 0, 1, &vk_vertexBuffer, offsets);
        vkCmdBindIndexBuffer(cmdBuff.vk_handle, sceneBuffer.getIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);

        vkCmdBindDescriptorSets(cmdBuff.vk_handle, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout.vk_handle, 0, 1, &descSet.vk_handle, 0, nullptr);

        renderer.execute(cmdBuff.vk_handle, { .offset = {0, 0}, .extent = vulkanCore.vk_swapchainExtent });

        cmdBuff.end();

        const VkSubmitInfo vk_submitInfo {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .waitSemaphoreCount = 0,
            .pWaitSemaphores = nullptr,
            .pWaitDstStageMask = nullptr,
            .commandBufferCount = 1,
            .pCommandBuffers = &cmdBuff.vk_handle,
            .signalSemaphoreCount = 0,
            .pSignalSemaphores = 0,
        };

        VK_CHECK(vkQueueSubmit(vulkanCore.vk_graphicsQ, 1, &vk_submitInfo, VK_NULL_HANDLE));
        VK_CHECK(vkQueueWaitIdle(vulkanCore.vk_graphicsQ));

        cmdPool.reset(0x0);

        cmdBuff.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

        blit(cmdBuff.vk_handle, 
             renderer.getAttachmentImage(0),
             vulkanCore.vk_swapchainImages[swapchainImageIdx], 
             { (int32_t)vulkanCore.vk_swapchainExtent.width, (int32_t)vulkanCore.vk_swapchainExtent.height, 1 });

        cmdBuff.end();

        VK_CHECK(vkQueueSubmit(vulkanCore.vk_graphicsQ, 1, &vk_submitInfo, VK_NULL_HANDLE));
        VK_CHECK(vkQueueWaitIdle(vulkanCore.vk_graphicsQ));

        //*** Present (wait for graphics work to complete)
        const VkPresentInfoKHR vk_presentInfo {
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .swapchainCount = 1,
            .pSwapchains = &vulkanCore.vk_swapchain,
            .pImageIndices = &swapchainImageIdx,
        };

        VK_CHECK(vkQueuePresentKHR(vulkanCore.vk_graphicsQ, &vk_presentInfo));
        VK_CHECK(vkDeviceWaitIdle(vulkanCore.vk_device));
    }

    VK_CHECK(vkDeviceWaitIdle(vulkanCore.vk_device));

    delete globalUBO;
}


int main()
{
    const char* windowName = "Engine";

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    GLFWwindow* glfw_window = glfwCreateWindow(static_cast<int>(WINDOW_WIDTH), static_cast<int>(WINDOW_HEIGHT), windowName, nullptr, nullptr);
    assert(glfw_window && "Failed to create window");
    glfwSetKeyCallback(glfw_window, keyCallback);

    const VkPhysicalDeviceVulkan12Features vk_physicalDeviceFeatures12 {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
        .drawIndirectCount = VK_TRUE,
        .shaderSampledImageArrayNonUniformIndexing = VK_TRUE,
        .descriptorBindingPartiallyBound = VK_TRUE,
        .descriptorBindingVariableDescriptorCount = VK_TRUE,
        .runtimeDescriptorArray = VK_TRUE,
    };

    const VkPhysicalDeviceVulkan13Features vk_physicalDeviceFeatures13 {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
        .pNext = (void*)&vk_physicalDeviceFeatures12,
        .synchronization2 = VK_TRUE,
        // .dynamicRendering = VK_TRUE
    };

    const VulkanInitInfo vulkanInitInfo {
        .window = glfw_window,
#ifdef DEBUG
        .requestedInstanceLayerNames = { "VK_LAYER_KHRONOS_validation" },
#endif
        .requestedInstanceExtensionNames = { "VK_KHR_surface", "VK_KHR_xcb_surface", VK_EXT_DEBUG_UTILS_EXTENSION_NAME },
        // .requestedDeviceExtensionNames = { VK_KHR_SWAPCHAIN_EXTENSION_NAME, "VK_KHR_dynamic_rendering", VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME },
        .requestedDeviceExtensionNames = { VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME },
        .requestedSwapchainImageCount = 3,
        .requestedSwapchainImageFormat = VK_FORMAT_R8G8B8_SRGB,
        .requestedSwapchainImageExtent = { WINDOW_WIDTH, WINDOW_HEIGHT },
        .requestedSwapchainImagePresentMode = VK_PRESENT_MODE_FIFO_KHR,
        .pDeviceFeatures = (void*)(&vk_physicalDeviceFeatures13)
    };

    VulkanCore vulkanCore = initVulkan(vulkanInitInfo);

    VulkanWrapper::Resource::initResources(vulkanCore.vk_physicalDevice, vulkanCore.vk_device);
#ifdef DEBUG
    DebugUtilsEXT::init(vulkanCore.vk_instance, vulkanCore.vk_device);
#endif

    kickoff(glfw_window, vulkanCore);

    cleanupVulkan(vulkanCore);

    glfwDestroyWindow(glfw_window);
    glfwTerminate();
}



void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    switch (key)
    {
        case GLFW_KEY_ESCAPE:
            // if (action == GLFW_PRESS)
                // renderGui = !renderGui;
            break;
        default:
            break;
    };
}

VulkanWrapper::DescriptorPool createDescriptorPool()
{
    // Allows for descriptors allocated from this pool to have bindings with the 
    // VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT flag set.
    const VkDescriptorPoolCreateFlags vk_flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;

    const VkDescriptorPoolSize vk_poolSizes[2] {
        {
            .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1
        },
        {
            .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .descriptorCount = 2
        }
    };

    const VkDescriptorPoolCreateInfo vk_createInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .flags = vk_flags,
        .maxSets = 1,
        .poolSizeCount = 2,
        .pPoolSizes = vk_poolSizes,
    };

    VulkanWrapper::DescriptorPool descPool(vk_createInfo);
    return descPool;
}

VulkanWrapper::DescriptorSetLayout createDescriptorSetLayout()
{
    const uint32_t bindingCount = 4;

    const VkDescriptorBindingFlags vk_bindlessFlags {
        // The size of the binding will be determined upon descriptor set allocation. 
        VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT | 

        // If all shader innvocations do not accesses memory via descriptor X, then descriptor
        // X does not need to contain a valid descriptor at the time of consumption. This is 
        // true for all descriptors within said binding. 
        VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT
    };

    const VkDescriptorBindingFlags vk_descriptorBindingFlags[bindingCount] {
        0x0,
        0x0,
        0x0,
        vk_bindlessFlags,
    };

    const VkDescriptorSetLayoutBindingFlagsCreateInfo vk_descriptorSetLayoutBindingFlagsCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO,
        .bindingCount = bindingCount,
        .pBindingFlags = vk_descriptorBindingFlags,
    };

    const VkDescriptorSetLayoutBinding vk_bindings[] {
        {
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_ALL,
            .pImmutableSamplers = nullptr,
        },
        {
            .binding = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_ALL,
            .pImmutableSamplers = nullptr,
        },
        {
            .binding = 2,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_ALL,
            .pImmutableSamplers = nullptr,
        },
        {
            .binding = 3,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_ALL,
            .pImmutableSamplers = nullptr,
        },
    };

    const VkDescriptorSetLayoutCreateInfo vk_createInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = &vk_descriptorSetLayoutBindingFlagsCreateInfo,
        .bindingCount = bindingCount,
        .pBindings = vk_bindings,
    };

    VulkanWrapper::DescriptorSetLayout descSetLayout(vk_createInfo);
    return descSetLayout;
}

VulkanWrapper::DescriptorSet allocateDescriptorSet(const VkDescriptorPool vk_pool, const VkDescriptorSetLayout vk_layout, void* pNext)
{
    const VkDescriptorSetAllocateInfo vk_allocInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext = pNext,
        .descriptorPool = vk_pool,
        .descriptorSetCount = 1,
        .pSetLayouts = &vk_layout,
    };

    VulkanWrapper::DescriptorSet descSet(vk_allocInfo);
    return descSet;

    VkDescriptorSetLayout vk_globalDescSetLayout;
}

VulkanWrapper::PipelineLayout createPipelineLayout(const VkDescriptorSetLayout vk_descSetLayout)
{
    const VkPipelineLayoutCreateInfo vk_createInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 1,
        .pSetLayouts = &vk_descSetLayout,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = nullptr,
    };

    VulkanWrapper::PipelineLayout pipelineLayout(vk_createInfo);
    return pipelineLayout;
}

VulkanWrapper::Pipeline createPipeline(const VkPipelineLayout vk_pipelineLayout, const VkExtent2D vk_viewportExtent, const VkRenderPass vk_renderPass)
{
    VulkanWrapper::ShaderModule vertexShaderModule = VulkanWrapper::ShaderModule();
    vertexShaderModule.create("../shaders/spirv/shader-vert.spv");

    VulkanWrapper::ShaderModule fragmentShaderModule = VulkanWrapper::ShaderModule();
    fragmentShaderModule.create("../shaders/spirv/shader-frag.spv");

    const VkPipelineShaderStageCreateInfo vk_shaderStageCreateInfos[] = {
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_VERTEX_BIT,
            .module = vertexShaderModule.m_vkShaderModule,
            .pName = "main",
            .pSpecializationInfo = nullptr
        },
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            .module = fragmentShaderModule.m_vkShaderModule,
            .pName = "main",
            .pSpecializationInfo = nullptr
        }
    };

    const VkVertexInputBindingDescription vk_vertexInputBindingDesc {
        .binding = 0,
        .stride = sizeof(Vertex),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
    };

    const VkVertexInputAttributeDescription vk_vertexInputAttribDescs[] {
        {
            .location = 0,
            .binding = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = 0,
        },
        {
            .location = 1,
            .binding = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = sizeof(float) * 3,
        },
        {
            .location = 2,
            .binding = 0,
            .format = VK_FORMAT_R32G32_SFLOAT,
            .offset = sizeof(float) * 6,
        },
    };

    const VkPipelineVertexInputStateCreateInfo vk_vertexInputStateCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = 1,
        .pVertexBindingDescriptions = &vk_vertexInputBindingDesc,
        .vertexAttributeDescriptionCount = ARRAYSIZE(vk_vertexInputAttribDescs),
        .pVertexAttributeDescriptions = vk_vertexInputAttribDescs,
    };

    const VkPipelineInputAssemblyStateCreateInfo vk_inputAssemblyStateCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE,
    };

    const VkPipelineTessellationStateCreateInfo vk_tessellationStateCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,
        .patchControlPoints = 0,
    };

    const VkViewport vk_viewport {
        .x = 0,
        .y = 0,
        .width = static_cast<float>(vk_viewportExtent.width),
        .height = static_cast<float>(vk_viewportExtent.height),
        .minDepth = 0,
        .maxDepth = 1,
    };

    const VkRect2D vk_scissor {
        .offset = {.x = 0, .y = 0},
        .extent = vk_viewportExtent,
    };

    const VkPipelineViewportStateCreateInfo vk_viewportStateCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .pViewports = &vk_viewport,
        .scissorCount = 1,
        .pScissors = &vk_scissor,
    };

    const VkPipelineRasterizationStateCreateInfo vk_rasterizationStateCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .cullMode = VK_CULL_MODE_NONE,
        .frontFace = VK_FRONT_FACE_CLOCKWISE,
        .depthBiasEnable = VK_FALSE,
        .depthBiasConstantFactor = 0.f,
        .depthBiasClamp = 0.f,
        .depthBiasSlopeFactor = 0.f,
        .lineWidth = 1.f,
    };

    const VkPipelineMultisampleStateCreateInfo vk_multisampleStateCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .sampleShadingEnable = VK_FALSE,
        .minSampleShading = 0,
        .pSampleMask = nullptr,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable = VK_FALSE,
    };

    const VkPipelineDepthStencilStateCreateInfo vk_depthStencilStateCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .depthTestEnable = VK_TRUE,
        .depthWriteEnable = VK_TRUE,
        .depthCompareOp = VK_COMPARE_OP_LESS,
        .depthBoundsTestEnable = VK_FALSE,
        .stencilTestEnable = VK_FALSE,
    };

    const VkPipelineColorBlendAttachmentState vk_colorBlendAttachmentState {
        .blendEnable = VK_FALSE,
        .srcColorBlendFactor = VK_BLEND_FACTOR_ZERO,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
        .colorBlendOp = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        .alphaBlendOp = VK_BLEND_OP_ADD,
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    };
    
    const VkPipelineColorBlendStateCreateInfo vk_colorBlendStateCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY,
        .attachmentCount = 1,
        .pAttachments = &vk_colorBlendAttachmentState,
        .blendConstants = { 0, 0, 0, 0 }
    };

    VkGraphicsPipelineCreateInfo vk_graphicsPipelineCreateInfo {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext = nullptr,
        .stageCount = ARRAYSIZE(vk_shaderStageCreateInfos),
        .pStages = vk_shaderStageCreateInfos,
        .pVertexInputState = &vk_vertexInputStateCreateInfo,
        .pInputAssemblyState = &vk_inputAssemblyStateCreateInfo,
        .pTessellationState = &vk_tessellationStateCreateInfo,
        .pViewportState = &vk_viewportStateCreateInfo,
        .pRasterizationState = &vk_rasterizationStateCreateInfo,
        .pMultisampleState = &vk_multisampleStateCreateInfo,
        .pDepthStencilState = &vk_depthStencilStateCreateInfo,
        .pColorBlendState = &vk_colorBlendStateCreateInfo,
        .pDynamicState = nullptr,
        .layout = vk_pipelineLayout,
        .renderPass = vk_renderPass,
        .subpass = 0,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = 0,
    };

    VulkanWrapper::Pipeline pipeline(vk_graphicsPipelineCreateInfo);
    return pipeline;
}

void blit(const VkCommandBuffer vk_cmdBuff, const VkImage vk_srcImage, const VkImage vk_dstImage, const VkOffset3D vk_dimensions)
{
    const VkImageMemoryBarrier2 vk_preBlitBarrier {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
        .srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        .srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
        .dstAccessMask = VK_ACCESS_2_TRANSFER_READ_BIT | 
                         VK_ACCESS_2_TRANSFER_WRITE_BIT,
        .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = vk_dstImage,
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
        },
    };

    const VkImageMemoryBarrier2 vk_postBlitBarrier {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
        .srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
        .srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_2_NONE,
        .dstAccessMask = VK_ACCESS_2_NONE,
        .oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = vk_dstImage,
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
        },
    };

    const VkDependencyInfo vk_preBlitDependencyInfo {
        .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        .imageMemoryBarrierCount = 1,
        .pImageMemoryBarriers = &vk_preBlitBarrier,
    };

    const VkDependencyInfo vk_postBlitDependencyInfo {
        .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        .imageMemoryBarrierCount = 1,
        .pImageMemoryBarriers = &vk_postBlitBarrier,
    };

    const VkImageSubresourceLayers vk_blitImageSubresource {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .mipLevel = 0,
        .baseArrayLayer = 0,
        .layerCount = 1,
    };

    const VkImageBlit2 vk_imageBlit {
        .sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2,
        .srcSubresource = vk_blitImageSubresource,
        .srcOffsets = {
            { 0, 0, 0 },
            vk_dimensions,
        },
        .dstSubresource = vk_blitImageSubresource,
        .dstOffsets = {
            { 0, 0, 0 },
            vk_dimensions,
        },
    };

    const VkBlitImageInfo2 vk_blitImageInfo {
        .sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2,
        .pNext = nullptr,
        .srcImage = vk_srcImage,
        .srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        .dstImage = vk_dstImage,
        .dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        .regionCount = 1,
        .pRegions = &vk_imageBlit,
        .filter = VK_FILTER_NEAREST,
    };

    vkCmdPipelineBarrier2(vk_cmdBuff, &vk_preBlitDependencyInfo);

    vkCmdBlitImage2(vk_cmdBuff, &vk_blitImageInfo);

    vkCmdPipelineBarrier2(vk_cmdBuff, &vk_postBlitDependencyInfo);
}

void updateBufferDescriptorSet(const VkDevice vk_device, const VkDescriptorSet vk_descSet, const uint32_t descBinding, const VkDescriptorType vk_descType, VkDescriptorBufferInfo&& vk_descBufferInfo)
{
    const VkWriteDescriptorSet vk_writeDescriptorSet {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = vk_descSet,
        .dstBinding = descBinding,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = vk_descType,
        .pImageInfo = nullptr,
        .pBufferInfo = &vk_descBufferInfo,
        .pTexelBufferView = nullptr,
    };

    vkUpdateDescriptorSets(vk_device, 1, &vk_writeDescriptorSet, 0, nullptr);
}

RenderPlan::DrawInfo loadModel(Core::SceneBuffer& sceneBuffer, Core::BufferPool<MatData>& matBufferPool,  Core::BufferPool<DrawData>& drawBufferPool, Core::TextureList& textureList, const std::string& filename)
{
    static uint32_t meshID = 0;

    ObjectBufferData vertexIndexData = loadObjFile(filename);
    const MeshInfo meshInfo = sceneBuffer.queueUpload(meshID, sizeof(Vertex) * vertexIndexData.vertices.size(), (void*)vertexIndexData.vertices.data(), sizeof(uint32_t) * vertexIndexData.indices.size(), vertexIndexData.indices.data());

    // Load all Texture indices
    const uint32_t albedoID = 0;

    // textureList.acquire();

    const uint32_t matID = matBufferPool.acquireBlock();
    {
        MatData& matData = matBufferPool.getWritableBlock(matID);
        matData.albedoID = albedoID;
    }

    const uint32_t drawID = drawBufferPool.acquireBlock();
    {
        DrawData& drawData = drawBufferPool.getWritableBlock(drawID);
        drawData.modelMatrix = glm::mat4(1.0f);
        drawData.modelMatrix = glm::translate(drawData.modelMatrix, glm::vec3(0.0, 0.0, 0.0));
        drawData.modelMatrix = glm::rotate(drawData.modelMatrix, glm::half_pi<float>() / 2.0f, glm::vec3(1.0, 0.0, 0.0)); 
        // drawData.modelMatrix = glm::scale(drawData.modelMatrix, glm::vec3(0.1, 0.1, 0.1));

        drawData.matID = matID;
    }


    const RenderPlan::DrawInfo drawInfo {
        .drawID = drawID,
        .matID  = matID,
        .pipelineID = 0,
        .indexCount = meshInfo.m_uIndexCount,
        .instanceCount = 1,
        .firstIndex = meshInfo.m_uFirstIndex,
        .vertexOffset = meshInfo.m_iVertexOffset,
        .vertexCount = 0,
    };

    return drawInfo;
}