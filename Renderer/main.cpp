#include "pch.hpp"

#if not defined(BUILD_EXEC)

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

#else

#include <vulkan/vulkan.h>
#include <vector>
#include <iostream>
#include <Renderer/Core/Renderer.hpp>
#include <Renderer/Core/SwapChain/SwapChain.hpp>
#include <Renderer/Window/Window.hpp>

int main()
{
    /*uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    std::vector<const char*> extensionsNames(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    int i = 0;
    std::cout << "available extensions:\n";
    for (const auto& extension : extensions) {
        std::cout << '\t' << extension.extensionName << '\n';
        extensionsNames[i++] = extension.extensionName;
    }*/

    const unsigned int SCR_WIDTH = 1920 / 2;
    const unsigned int SCR_HEIGHT = 1080 / 2;

    TRE::Window window(SCR_WIDTH, SCR_HEIGHT, "Trikyta ENGINE 3 (Vulkan 1.2)", WindowStyle::Resize);
    TRE::Renderer::RenderContext ctx{0};
    ctx.window = &window;

    if (TRE::Renderer::Init(ctx) == 0) {
        puts("Creation with sucesss !");
    }

    TRE::Event ev;

    while (window.getEvent(ev)) {
        TRE::Renderer::Present(ctx, {});
    }
    
    getchar();
}

#endif