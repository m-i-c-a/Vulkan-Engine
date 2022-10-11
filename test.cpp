#include <vector>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "Defines.hpp"


/*
Programmable Vertex Pulling (PVP) on Triangle
PVP on obj file mesh
Phong Shading
Bindless PerDrawData (push constants (PC))
PVP on multple obj file meshes
Textures in samplers
Sparse Textures
Bindless PerMaterialData (PC)
DrawIndirect
DrawIndirect + instancing
Auto gen shader structs (offline/pre-step)
PBR materials

Research
--------
PVP vs Traditional

Projects
----------------
Neural Net Trees
Clouds
Scene Graph Exploration
Culling
*/


struct VulkanInitInfo
{
    GLFWwindow* window = nullptr;
    std::vector<const char*> requestedInstanceLayerNames {};
    std::vector<const char*> requestedInstanceExtensionNames {};
    std::vector<const char*> requestedDeviceExtensionNames {};
    uint32_t requestedSwapchainImageCount;
    VkFormat requestedSwapchainImageFormat;
    VkExtent2D requestedSwapchainImageExtent;
    VkPresentModeKHR requestedSwapchainImagePresentMode;
    // std::vector<VkQueueFlagBits> requestedQs {}; // for now we default to a single graphics queue
};

struct QFamIndices
{
    uint32_t graphics = UINT32_MAX;
    uint32_t transfer = UINT32_MAX;
    uint32_t compute = UINT32_MAX;
    uint32_t sparse = UINT32_MAX;
};

VkInstance createInstance(const uint32_t enabledLayerCount, const char* const* ppEnabledLayerNames, const uint32_t enabledExtensionCount, const char* const* ppEnabledExtensionNames)
{
    VkInstance vk_instance = VK_NULL_HANDLE;

    const VkApplicationInfo appInfo {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .apiVersion = VK_API_VERSION_1_3,
    };

    const VkInstanceCreateInfo createInfo {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = enabledLayerCount,
        .ppEnabledLayerNames = ppEnabledLayerNames,
        .enabledExtensionCount = enabledExtensionCount,
        .ppEnabledExtensionNames = ppEnabledExtensionNames
    };

    VK_CHECK(vkCreateInstance(&createInfo, nullptr, &vk_instance));
    return vk_instance;
}

VkSurfaceKHR createSurface(const VkInstance vk_instance, GLFWwindow *window)
{
    VkSurfaceKHR vk_surface = VK_NULL_HANDLE;
    VK_CHECK(glfwCreateWindowSurface(vk_instance, window, nullptr, &vk_surface));
    return vk_surface;
}

VkPhysicalDevice selectPhysicalDevice(const VkInstance vk_instance)
{
    uint32_t numPhysicalDevices = 0u;
    VkPhysicalDevice* vk_physicalDevices = nullptr;
    const uint32_t selectedPhysicalDeviceIndex = 2u;

    VK_CHECK(vkEnumeratePhysicalDevices(vk_instance, &numPhysicalDevices, nullptr));
    vk_physicalDevices = new VkPhysicalDevice[numPhysicalDevices];
    vkEnumeratePhysicalDevices(vk_instance, &numPhysicalDevices, vk_physicalDevices);

    LOG("# Physical Devices: %u\n", numPhysicalDevices);
    for (uint32_t i = 0; i < numPhysicalDevices; ++i)
    {
        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(vk_physicalDevices[i], &props);
        LOG("%i : %s\n", i, props.deviceName);
    }
    LOG("Using Physical Device %u\n\n", selectedPhysicalDeviceIndex);

    delete [] vk_physicalDevices;

    return vk_physicalDevices[selectedPhysicalDeviceIndex];
}

uint32_t selectGraphicsQFamIdx(const VkPhysicalDevice vk_physicalDevice, const VkSurfaceKHR vk_surface)
{
    uint32_t numQFamProps= 0;
    VkQueueFamilyProperties* vk_qFamProps = nullptr;

    vkGetPhysicalDeviceQueueFamilyProperties(vk_physicalDevice, &numQFamProps, nullptr);
    vk_qFamProps = new VkQueueFamilyProperties[numQFamProps];
    vkGetPhysicalDeviceQueueFamilyProperties(vk_physicalDevice, &numQFamProps, vk_qFamProps);

    for (uint8_t i = 0; i < numQFamProps; ++i)
    {
        if (vk_qFamProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            VkBool32 qFamSupportsPresent = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(vk_physicalDevice, i, vk_surface, &qFamSupportsPresent);
            if (qFamSupportsPresent)
            {
                delete vk_qFamProps;
                return i;
            }
        }
    }

    delete [] vk_qFamProps;
    EXIT("Failed to find suitable graphics queue.");
    return UINT32_MAX;
}

