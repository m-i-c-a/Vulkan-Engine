add_library(core STATIC
    Attachment.cpp                Attachment.hpp
    BufferPool.cpp                BufferPool.hpp
    PersistentDeviceBuffer.cpp    PersistentDeviceBuffer.hpp
    PersistentStagingBuffer.cpp   PersistentStagingBuffer.hpp
    SceneBuffer.cpp               SceneBuffer.hpp
    StagingBuffer.cpp             StagingBuffer.hpp
    Texture.cpp                   Texture.hpp
)

target_link_libraries(core PRIVATE vulkanwrapper  $ENV{VULKAN_SDK}/lib/libvulkan.so)
target_include_directories(core PRIVATE ${CMAKE_HOME_DIRECTORY}/src  $ENV{VULKAN_SDK}/include) 
target_compile_options(core PRIVATE -g -std=c++2a)