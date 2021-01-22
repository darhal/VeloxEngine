#pragma once

#include <Renderer/Common.hpp>
#include <string.h>
#include <stdlib.h>

TRE_NS_START

namespace Renderer
{
	template<uint32 N = 128>
	class StaticString
	{
	public:
		StaticString(const char* str, bool)
		{
			memcpy(string, str, strlen(str));
		}

		template<uint32 N>
		StaticString(const char(&str)[N])
		{
			memcpy(string, str, N);
		}

		operator char*() const
		{
			return string;
		}

		char operator[](uint32 i)
		{
			return string[i];
		}
	private:
		char string[N];
	};
}

TRE_NS_END