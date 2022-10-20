#include <stdio.h>
#include <stdlib.h>

#include "ShaderModule.hpp"
#include "Defines.hpp"

namespace VulkanWrapper
{

void ShaderModule::create(const char* filename)
{
    FILE* f = fopen(filename, "r");
    if (f == NULL)
    {
        printf("Failed to open file %s!\n", filename);
        exit(EXIT_FAILURE);
    }

    fseek(f, 0, SEEK_END);
    const size_t nbytes_file_size = (size_t)ftell(f);
    rewind(f);

    uint32_t* buffer = (uint32_t*)malloc(nbytes_file_size);
    fread(buffer, nbytes_file_size, 1, f);
    fclose(f);

    VkShaderModuleCreateInfo vk_shaderModuleCreateInfo {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = nbytes_file_size,
        .pCode = buffer,
    };

    VK_CHECK(vkCreateShaderModule(s_vkDevice, &vk_shaderModuleCreateInfo, NULL, &m_vkShaderModule));

    free(buffer);
}

ShaderModule::~ShaderModule()
{
    vkDestroyShaderModule(s_vkDevice, m_vkShaderModule, nullptr);
}

}; // VulkanWrapper