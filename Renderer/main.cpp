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

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Renderer/Misc/stb_image.hpp>

//#define RASTER
#ifdef RASTER
#include <zApp/raster.hpp>
#else
#include <zApp/raytracing.hpp>
#endif

int main()
{
#ifdef RASTER
    raster();
#else
    rt();
#endif
}

#endif