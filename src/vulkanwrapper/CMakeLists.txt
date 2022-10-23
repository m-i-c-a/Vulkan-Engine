add_library(vulkanwrapper STATIC 
    Buffer.cpp         Buffer.hpp
    CommandBuffer.cpp  CommandBuffer.hpp
    CommandPool.cpp    CommandPool.hpp
    DescriptorPool.cpp DescriptorPool.hpp
    Fence.cpp          Fence.hpp
    Resource.cpp       Resource.hpp
    ShaderModule.cpp   ShaderModule.hpp
)

target_compile_options(vulkanwrapper PRIVATE -g -std=c++17)