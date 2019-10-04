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
	#include <Core/Misc/Utils/Logging.hpp>

	#if defined(COMPILER_MSVC) && (CPU_ARCH == CPU_ARCH_x86)
		#define DEBUG_BREAK() __asm { int 3 }
	#elif defined(OS_WINDOWS) && (CPU_ARCH == CPU_ARCH_x86_64)
		#include <Windows.h>
		#include <debugapi.h>

		typedef LONG(NTAPI *NtSuspendProcess)(IN HANDLE ProcessHandle);

		FORCEINLINE void Suspend(DWORD processId)
		{
			HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
			NtSuspendProcess pfnNtSuspendProcess = (NtSuspendProcess) GetProcAddress(GetModuleHandle("ntdll"), "NtSuspendProcess");
			pfnNtSuspendProcess(processHandle);
			CloseHandle(processHandle);
		}

		#define DEBUG_BREAK() Suspend(GetCurrentProcessId());
	#elif defined(OS_UNIX) || defined(OS_LINUX)
		#include <signal.h>
		#define DEBUG_BREAK() raise(SIGINT);
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
			LOG::Write(LOG::ASSERT, "Assert occured in (%s:%d) %s.", __FILENAME__, __LINE__, assert_msg); \
			DEBUG_BREAK(); \
		}\

	#define ASSERT(condition) \
		if ((bool)(condition)){ \
			LOG::Write(LOG::ASSERT, "Assert occured in (%s:%d).", __FILENAME__, __LINE__); \
			DEBUG_BREAK(); \
		} \

	#define assert ASSERT
	#define _assert ASSERT

#endif