VkDevice createDevice(const VkPhysicalDevice vk_physicalDevice, const uint32_t graphicsQFamIdx, std::vector<const char*> deviceExtensionNames) 
{
    constexpr float q_priority = 1.0f;

    const VkDeviceQueueCreateInfo vk_deviceQCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = graphicsQFamIdx,
        .queueCount = 1u,
        .pQueuePriorities = &q_priority
    };

    const VkDeviceCreateInfo vk_deviceCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = nullptr,
        .queueCreateInfoCount = 1u,
        .pQueueCreateInfos = &vk_deviceQCreateInfo,
        .enabledLayerCount = 0u,
        .ppEnabledLayerNames = nullptr,
        .enabledExtensionCount = static_cast<uint32_t>(deviceExtensionNames.size()),
        .ppEnabledExtensionNames = deviceExtensionNames.data(),
        .pEnabledFeatures = nullptr
    };

    VkDevice vk_device;
    VK_CHECK(vkCreateDevice(vk_physicalDevice, &vk_deviceCreateInfo, nullptr, &vk_device));
    return vk_device;
}

VkQueue getGraphicsQ(VkDevice vk_device, uint32_t graphicsQFamIdx)
{
   VkQueue vk_graphicsQ;
   vkGetDeviceQueue(vk_device, graphicsQFamIdx, 0, &vk_graphicsQ);
   return vk_graphicsQ;
}

uint32_t getSwapchainMinImageCount(const VkSurfaceCapabilitiesKHR& vk_surfaceCapabilities, const uint32_t requestedImageCount)
{
    assert(requestedImageCount > 0 && "Invalid requested image count for swapchain!");

    uint32_t minImageCount = UINT32_MAX;

    // If the maxImageCount is 0, then there is not a limit on the number of images the swapchain
    // can support (ignoring memory constraints). See the Vulkan Spec for more information.

    if (vk_surfaceCapabilities.maxImageCount == 0)
    {
        if (requestedImageCount >= vk_surfaceCapabilities.minImageCount)
        {
            minImageCount = requestedImageCount;
        }
        else
        {
            EXIT("Failed to create Swapchain. The requested number of images %u does not meet the minimum requirement of %u.\n", requestedImageCount, vk_surfaceCapabilities.minImageCount);
        }
    }
    else if (requestedImageCount >= vk_surfaceCapabilities.minImageCount && requestedImageCount <= vk_surfaceCapabilities.maxImageCount)
    {
        minImageCount = requestedImageCount;
    }
    else
    {
        EXIT("The number of requested Swapchain images %u is not supported. Min: %u Max: %u.\n", requestedImageCount, vk_surfaceCapabilities.minImageCount, vk_surfaceCapabilities.maxImageCount);
    }

    return minImageCount;
}

void getSwapchainImageFormatAndColorSpace(const VkPhysicalDevice vk_physicalDevice, const VkSurfaceKHR vk_surface, const VkFormat requestedFormat, VkFormat& chosenFormat, VkColorSpaceKHR& chosenColorSpace)
{
    uint32_t numSupportedSurfaceFormats = 0;
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(vk_physicalDevice, vk_surface, &numSupportedSurfaceFormats, nullptr));
    VkSurfaceFormatKHR* supportedSurfaceFormats = new VkSurfaceFormatKHR[numSupportedSurfaceFormats];
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(vk_physicalDevice, vk_surface, &numSupportedSurfaceFormats, supportedSurfaceFormats));

    bool requestedFormatFound = false;
    for (uint32_t i = 0; i < numSupportedSurfaceFormats; ++i)
    {
        if (supportedSurfaceFormats[i].format == requestedFormat)
        {
            chosenFormat = supportedSurfaceFormats[i].format;
            chosenColorSpace = supportedSurfaceFormats[i].colorSpace;
            requestedFormatFound = true;
            break;
        }
    }

    if (!requestedFormatFound)
    {
        chosenFormat = supportedSurfaceFormats[0].format;
        chosenColorSpace = supportedSurfaceFormats[0].colorSpace;
    }

    delete [] supportedSurfaceFormats;
}

VkExtent2D getSwapchainExtent(const VkSurfaceCapabilitiesKHR& vk_surfaceCapabilities, const VkExtent2D vk_requestedImageExtent)
{
    VkExtent2D vk_extent;
    
    // The Vulkan Spec states that if the current width/height is 0xFFFFFFFF, then the surface size
    // will be deteremined by the extent specified in the VkSwapchainCreateInfoKHR.

    if (vk_surfaceCapabilities.currentExtent.width != (uint32_t)-1)
    {
        vk_extent = vk_requestedImageExtent;
    }
    else
    {
        vk_extent = vk_surfaceCapabilities.currentExtent;
    }

    return vk_extent;
}

