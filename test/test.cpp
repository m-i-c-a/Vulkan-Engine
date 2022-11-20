#include <inttypes.h>
#include <vulkan/vulkan.h>
#include "Defines.hpp"


struct A_defintion {

    // Vertex Attributes
    // POSITION
    // NORMAL
    // TEX_COORD


    // Material Inputs
    float color[3];
};


struct B_defintion {

    // Vertex Attributes
    // POSITION
    // NORMAL
    // TEX_COORD


    // Material Inputs
    uint32_t tex_diffuseID;
};

struct C_defintion {

    // Vertex Attributes
    // POSITION
    // TEX_COORD


    // Material Inputs
    uint32_t tex_diffuseID;
    uint32_t tex_normalID ;
};

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

struct PipelineBin
{
    VkPipeline vk_pipeline;

    std::vector<Renderable> renderables;
};

struct MaterialBin
{
    VkPipelineLayout vk_pipelineLayout = VK_NULL_HANDLE;
    VkDescriptorSet  vk_descSet        = VK_NULL_HANDLE;;

    std::vector<PipelineBin> pipelines;
};

struct SortBin
{
    std::vector<MaterialBin> materials;
};

struct Subpass
{
    std::vector<SortBin> sortBins;
};

enum class RendererType
{
    eForward  = 0,
    eDeferred     // 1
};

struct DefSpotLight
{
    void cull();
    void render();
};

#include <set>
#include <unordered_map>

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


struct _Pipeline
{

};

struct PipelineManager
{

    void createPipeline();
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

    void registerMaterial(const uint16_t materialID, const VkPipelineLayout vk_pipelineLayout, const VkDescriptorSet vk_descSet);
    void registerPipeline(const uint16_t materialID, const uint16_t pipelineID, const VkPipeline vk_pipeline);
    void addRenderable(const DrawInfo& drawInfo);

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

void addRenderable(const DrawInfo& drawInfo)
{

    // Determine which frustum maps to which pass

    // Determine Sort Bin

    std::unordered_map<uint16_t, _MatierialInterface> materialInterfaces;

    auto iter = materialInterfaces.find(drawInfo.materialInterfaceID);

    // Should always exist
}


#include <vector>

void recordDraws(std::vector<Material> materials, VkDescriptorSet vk_descSet, VkCommandBuffer vk_cmdBuff)
{
    vkCmdBindDescriptorSets(vk_cmdBuff,
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            materials[0].vk_pipelineLayout,
                            1, 1,
                            &vk_descSet,
                            0, nullptr);

    for (Material mat : materials) // active materials which made it past cull
    {
        // bind mat + draw SSBO

        vkCmdBindDescriptorSets(vk_cmdBuff,
                                VK_PIPELINE_BIND_POINT_GRAPHICS,
                                mat.vk_pipelineLayout,
                                1, 1,
                                &mat.vk_descSet,
                                0, nullptr);

        for (Pipeline pipeline : mat.pipelines) // active pipelines which made it past cull
        {
            // bind pipeline

            vkCmdBindPipeline(vk_cmdBuff, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.vk_pipeline);

            for (Renderable renderable : pipeline.renderables) // active renderables which made it past cull
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
        }
    }
}

int main()
{

#define COMPUTE_CULL

#ifdef COMPUTE_CULL
    std::vector<Material> materials;

    // bind global data

    for (Material mat : materials)
    {
        // if readback from gpu says active continue

        // bind mat + draw SSBO

        for (Pipeline pipeline : mat.pipelines)
        {
            // if readback from gpu says active continu 

            // bind pipeline

            // draw # of renderables (one call to drawIndirect)
        }
    }
#endif








    VkCommandBuffer vk_cmdBuff = VK_NULL_HANDLE;
    VkDescriptorSet vk_globalDescSet = VK_NULL_HANDLE;
    constexpr RendererType rendererType = RendererType::eForward;

    const bool dynamicInstanceBatching = false;



    // First load of material, generic sort bin set in JSON
    Material mat;

    if constexpr (rendererType == RendererType::eForward)
    {
        // set sort bin
        // mat.sortBin = getForwardSortBin(mat.genericSortBin);
    }

    if constexpr (rendererType == RendererType::eDeferred)
    {
        // set sort bin
        // mat.sortBin = getDeferred(mat.genericSortBin);
    }


    // Cull
    if constexpr (rendererType == RendererType::eForward)
    {
        
    }

    if constexpr (rendererType == RendererType::eDeferred)
    {
    }




    std::vector<Material> materials; // could be sorted based on draw priority (replicating sort bin logic)

    if constexpr (rendererType == RendererType::eForward)
    {
        recordDraws(materials, vk_globalDescSet, vk_cmdBuff);
    }

    if constexpr (rendererType == RendererType::eDeferred)
    {
        // gbuffer pass
        recordDraws(materials, vk_globalDescSet, vk_cmdBuff);

        // light pass
        recordDraws(materials, vk_globalDescSet, vk_cmdBuff);

        // transparent pass
        recordDraws(materials, vk_globalDescSet, vk_cmdBuff);
    }


    // bind global data, works under the assumption that all materials have the same
    // global set

}