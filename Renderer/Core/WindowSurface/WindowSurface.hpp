#pragma once

#include <Renderer/Common.hpp>
#include <Renderer/Core/Common/Globals.hpp>

TRE_NS_START

namespace Renderer
{
    int32 CreateWindowSurface(RenderContext& p_ctx, const VkAllocationCallbacks* allocator = NULL);

    void DestroryWindowSurface(VkInstance instance, VkSurfaceKHR surface);

    typedef VkFlags VkWin32SurfaceCreateFlagsKHR;

    typedef struct VkWin32SurfaceCreateInfoKHR
    {
        VkStructureType                 sType;
        const void* pNext;
        VkWin32SurfaceCreateFlagsKHR    flags;
        HINSTANCE                       hinstance;
        HWND                            hwnd;
    } VkWin32SurfaceCreateInfoKHR;

    typedef VkResult(APIENTRY* PFN_vkCreateWin32SurfaceKHR)(VkInstance, const VkWin32SurfaceCreateInfoKHR*, const VkAllocationCallbacks*, VkSurfaceKHR*);
};

TRE_NS_END