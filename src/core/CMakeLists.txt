add_library(core STATIC
    BufferPool.cpp                BufferPool.hpp
    GenericBuffer.cpp             GenericBuffer.hpp
    HostBuffer.cpp                HostBuffer.hpp
    PersistentDeviceBuffer.cpp    PersistentDeviceBuffer.hpp
    PersistentStagingBuffer.cpp   PersistentStagingBuffer.hpp
    SceneBuffer.cpp               SceneBuffer.hpp
    Texture.cpp                   Texture.hpp
)

target_link_libraries(core PRIVATE vulkanwrapper)
target_include_directories(core PRIVATE ${CMAKE_HOME_DIRECTORY}/src)
target_compile_options(core PRIVATE -g -std=c++2a)