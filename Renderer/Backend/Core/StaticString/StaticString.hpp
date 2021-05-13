#pragma once

#include <Renderer/Backend/Common.hpp>
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

		template<uint32 SIZE>
		StaticString(const char(&str)[SIZE])
		{
			memcpy(string, str, SIZE);
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