add_library(vulkanwrapper STATIC 
    Buffer.cpp              Buffer.hpp
    CommandBuffer.cpp       CommandBuffer.hpp
    CommandPool.cpp         CommandPool.hpp
    DescriptorPool.cpp      DescriptorPool.hpp
    DescriptorSetLayout.cpp DescriptorSetLayout.hpp
    DescriptorSet.cpp       DescriptorSet.hpp
    DeviceMemory.cpp        DeviceMemory.hpp
    Fence.cpp               Fence.hpp
    Framebuffer.cpp         Framebuffer.hpp
    Generic.cpp             Generic.hpp
    Image.cpp               Image.hpp
    ImageView.cpp           ImageView.hpp
    Pipeline.cpp            Pipeline.hpp
    PipelineLayout.cpp      PipelineLayout.hpp
    RenderPass.cpp          RenderPass.hpp
    Resource.cpp            Resource.hpp
    Sampler.cpp             Sampler.hpp
    ShaderModule.cpp        ShaderModule.hpp
)

target_compile_options(vulkanwrapper PRIVATE -g -std=c++17)