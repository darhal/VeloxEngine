#pragma once

#ifdef RENDERER_EXPORTS
#define RENDERER_API __declspec(dllexport)
#else
#define RENDERER_API __declspec(dllimport)
#endif

// #define BUILD_LIB
#define BUILD_EXEC

#include <Engine/Core/Misc/Defines/Common.hpp>
#include <Engine/Core/Misc/Defines/PlatformInclude.hpp>