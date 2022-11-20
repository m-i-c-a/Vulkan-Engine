#include <vector>
#include <array>
#include <inttypes.h>
#include <atomic>
#include <set>

constexpr uint32_t MAX_TILES       =  5000;
constexpr uint32_t MAX_RENDERABLES = 10000;

enum class CullState
{
    eInside     = 0,
    eStraddling     // 1
};

struct TransformData
{
    float position[3];
    float rotation[3];
    float scale[3];

    float minAABB[3];
    float maxAABB[3];
};

struct Frustum
{
    CullState intersect(const TransformData& transformData) const;
};

struct RenderableInfo
{
    uint32_t handle ;
    uint8_t  frustum;
};

struct Renderable
{
    uint32_t matID;
    uint32_t drawID;
};

struct Tile
{
    std::vector<uint32_t>      handles   ; // Unique Renderable ID
    std::vector<TransformData> transforms; // Transform data required for cull
};

struct Grid
{
    std::array<Tile*  , MAX_TILES> tiles;

    uint32_t activeTileCount = 0;
};

struct DrawInfo
{
    uint8_t frustum;

    uint32_t drawID;
    uint32_t matID ;

    uint32_t indexCount;
    uint32_t instanceCount;
    uint32_t firstIndex;
    int32_t  vertexOffset;
    uint32_t firstInstance;
    uint32_t vertexCount;
};

struct Renderer
{
    void addRenderable(const DrawInfo& drawInfo);
};

void cull()
{
    Renderer renderer;
    Grid grid;
    Frustum frustum; 
    const uint32_t activeRenderables = 1000;

    std::vector<uint32_t> visibleRenderables(activeRenderables);
    std::atomic<uint32_t> visibleRenderableCount = 0;

    for (uint32_t i = 0; i < grid.activeTileCount; ++i)
    {
        Tile* tile = grid.tiles[i];

        const CullState result = frustum.intersect(tile->transforms[0]);

        if (result == CullState::eInside)
        {
            visibleRenderables[visibleRenderableCount.fetch_add(1, std::memory_order_relaxed)] = tile->handles[0];

            for (uint32_t handleIdx = 1; handleIdx < tile->handles.size(); ++handleIdx) 
            {
                visibleRenderables[visibleRenderableCount.fetch_add(1, std::memory_order_relaxed)] = tile->handles[handleIdx];
            }
            continue;
        }

        if (result == CullState::eStraddling)
        {

            continue;
        }
    }

    // Offload visible renderables to renderer

    // Grab Material ID
    // Grab Draw ID
    // Grab Vertex/Index Buffer Info

    DrawInfo drawInfo;
    renderer.addRenderable(drawInfo);
}
