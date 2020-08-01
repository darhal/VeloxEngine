#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <Engine/Core/Misc/Defines/Common.hpp>

#if defined(COMPILER_MSVC)
#	pragma warning(disable:4996)
#endif

#if defined(OS_WINDOWS)
#	include <Windows.h>
#	include <processenv.h>
#	include <ConsoleApi2.h>
#	undef min
#	undef max
#endif

TRE_NS_START

typedef class Log
{
public:
	enum LogType {
		INFO = 0,
		WARN = 1,
		ERR = 2,
		FATAL = 3,
		ASSERT = 4,
		DEBUG = 5,
		DISPLAY = 6,
		OTHER = 7,
	};

	static void Write(LogType logtype, const char* msg, ...) {
		va_list arg;
		va_start(arg, msg);
		Log::WriteHelper(logtype, msg, arg);
		va_end(arg);
	}

	static void Write(const char* msg, ...) {
		va_list arg;
		va_start(arg, msg);
		Log::WriteHelper(INFO, msg, arg);
		va_end(arg);
	}

private:
	CONSTEXPR static const char* logtype2str[] = { "INFO", "WARNING", "ERROR", "FATAL", "ASSERT", "DEBUG", "DISPLAY", "OTHER" };
	/* GREEN, YELLOW, RED, DARK RED, PURPLE, GREY, CYAN, DEFAULT*/
	CONSTEXPR static const uint8 logtype2color[] = { 10, 14, 12, 4, 13, 8, 11, 15}; 
	CONSTEXPR static const usize MAX_MESSAGE_BUFFER = 1024;

	static void WriteHelper(LogType logtype, const char* msg, va_list ap)
	{
		char text_message_buffer[MAX_MESSAGE_BUFFER];
		char log_message[MAX_MESSAGE_BUFFER];
		char date_str[26];
		Log::GetDateString(date_str, 26);
		snprintf(text_message_buffer, MAX_MESSAGE_BUFFER, "[%s] [TrikytaEngine][%s]: ", date_str, Log::GetStringFromLogType(logtype));
		vsnprintf(log_message, MAX_MESSAGE_BUFFER, msg, ap);
		strcat(text_message_buffer, log_message);

		Log::SetColor(Log::GetColorFromLogType(logtype));
		printf("%s\n", text_message_buffer);
	}

	FORCEINLINE static const char* GetStringFromLogType(LogType v) {
		return logtype2str[v];
	}

	FORCEINLINE static const uint8 GetColorFromLogType(LogType v) {
		return logtype2color[v];
	}

	FORCEINLINE static void GetDateString(char* buffer, uint32 size)
	{
		time_t t = time(NULL);
		struct tm tm = *localtime(&t);
		strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", &tm);
	}

	FORCEINLINE static void SetColor(uint8 color_code)
	{
#if defined(OS_WINDOWS)
		// const HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		// SetConsoleTextAttribute(hConsole, color_code);
#endif
	}

	FORCEINLINE static void ResetColors()
	{
#if defined(OS_WINDOWS)
		const HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, 15);
#endif
	}
} LOG;

TRE_NS_END