VkSurfaceTransformFlagBitsKHR getSwapchainPreTransform(const VkSurfaceCapabilitiesKHR& vk_surfaceCapabilities)
{
    VkSurfaceTransformFlagBitsKHR vk_preTransform = VK_SURFACE_TRANSFORM_FLAG_BITS_MAX_ENUM_KHR;

    if (vk_surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
    {
        vk_preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    }
    else
    {
        vk_preTransform = vk_surfaceCapabilities.currentTransform;
        LOG("WARNING - Swapchain pretransform is not IDENTITIY_BIT_KHR!\n");
    }

    return vk_preTransform;
}

VkCompositeAlphaFlagBitsKHR getSwapchainCompositeAlpha(const VkSurfaceCapabilitiesKHR& vk_surfaceCapabilities)
{
    VkCompositeAlphaFlagBitsKHR vk_compositeAlpha = VK_COMPOSITE_ALPHA_FLAG_BITS_MAX_ENUM_KHR;

    // Determine the composite alpha format the application needs.
    // Find a supported composite alpha format (not all devices support alpha opaque),
    // but we prefer it.
    // Simply select the first composite alpha format available
    // Used for blending with other windows in the system

    const VkCompositeAlphaFlagBitsKHR vk_compositeAlphaFlags[4] = {
        VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
    };

    for (size_t i = 0; i < 4; ++i)
    {
        if (vk_surfaceCapabilities.supportedCompositeAlpha & vk_compositeAlphaFlags[i]) 
        {
            vk_compositeAlpha = vk_compositeAlphaFlags[i];
            break;
        };
    }

    return vk_compositeAlpha;
}

VkPresentModeKHR getSwapchainPresentMode(const VkPhysicalDevice vk_physicalDevice, const VkSurfaceKHR vk_surface, const VkPresentModeKHR vk_requestedPresentMode)
{
    VkPresentModeKHR vk_presentMode = VK_PRESENT_MODE_MAX_ENUM_KHR;
    uint32_t numSupportedPresentModes = 0;
    VkPresentModeKHR* supportedPresentModes = nullptr;

    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(vk_physicalDevice, vk_surface, &numSupportedPresentModes, nullptr));
    supportedPresentModes = new VkPresentModeKHR[numSupportedPresentModes];
    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(vk_physicalDevice, vk_surface, &numSupportedPresentModes, supportedPresentModes));

    // Determine the present mode the application needs.
    // Try to use mailbox, it is the lowest latency non-tearing present mode
    // All devices support FIFO (this mode waits for the vertical blank or v-sync)

    vk_presentMode = VK_PRESENT_MODE_FIFO_KHR;

    for (uint32_t i = 0; i < numSupportedPresentModes; ++i)
    {
        if (supportedPresentModes[i] == vk_requestedPresentMode)
        {
            vk_presentMode = vk_requestedPresentMode;
            break;
        }
    }

    delete [] supportedPresentModes;
    return vk_presentMode;
}

VkSwapchainCreateInfoKHR populateSwapchainCreateInfo(VkPhysicalDevice vk_physicalDevice, VkSurfaceKHR vk_surface, VkDevice vk_device, const uint32_t requestedImageCount, const VkFormat vk_requestedFormat, const VkExtent2D vk_requestedExtent, const VkPresentModeKHR vk_requestedPresentMode)
{
    VkSurfaceCapabilitiesKHR vk_surfaceCapabilities;
    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk_physicalDevice, vk_surface, &vk_surfaceCapabilities));

    VkFormat vk_imageFormat;
    VkColorSpaceKHR vk_imageColorSpace;
    getSwapchainImageFormatAndColorSpace(vk_physicalDevice, vk_surface, vk_requestedFormat, vk_imageFormat, vk_imageColorSpace);

    VkSwapchainCreateInfoKHR vk_swapchainCreateInfo {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = vk_surface,
        .minImageCount = getSwapchainMinImageCount(vk_surfaceCapabilities, requestedImageCount),
        .imageFormat = vk_imageFormat,
        .imageColorSpace = vk_imageColorSpace,
        .imageExtent = getSwapchainExtent(vk_surfaceCapabilities, vk_requestedExtent),
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = nullptr,
        .preTransform = getSwapchainPreTransform(vk_surfaceCapabilities),
        .compositeAlpha = getSwapchainCompositeAlpha(vk_surfaceCapabilities),
        .presentMode = getSwapchainPresentMode(vk_physicalDevice, vk_surface, vk_requestedPresentMode),
        .clipped = VK_TRUE,
        .oldSwapchain = VK_NULL_HANDLE,
    };

    return vk_swapchainCreateInfo;
}

VkSwapchainKHR createSwapchain(const VkDevice vk_device, const VkSwapchainCreateInfoKHR& vk_swapchainCreateInfo)
{
    VkSwapchainKHR vk_swapchain;
    VK_CHECK(vkCreateSwapchainKHR(vk_device, &vk_swapchainCreateInfo, nullptr, &vk_swapchain));
    return vk_swapchain;
}

