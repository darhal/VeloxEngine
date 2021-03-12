#include "pch.hpp"

#if !defined(BUILD_EXEC)

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

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Renderer/Misc/stb_image.hpp>
#include <Renderer/zApp/Shared.hpp>

#define RASTER
#ifdef RASTER
#include <Renderer/zApp/raster.hpp>
#else
#include <Renderer/zApp/raytracing.hpp>
#endif

int main()
{
    using namespace TRE::Renderer;
    using namespace TRE;

    TRE::Window window(SCR_WIDTH, SCR_HEIGHT, "Trikyta ENGINE 3 (Vulkan 1.2)", WindowStyle::Resize);
    RenderBackend backend{ &window };
    // backend.SetSamplerCount(2);

#ifdef RASTER
    backend.InitInstance();
    raster(backend);
#else
    backend.InitInstance(Features::RAY_TRACING);
    rt(backend);
#endif
}

#endif
