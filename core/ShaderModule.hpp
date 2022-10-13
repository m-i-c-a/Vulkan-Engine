#ifndef MICA_SHADER_MODULE_HPP
#define MICA_SHADER_MODULE_HPP

#include <vulkan/vulkan.h>

#include "Resource.hpp"

struct ShaderModule : public Resource
{
    VkShaderModule m_vkShaderModule;

    ShaderModule() = default;
    ~ShaderModule();

    void create(const char* filename);
};

#endif // MICA_BUFFER_HPP