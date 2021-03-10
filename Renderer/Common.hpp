
#define BUILD_EXEC
#define COMPILE_DLL

#if defined(COMPILE_DLL)
    #if defined(COMPILER_MSVC)
        #define RENDERER_API __declspec(dllexport)
    #else
        #define RENDERER_API 
    #endif
#else
    #if defined(COMPILER_MSVC)
        #define RENDERER_API __declspec(dllimport)
    #else
        #define RENDERER_API 
    #endif
#endif

#undef UNICODE 
#include <Engine/Core/Misc/Defines/Common.hpp>
#include <Engine/Core/Misc/Defines/PlatformInclude.hpp>
#include <Engine/Core/Misc/Defines/Debug.hpp>
#include <Renderer/Core/Logs/Logs.hpp>

#if defined(COMPILER_MSVC)
    #pragma warning(disable:4251) // This is to avoid the spam of warning bcuz of std classes, more concrete solution must be found in the future
#endif

// #undef DEBUG
// #define DEBUG

#define VALIDATION_LAYERS
// #undef VALIDATION_LAYERS

#include <chrono>

#define INIT_BENCHMARK std::chrono::time_point<std::chrono::high_resolution_clock> start, end; std::chrono::microseconds duration;

#define BENCHMARK(name, bloc_of_code) \
    start = std::chrono::high_resolution_clock::now(); \
    bloc_of_code; \
    end = std::chrono::high_resolution_clock::now();\
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start); \
    std::cout << "\nExecution of '" << name << "' took : " << duration.count() << " microsecond(s)" << std::endl; \

#include <vulkan/vulkan.h>

// checks on OS
#if defined(OS_WINDOWS)
#include <vulkan/vulkan_win32.h>
#elif defined(OS_LINUX)
#include <vulkan/vulkan_xlib.h>
#endif
