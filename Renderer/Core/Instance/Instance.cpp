#include "Instance.hpp"

int32 Renderer::CreateRenderContext(Renderer::RenderContext* p_ctx)
{
    int32 err_code; 
    err_code = CreateInstance(&p_ctx->instance);
    err_code |= SetupDebugMessenger(p_ctx->instance, &p_ctx->debugMessenger);
    return err_code;
}

void Renderer::DestroyRenderContext(const Renderer::RenderContext& p_ctx)
{
    DestroyDebugUtilsMessengerEXT(p_ctx.instance, p_ctx.debugMessenger, NULL);
    DestroyInstance(p_ctx.instance);
}

int32 Renderer::CreateInstance(VkInstance* p_instance)
{
    if (p_instance == NULL) {
        return -1;
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "TRE Renderer";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "TRE";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_VERSION_1_2;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    // Extensions:
    createInfo.enabledExtensionCount = VK_REQ_EXTENSIONS.size();
    createInfo.ppEnabledExtensionNames = VK_REQ_EXTENSIONS.begin();

    // Layers:
    createInfo.enabledLayerCount = VK_REQ_LAYERS.size();
    createInfo.ppEnabledLayerNames = VK_REQ_LAYERS.begin();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

    if (Renderer::InitDebugMessengerCreateInfo(debugCreateInfo) == 0) {
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
    } else {
        createInfo.pNext = NULL;
    }

    if (vkCreateInstance(&createInfo, NULL, p_instance) != VK_SUCCESS) {
        return -1;
    }

    return 0;
}

void Renderer::DestroyInstance(VkInstance p_instance)
{
    if (p_instance == NULL) {
        return;
    }

    vkDestroyInstance(p_instance, NULL);
}

// DEBUGGING SECTION // 

int32 Renderer::SetupDebugMessenger(VkInstance p_instance, VkDebugUtilsMessengerEXT* p_debugMessenger)
{
#if defined(DEBUG)
    if (!p_debugMessenger)
        return -1;

    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    Renderer::InitDebugMessengerCreateInfo(createInfo);

    if (Renderer::CreateDebugUtilsMessengerEXT(p_instance, &createInfo, nullptr, p_debugMessenger) != VK_SUCCESS) {
        return -1;
    }

    return 0;
#else
    return 0;
#endif
}

int32 Renderer::InitDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
#if defined(DEBUG)
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = Renderer::DebugCallback;
    return 0;
#else
    return 1; // No debugging
#endif
}

VkResult Renderer::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
#if defined(DEBUG)
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
#endif
}

void Renderer::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
#if defined(DEBUG)
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
#endif
}

VkBool32 Renderer::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
    fprintf(stderr, "[VALIDATION LAYER]: %s\n", pCallbackData->pMessage);
    return VK_FALSE;
}