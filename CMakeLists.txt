cmake_minimum_required(VERSION 3.0)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 17)
project(engine)

add_subdirectory(third-party)

add_executable(${PROJECT_NAME} test.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw $ENV{VULKAN_SDK}/lib/libvulkan.so)
target_include_directories(${PROJECT_NAME} PRIVATE glfw_INCLUDE_DIRS $ENV{VULKAN_SDK}/include)