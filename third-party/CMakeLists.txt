set(GLFW_BUILD_EXAMPLES 1)
add_subdirectory(glfw)
add_subdirectory(glm)

set(IMGUI_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/third-party ${PROJECT_SOURCE_DIR}/third-party/imgui PARENT_SCOPE)

set(IMGUI_CORE_FILES
    ${PROJECT_SOURCE_DIR}/third-party/imgui/imgui_demo.cpp
    ${PROJECT_SOURCE_DIR}/third-party/imgui/imgui_draw.cpp
    ${PROJECT_SOURCE_DIR}/third-party/imgui/imgui_tables.cpp
    ${PROJECT_SOURCE_DIR}/third-party/imgui/imgui_widgets.cpp
    ${PROJECT_SOURCE_DIR}/third-party/imgui/imgui.cpp)

set(IMGUI_BACKEND_FILES
    ${PROJECT_SOURCE_DIR}/third-party/imgui/backends/imgui_impl_glfw.cpp
    ${PROJECT_SOURCE_DIR}/third-party/imgui/backends/imgui_impl_vulkan.cpp)
 
set(IMGUI_SOURCES ${IMGUI_CORE_FILES} ${IMGUI_BACKEND_FILES} PARENT_SCOPE)