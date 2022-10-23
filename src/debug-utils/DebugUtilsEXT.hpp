#ifndef MICA_DEBUG_UTILS_EXT_HPP
#define MICA_DEBUG_UTILS_EXT_HPP

#include <vulkan/vulkan.h>

namespace DebugUtilsEXT
{

void init(const VkInstance vk_instance, const VkDevice _vk_device);

void setObjectName(const VkObjectType vk_objectType, const uint64_t handle, const char* name);

}; // DebugUtilsEXT

#endif // MICA_DEBUG_UTILS_EXT_HPP