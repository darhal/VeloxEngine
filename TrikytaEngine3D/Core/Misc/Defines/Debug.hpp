#pragma once

#include <stdio.h>
#include <ctime>
#include "Platform.hpp"

#ifdef _MSC_VER
	#pragma warning(disable:4996) // DISABLE VS WARNING
#endif

inline static void PrintDate()
{
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	char buffer[26];
	strftime(buffer, 26, "[%Y-%m-%d %H:%M:%S]", &tm);
	printf("%s", buffer);
}

#if defined(NDEBUG)
	#define DEBUG_ASSERT
#else
	#include <string.h>

	#if defined(COMPILER_MSVC)
		#define DEBUG_BREAK() __asm { int 3 }
	#else
		//#define DEBUG_BREAK() __asm ( "int 3" );
		#define DEBUG_BREAK();
	#endif
	
	#if defined(OS_WINDOWS)
		#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
	#else
		#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
	#endif

	#if defined(assert)
		#undef assert
	#endif
	#if defined(_ASSERT)
		#undef _ASSERT
	#endif

	#define ASSERTF(condition, ...) \
		if (!(bool)(condition)){ \
			PrintDate(); \
			fprintf(stderr, "[TrikytaEngine]"); \
			fprintf(stderr, "[ASSERT]: Assert occured in "); \
			fprintf(stderr, "(%s:%d) ", __FILENAME__, __LINE__); \
			printf(__VA_ARGS__);	\
			fprintf(stderr, ".\n"); \
			DEBUG_BREAK(); \
		}\

	#define _ASSERT(condition) \
		if (!(bool)(condition)){ \
			PrintDate(); \
			fprintf(stderr, "[TrikytaEngine]"); \
			fprintf(stderr, "[ASSERT]: Assert occured in "); \
			fprintf(stderr, "(%s:%d) ", __FILENAME__, __LINE__); \
			fprintf(stderr, ".\n"); \
			DEBUG_BREAK(); \
		}\

	#define assert ASSERT
	#define _assert _ASSERT

#endif