#include <vector>
#include <unordered_map>

#include <vulkan/vulkan.h>

#include "Defines.hpp"

struct Renderable
{
    uint32_t indexCount;
    uint32_t instanceCount;
    uint32_t firstIndex;
    int32_t  vertexOffset;
    uint32_t firstInstance;
    uint32_t vertexCount;
    uint32_t drawID;      // index into structure containing draw + mat ID
    bool indexedDraw;
};

struct DrawInfo
{
    uint8_t frustum;

    uint32_t drawID;
    uint32_t matID ;

    uint16_t pipelineID;

    uint32_t indexCount;
    uint32_t instanceCount;
    uint32_t firstIndex;
    int32_t  vertexOffset;
    uint32_t firstInstance;
    uint32_t vertexCount;
};

struct RenderBin_Pipeline
{
    uint16_t   pipelineID  = 0;
    VkPipeline vk_pipeline = VK_NULL_HANDLE;
    std::vector<Renderable> renderables;
};

struct RenderBin_Material
{
    uint16_t         materialID        = 0;
    VkPipelineLayout vk_pipelineLayout = VK_NULL_HANDLE;
    VkDescriptorSet  vk_descSet        = VK_NULL_HANDLE;

    std::unordered_map<uint16_t, RenderBin_Pipeline*> pipelineBins;
};

struct Renderer
{
    std::unordered_map<uint16_t, RenderBin_Pipeline> pipelineBins;
    std::unordered_map<uint16_t, RenderBin_Material> materialBins;
    VkDescriptorSet vk_globalDescSet = VK_NULL_HANDLE;

    void registerMaterial(const uint16_t materialID, const VkPipelineLayout vk_pipelineLayout, const VkDescriptorSet vk_descSet);
    void registerPipeline(const uint16_t materialID, const uint16_t pipelineID, const VkPipeline vk_pipeline);
    void addRenderable(const DrawInfo& drawInfo);

    void regisiterGlobalDescSet(const VkDescriptorSet vk_descSet);

    void render();

    Renderer();
};

void Renderer::registerMaterial(const uint16_t materialID, const VkPipelineLayout vk_pipelineLayout, const VkDescriptorSet vk_descSet)
{
    if (materialBins.contains(materialID))
    {
        EXIT("Attemting to add the same material twice to the renderer.\n");
    }

    const RenderBin_Material bin {
        .materialID        = materialID,
        .vk_pipelineLayout = vk_pipelineLayout,
        .vk_descSet        = vk_descSet,
    };

    materialBins.insert({ materialID, std::move(bin) });
}

void Renderer::registerPipeline(const uint16_t materialID, const uint16_t pipelineID, const VkPipeline vk_pipeline)
{
    if (pipelineBins.contains(pipelineID))
    {
        EXIT("Attemting to add the same material twice to the renderer.\n");
    }

    const RenderBin_Pipeline bin {
        .pipelineID  = pipelineID,
        .vk_pipeline = vk_pipeline,
    };

    auto pipeIter = pipelineBins.insert({ materialID, std::move(bin) });
    
    auto matIter = materialBins.find(materialID);
    if (matIter == materialBins.end())
    {
        EXIT("Attempting to add pipeline to renderer when associated material does not exist!\n");
    }

    matIter->second.pipelineBins.insert({ materialID, &pipeIter.first->second });
}

void Renderer::addRenderable(const DrawInfo& drawInfo)
{
    auto iter = pipelineBins.find(drawInfo.pipelineID);
    if (iter == pipelineBins.end())
    {
        EXIT("Attempting to add renderable to pipeline that doesn't exist!\n");
    }

    const Renderable renderable {
        .indexCount = drawInfo.indexCount,
        .instanceCount = drawInfo.instanceCount,
        .firstIndex = drawInfo.firstIndex,
        .vertexOffset = drawInfo.vertexOffset,
        .firstInstance = drawInfo.firstInstance,
        .vertexCount = drawInfo.vertexCount,
        .drawID = drawInfo.drawID,      // index into structure containing draw + mat ID
    };

    iter->second.renderables.push_back(std::move(renderable));
}

void Renderer::regisiterGlobalDescSet(const VkDescriptorSet vk_descSet)
{
    vk_globalDescSet = vk_descSet;
}

void Renderer::render()
{
    const VkCommandBuffer vk_cmdBuff = VK_NULL_HANDLE;

    auto matBinIter = materialBins.begin();

    // Bind Global UBO + Global Array of Textures + Draw SSBO

    vkCmdBindDescriptorSets(vk_cmdBuff,
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            matBinIter->second.vk_pipelineLayout,
                            1, 1,
                            &vk_globalDescSet,
                            0, nullptr);

    for (const auto [matID, matBin] : materialBins)
    {
        bool firstPipelineWithDraws = true;

        for (const auto [pipelineID, pipelineBin] : matBin.pipelineBins) // active pipelines which made it past cull
        {
            if (pipelineBin->renderables.empty())
            {
                continue;
            }

            if (firstPipelineWithDraws)
            {
                // bind mat SSBO

                vkCmdBindDescriptorSets(vk_cmdBuff,
                                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                                        matBin.vk_pipelineLayout,         // we can have pipeline layout in pipeline
                                        1, 1,
                                        &matBin.vk_descSet,
                                        0, nullptr);

                firstPipelineWithDraws = false;
            }

            // bind pipeline

            vkCmdBindPipeline(vk_cmdBuff, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineBin->vk_pipeline);

            for (const Renderable& renderable : pipelineBin->renderables) // active renderables which made it past cull
            {
                // draw

                if (renderable.indexedDraw)
                {
                    vkCmdDrawIndexed(vk_cmdBuff, 
                                    renderable.indexCount,
                                    renderable.instanceCount,  // 1 for all non-instanced models
                                    renderable.firstIndex,
                                    renderable.vertexOffset,
                                    renderable.drawID); // index into structure containing draw + mat ID
                }
                else
                {
                    vkCmdDraw(vk_cmdBuff,
                            renderable.vertexCount,
                            renderable.instanceCount,  // 1 for all non-instanced models
                            renderable.vertexOffset,
                            renderable.drawID); // index into structure containing draw + mat ID
                }
            }

            pipelineBin->renderables.clear();
        }
    }
}


struct RenderableInfo
{
    uint32_t matID;
    uint32_t drawID;
};


struct Material
{
    // uint32_t materialID <- from file

    // uint32_t renderableID = renderableInfoBufferPool->acquireBlock();

    // uint32_t drawID = drawBufferPool->acquireBlock()

    // Material* mat = materials.find(materialID);
    // uint32_t matID  = mat->bufferPool->acquireBlock()

    // RenderableInfo& renInfo = renderableInfoBufferPool->getWritableBlock()
    // renInfo.matID = matID;
    // renInfo.drawID = drawID;

    // set mat data

    // set draw data


    // BufferPool<MaterialUBO> matUBO;

    // renderableInfo
};