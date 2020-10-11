
#define BUILD_EXEC
#define COMPILE_DLL

#if defined(COMPILE_DLL)
#define RENDERER_API __declspec(dllexport)
#else
#define RENDERER_API __declspec(dllimport)
#endif

#undef UNICODE 
#include <Engine/Core/Misc/Defines/Common.hpp>
#include <Engine/Core/Misc/Defines/PlatformInclude.hpp>
#include <Engine/Core/Misc/Defines/Debug.hpp>
#include <Renderer/Core/Logs/Logs.hpp>

#pragma warning(disable:4251) // This is to avoid the spam of warning bcuz of std classes, more concrete solution must be found in the future

// #undef DEBUG
// #define DEBUG

#define VALIDATION_LAYERS

#include <chrono>

#define INIT_BENCHMARK std::chrono::time_point<std::chrono::high_resolution_clock> start, end; std::chrono::microseconds duration;

#define BENCHMARK(name, bloc_of_code) \
    start = std::chrono::high_resolution_clock::now(); \
    bloc_of_code; \
    end = std::chrono::high_resolution_clock::now();\
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start); \
    std::cout << "\nExecution of '" << name << "' took : " << duration.count() << " microsecond(s)" << std::endl; \

#include <vulkan/vulkan.h>