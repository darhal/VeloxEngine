#include "WindowSurface.hpp"
#include <Renderer/Core/Common/Utils.hpp>
#include <Renderer/Window/Window.hpp>

TRE_NS_START

int32 Renderer::CreateWindowSurface(RenderContext& p_ctx, const VkAllocationCallbacks* allocator)
{
	ASSERT(p_ctx.window == NULL);
	ASSERT(p_ctx.instance == NULL);

    VkInstance instance = p_ctx.instance->instance;
    ASSERT(instance == NULL);

    VkResult err;
    VkWin32SurfaceCreateInfoKHR sci{};
    PFN_vkCreateWin32SurfaceKHR vkCreateWin32SurfaceKHR;

    vkCreateWin32SurfaceKHR = (PFN_vkCreateWin32SurfaceKHR)vkGetInstanceProcAddr(instance, "vkCreateWin32SurfaceKHR");

    if (!vkCreateWin32SurfaceKHR) {
        ASSERTF(true, "Win32: Vulkan instance missing VK_KHR_win32_surface extension");
        return -1;
    }

    sci.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    sci.hinstance = GetModuleHandle(NULL);
    sci.hwnd = (HWND)p_ctx.window->GetNativeHandle();

    err = vkCreateWin32SurfaceKHR(instance, &sci, allocator, &p_ctx.surface);

    if (err) {
        ASSERTF(true, "Win32: Failed to create Vulkan surface: %s", GetVulkanResultString(err));
    }

    return err;
}

void Renderer::DestroryWindowSurface(VkInstance instance, VkSurfaceKHR surface)
{
    vkDestroySurfaceKHR(instance, surface, NULL);
}

TRE_NS_END