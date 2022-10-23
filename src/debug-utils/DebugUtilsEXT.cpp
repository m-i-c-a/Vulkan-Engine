#include "DebugUtilsEXT.hpp"

namespace DebugUtilsEXT
{

static VkDevice vk_device = VK_NULL_HANDLE;
static PFN_vkSetDebugUtilsObjectNameEXT pfnSetDebugUtilsObjectNameEXT;

void init(const VkInstance vk_instance, const VkDevice _vk_device)
{
    pfnSetDebugUtilsObjectNameEXT = (PFN_vkSetDebugUtilsObjectNameEXT)vkGetInstanceProcAddr(vk_instance, "vkSetDebugUtilsObjectNameEXT");
    vk_device = _vk_device;
}

void setObjectName(const VkObjectType vk_objectType, const uint64_t handle, const char* name)
{
    const VkDebugUtilsObjectNameInfoEXT vk_debugObjectName {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
        .objectType = vk_objectType,
        .objectHandle = handle,
        .pObjectName = name,
    };

    pfnSetDebugUtilsObjectNameEXT(vk_device, &vk_debugObjectName);
}

}; // DebugUtilsEXT