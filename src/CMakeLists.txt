include_directories(common)

add_subdirectory(vulkanwrapper)
add_subdirectory(core)
add_subdirectory(debug-utils)

message(STATUS ${IMGUI_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} main.cpp 
    VulkanCore.cpp    VulkanCore.hpp
    Loader.cpp        Loader.hpp    
    TextureLoader.cpp TextureLoader.hpp    
    ${IMGUI_SOURCES}
)

target_link_libraries(${PROJECT_NAME} PUBLIC 
    glfw 
    glm 
    ktx
    $ENV{VULKAN_SDK}/lib/libvulkan.so 
    vulkanwrapper 
    core debug-utils)

target_include_directories(${PROJECT_NAME} PUBLIC 
    glfw_INCLUDE_DIRS
    glm_INCLUDE_DIRS 
    ktx_INCLUDE_DIRS
    ${IMGUI_INCLUDE_DIRS}
    $ENV{VULKAN_SDK}/include 
    ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_options(${PROJECT_NAME} PRIVATE -g -std=c++17)