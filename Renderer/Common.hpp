
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

// #undef DEBUG
// #define DEBUG

#define VALIDATION_LAYERS

#include <vulkan/vulkan.h>