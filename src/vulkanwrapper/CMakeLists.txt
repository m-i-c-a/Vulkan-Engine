add_library(vulkanwrapper STATIC 
    Buffer.cpp         Buffer.hpp
    CommandBuffer.cpp  CommandBuffer.hpp
    CommandPool.cpp    CommandPool.hpp
    DescriptorPool.cpp DescriptorPool.hpp
    DeviceMemory.cpp   DeviceMemory.hpp
    Fence.cpp          Fence.hpp
    Generic.cpp        Generic.hpp
    Image.cpp          Image.hpp
    ImageView.cpp      ImageView.hpp
    Resource.cpp       Resource.hpp
    Sampler.cpp        Sampler.hpp
    ShaderModule.cpp   ShaderModule.hpp
)

target_compile_options(vulkanwrapper PRIVATE -g -std=c++17)