#ifndef MICA_VULKAN_WRAPPER_SHADER_MODULE_HPP
#define MICA_VULKAN_WRAPPER_SHADER_MODULE_HPP

#include <vulkan/vulkan.h>

#include "Resource.hpp"

namespace VulkanWrapper
{

struct ShaderModule : public Resource
{
    VkShaderModule m_vkShaderModule;

    ShaderModule() = default;
    ~ShaderModule();

    void create(const char* filename);
};

}; // Vulkan Wrapper

#endif // MICA_VULKAN_WRAPPER_SHADER_MODULE_HPP