#include "RenderInstance.hpp"

TRE_NS_START

Renderer::RenderInstance::RenderInstance() : internal{0}
{

}

Renderer::RenderInstance::~RenderInstance()
{

}

int32 Renderer::RenderInstance::CreateRenderInstance(const char** extensions, uint32 extCount, const char** layers, uint32 layerCount)
{
    int32 err_code; 
    err_code = CreateInstance(&internal.instance);
    err_code |= SetupDebugMessenger(internal.instance, &internal.debugMessenger);
    return err_code;
}

void Renderer::RenderInstance::DestroyRenderInstance()
{
    DestroyDebugUtilsMessengerEXT(internal.instance, internal.debugMessenger, NULL);
    DestroyInstance(internal.instance);
}

int32 Renderer::RenderInstance::CreateInstance(VkInstance* p_instance, const char** extensions, uint32 extCount, const char** layers, uint32 layerCount)
{
    ASSERT(p_instance == NULL);

    this->FetchAvailbleInstanceExtensions();

    StaticVector<const char*> extensionsArr;
    StaticVector<const char*> layersArr;

    for (const auto& ext : VK_REQ_EXT) {
        extensionsArr.PushBack(ext);
        Hash h = Utils::Data(ext, strlen(ext));

        if (availbleInstExtensions.find(h) == availbleInstExtensions.end()) {
            TRE_LOGE("Can't load mandatory extension '%s' not supported by VK instance", ext);
            return -1;
        }

        deviceExtensions.emplace();
    }

    for (uint32 i = 0; i < extCount; i++) {
        extensionsArr.PushBack(extensions[i]);
        Hash h = Utils::Data(extensions[i], strlen(extensions[i]));

        if (availbleInstExtensions.find(h) == availbleInstExtensions.end()) {
            TRE_LOGW("Skipping extension '%s' not supported by VK instance", extensions[i]);
            continue;
        }

        deviceExtensions.emplace();
    }

    for (const auto& ext : VK_REQ_LAYERS) {
        layersArr.PushBack(ext);
    }

    for (uint32 i = 0; i < layerCount; i++) {
        layersArr.PushBack(layers[i]);
    }


    VkApplicationInfo appInfo{};
    appInfo.sType               = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName    = "TRE Renderer";
    appInfo.applicationVersion  = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName         = "Trikyta Engine";
    appInfo.engineVersion       = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion          = VK_VERSION_1_2;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType                    = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo         = &appInfo;

    // Extensions:
    createInfo.enabledExtensionCount    = (uint32)extensionsArr.Size();//(uint32)VK_REQ_EXT.size();
    createInfo.ppEnabledExtensionNames  = extensionsArr.begin();//VK_REQ_EXT.begin();

    // Layers:
    createInfo.enabledLayerCount        = (uint32)layersArr.Size();     //(uint32)VK_REQ_LAYERS.size();
    createInfo.ppEnabledLayerNames      = layersArr.begin();    //VK_REQ_LAYERS.begin();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

    if (InitDebugMessengerCreateInfo(debugCreateInfo) == 0) {
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
    } else {
        createInfo.pNext = NULL;
    }

    //if (vkCreateInstance(&createInfo, NULL, p_instance) != VK_SUCCESS) {
    //    return -1;
    //}
    CALL_VK(vkCreateInstance(&createInfo, NULL, p_instance));

    return 0;
}

void Renderer::RenderInstance::FetchAvailbleInstanceExtensions()
{

    uint32 extensionsCount;
    StaticVector<VkExtensionProperties, 256> extensionsAvailble;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, extensionsAvailble.begin());
    extensionsAvailble.Resize(extensionsCount);

    for (const auto& ext : extensionsAvailble) {
        availbleInstExtensions.emplace(Utils::Data(ext.extensionName, strlen(ext.extensionName)));
    }
}

void Renderer::RenderInstance::DestroyInstance(VkInstance p_instance)
{
    ASSERT(p_instance == VK_NULL_HANDLE);

    vkDestroyInstance(p_instance, NULL);
}

// DEBUGGING SECTION // 

int32 Renderer::RenderInstance::SetupDebugMessenger(VkInstance p_instance, VkDebugUtilsMessengerEXT* p_debugMessenger)
{
#if defined(DEBUG) && defined(VALIDATION_LAYERS)
    ASSERT(p_debugMessenger == VK_NULL_HANDLE);

    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    InitDebugMessengerCreateInfo(createInfo);

    if (CreateDebugUtilsMessengerEXT(p_instance, &createInfo, nullptr, p_debugMessenger) != VK_SUCCESS) {
        return -1;
    }

    return 0;
#else
    return 0;
#endif
}

int32 Renderer::RenderInstance::InitDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
#if defined(DEBUG) && defined(VALIDATION_LAYERS)
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = Renderer::RenderInstance::DebugCallback;
    return 0;
#else
    return 1; // No debugging
#endif
}

VkResult Renderer::RenderInstance::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
#if defined(DEBUG) && defined(VALIDATION_LAYERS)
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
#endif
    return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void Renderer::RenderInstance::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
#if defined(DEBUG) && defined(VALIDATION_LAYERS)
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
#endif
}

VkBool32 Renderer::RenderInstance::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
    if ((VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT & messageType) ||
        (VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT & messageType)) {
        fprintf(stderr, "[VALIDATION LAYER]: %s\n", pCallbackData->pMessage);
        ASSERT(true);
    }

    return VK_FALSE;
}

TRE_NS_END