
namespace Core
{

class StagingBuffer;
class Texture;

}; // Core

namespace VulkanWrapper
{

class Sampler;

}; // Sampler

Core::Texture* loadTexture(const char* filename, Core::StagingBuffer* stagingBuffer, VulkanWrapper::Sampler* sampler);