#if defined(COMPILE_DLL)
#define RENDERER_API __declspec(dllexport)
#else
#define RENDERER_API __declspec(dllimport)
#endif

#include <Engine/>