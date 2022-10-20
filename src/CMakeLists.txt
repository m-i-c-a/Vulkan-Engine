include_directories(common)

add_subdirectory(vulkanwrapper)
add_subdirectory(core)

add_executable(${PROJECT_NAME} main.cpp VulkanCore.cpp VulkanCore.hpp)

target_link_libraries(${PROJECT_NAME} PRIVATE glfw glm $ENV{VULKAN_SDK}/lib/libvulkan.so vulkanwrapper core)
target_include_directories(${PROJECT_NAME} PRIVATE glfw_INCLUDE_DIRS glm_INCLUDE_DIRS $ENV{VULKAN_SDK}/include ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_options(${PROJECT_NAME} PRIVATE -g -std=c++2a)