#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <Core/Misc/Defines/Common.hpp>

#if defined(COMPILER_MSVC)
	#pragma warning(disable:4996)
#endif

TRE_NS_START

namespace LogType {
	
	enum log_type_t {
		INFO = 0,
		WARN = 1,
		ERR = 2,
	};
	static FORCEINLINE const char* ToString(log_type_t v) {
		static const char* logtype2str[] = { "INFO", "WARNING", "ERROR" };
		return logtype2str[v];
	}
}

inline static void PrintDate()
{
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	char buffer[26];
	strftime(buffer, 26, "[%Y-%m-%d %H:%M:%S]", &tm);
	printf("%s", buffer);
}

static void Log(LogType::log_type_t logtype, const char* msg, ...) {
	PrintDate(); 
	printf("[TrikytaEngine]"); 
	printf("[%s]:", LogType::ToString(logtype));
	va_list vl;
	va_start(vl, msg);
	vprintf(msg, vl);
	va_end(vl);
}

TRE_NS_END