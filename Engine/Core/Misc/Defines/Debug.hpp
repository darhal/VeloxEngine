#pragma once

#include <stdio.h>
#include <ctime>
#include "Platform.hpp"

#if defined(COMPILER_MSVC)
	#pragma warning(disable:4996) // DISABLE VS WARNING
#endif

#if defined(OS_LINUX)
	#define _DEBUG // Enable debugging for now !
#endif

#if not defined(_DEBUG) || defined(NDEBUG)
	#define ASSERTF(condition, ...) ;
	#define ASSERT(condition) ;
#else
	#include <Engine/Core/Misc/Utils/Logging.hpp>

    #define DEBUG

	#if defined(OS_WINDOWS) && (CPU_ARCH == CPU_ARCH_x86_64 || CPU_ARCH == CPU_ARCH_x86)
        #include <intrin.h>
		#define DEBUG_BREAK() __debugbreak() 
	#elif defined(OS_UNIX) || defined(OS_LINUX) && not defined(COMPILER_GCC)
		#include <signal.h>
		#define DEBUG_BREAK() raise(SIGINT);
    #elif defined(OS_UNIX) || defined(OS_LINUX) || defined(OS_APPLE)
        #define DEBUG_BREAK() __builtin_trap();
	#else
		#define DEBUG_BREAK()
	#endif
	
	#if defined(OS_WINDOWS)
		#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
	#else
		#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
	#endif

	#if defined(assert)
		#undef assert
	#endif
	#if defined(ASSERT)
		#undef ASSERT
	#endif

	#define ASSERTF(condition, ...) \
		if ((bool)(condition)){ \
			char assert_msg[215]; \
			snprintf(assert_msg, 215, ##__VA_ARGS__); \
			TRE::Log::Write(TRE::Log::ASSERT, "[%s::%d::%s] %s.", __FILENAME__, __LINE__, __FUNCTION__, assert_msg); \
			DEBUG_BREAK(); \
		}\

	#define ASSERT(condition) \
		if ((bool)(condition)){ \
			TRE::Log::Write(TRE::Log::ASSERT, "[%s::%d::%s] Assert occured (Condition: %s).", __FILENAME__, __LINE__, __FUNCTION__, #condition); \
			DEBUG_BREAK(); \
		} \

	#define assert ASSERT
	#define _assert ASSERT

#endif