
#define BUILD_EXEC
#define COMPILE_DLL

#if defined(COMPILE_DLL)
#define RENDERER_API __declspec(dllexport)
#else
#define RENDERER_API __declspec(dllimport)
#endif

#include <Engine/Core/Misc/Defines/Common.hpp>
#include <Engine/Core/Misc/Defines/Debug.hpp>
#include <Engine/Core/Misc/Defines/PlatformInclude.hpp>

#include <vulkan/vulkan.h>