#if 0
uint32_t createSwapchainImages(VkDevice vk_device, VkSwapchainKHR vk_swapchain, std::vector<VkImage>& images, std::vector<VkImageView>& imageViews, VkFormat format)
{
    uint32_t numSwapchainImages = 0;
    VK_CHECK(vkGetSwapchainImagesKHR(device, swapchain, &numSwapchainImages, nullptr));
    images.resize(numSwapchainImages);
    imageViews.resize(numSwapchainImages);
    VK_CHECK(vkGetSwapchainImagesKHR(device, swapchain, &numSwapchainImages, images.data()));

    VkImageViewCreateInfo imageViewCreateInfo {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = format,
        .components = {
            .r = VK_COMPONENT_SWIZZLE_IDENTITY,
            .g = VK_COMPONENT_SWIZZLE_IDENTITY,
            .b = VK_COMPONENT_SWIZZLE_IDENTITY,
            .a = VK_COMPONENT_SWIZZLE_IDENTITY },
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1 },
    };

    for (size_t i = 0; i < numSwapchainImages; ++i)
    {
        imageViewCreateInfo.image = images[i];
        VK_CHECK(vkCreateImageView(device, &imageViewCreateInfo, nullptr, &imageViews[i]));
    }

    std::cout << "Swapchain Image Count: " << numSwapchainImages << '\n';
    return numSwapchainImages;
}
#endif


void initVulkan(const VulkanInitInfo& vulkanInitInfo)
{
    VkInstance vk_instance = VK_NULL_HANDLE;
    VkSurfaceKHR vk_surface = VK_NULL_HANDLE;
    VkPhysicalDevice vk_physicalDevice = VK_NULL_HANDLE;
    uint32_t graphicsQFamIdx = UINT32_MAX;
    VkDevice vk_device = VK_NULL_HANDLE;
    VkQueue vk_graphicsQ = VK_NULL_HANDLE;
    VkSwapchainCreateInfoKHR vk_swapchainCreateInfo;
    VkSwapchainKHR vk_swapchain = VK_NULL_HANDLE;

    vk_instance = createInstance(vulkanInitInfo.requestedInstanceLayerNames.size(), vulkanInitInfo.requestedInstanceLayerNames.data(), vulkanInitInfo.requestedInstanceExtensionNames.size(), vulkanInitInfo.requestedInstanceExtensionNames.data());
    vk_surface = createSurface(vk_instance, vulkanInitInfo.window);
    vk_physicalDevice = selectPhysicalDevice(vk_instance);
    graphicsQFamIdx = selectGraphicsQFamIdx(vk_physicalDevice, vk_surface);
    vk_device = createDevice(vk_physicalDevice, graphicsQFamIdx, vulkanInitInfo.requestedDeviceExtensionNames);
    vk_graphicsQ = getGraphicsQ(vk_device, graphicsQFamIdx);
    vk_swapchainCreateInfo = populateSwapchainCreateInfo(vk_physicalDevice, vk_surface, vk_device, vulkanInitInfo.requestedSwapchainImageCount, vulkanInitInfo.requestedSwapchainImageFormat, vulkanInitInfo.requestedSwapchainImageExtent, vulkanInitInfo.requestedSwapchainImagePresentMode);
    vk_swapchain = createSwapchain(vk_device, vk_swapchainCreateInfo);
    // swapchain images
}

int main()
{
    const int windowWidth = 500;
    const int windowHeight = 500;
    const char* windowName = "Engine";
    bool continueExecuting = true;

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    GLFWwindow* glfw_window = glfwCreateWindow(windowWidth, windowHeight, windowName, nullptr, nullptr);
    assert(glfw_window && "Failed to create window");

    const VulkanInitInfo vulkanInitInfo {
        .window = glfw_window,
        .requestedInstanceLayerNames = { "VK_LAYER_KHRONOS_validation" },
        .requestedInstanceExtensionNames = { "VK_KHR_surface", "VK_KHR_xcb_surface" },
        .requestedDeviceExtensionNames = { VK_KHR_SWAPCHAIN_EXTENSION_NAME },
        .requestedSwapchainImageCount = 3,
        .requestedSwapchainImageFormat = VK_FORMAT_R8G8B8_SRGB,
        .requestedSwapchainImageExtent = { windowWidth, windowHeight },
        .requestedSwapchainImagePresentMode = VK_PRESENT_MODE_FIFO_KHR
    };

    initVulkan(vulkanInitInfo);

    while (!glfwWindowShouldClose(glfw_window))
    {
        glfwPollEvents();

        glfwSwapBuffers(glfw_window);
    }

    glfwDestroyWindow(glfw_window);
    glfwTerminate();
}