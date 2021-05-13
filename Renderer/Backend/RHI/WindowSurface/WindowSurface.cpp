#include "WindowSurface.hpp"
#include <Renderer/Backend/RHI/Common/Utils.hpp>
#include <Renderer/Backend/Window/Window.hpp>

TRE_NS_START

int32 Renderer::Internal::CreateWindowSurface(const RenderInstance& renderInstance, RenderContext& ctx, const VkAllocationCallbacks* allocator)
{
	ASSERT(ctx.window == NULL);
	ASSERT(renderInstance.instance == NULL);

    VkInstance instance = renderInstance.instance;
    VkResult err;

#if defined(OS_WINDOWS)
    VkWin32SurfaceCreateInfoKHR sci{};
    PFN_vkCreateWin32SurfaceKHR vkCreateWin32SurfaceKHR;

    vkCreateWin32SurfaceKHR = (PFN_vkCreateWin32SurfaceKHR)vkGetInstanceProcAddr(instance, "vkCreateWin32SurfaceKHR");

    if (!vkCreateWin32SurfaceKHR) {
        ASSERTF(true, "Win32: Vulkan instance missing VK_KHR_win32_surface extension");
        return -1;
    }

    sci.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    sci.hinstance = GetModuleHandle(NULL);
    sci.hwnd = (HWND)ctx.window->GetNativeHandle();

    err = vkCreateWin32SurfaceKHR(instance, &sci, allocator, &ctx.surface);

    if (err) {
        ASSERTF(true, "Win32: Failed to create Vulkan surface: %s", GetVulkanResultString(err));
    }
#elif defined(OS_LINUX)
    VkXlibSurfaceCreateInfoKHR sci{};
    PFN_vkCreateXlibSurfaceKHR vkCreateXlibSurfaceKHR = (PFN_vkCreateXlibSurfaceKHR)vkGetInstanceProcAddr(instance, "vkCreateXlibSurfaceKHR");

    if (!vkCreateXlibSurfaceKHR) {
        ASSERTF(true, "Xlib: Vulkan instance missing VK_KHR_Xlib_surface extension");
        return -1;
    }

    sci.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
    sci.flags = 0;
    sci.dpy = (Display*)ctx.window->GetNativeHandle();
    sci.window = ctx.window->GetWindowHandle();

    vkCreateXlibSurfaceKHR(instance, &sci, allocator, &ctx.surface);
#endif

    return err;
}

void Renderer::Internal::DestroryWindowSurface(VkInstance instance, VkSurfaceKHR surface)
{
    vkDestroySurfaceKHR(instance, surface, NULL);
}

TRE_NS_END