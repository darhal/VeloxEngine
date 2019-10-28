#pragma once

#include "Core/Misc/Defines/Common.hpp"
#include "Core/DataStructure/String/String.hpp"

// http://www.cse.yorku.ca/~oz/hash.html

TRE_NS_START

struct Hash {
	template<typename T>
	FORCEINLINE static usize GetHash(const T& val)
	{
		return std::hash<T>{}(val);
	}

	template<>
	FORCEINLINE static usize GetHash(const int32& val)
	{
		return val;
	}

	template<>
	FORCEINLINE static usize GetHash(const String& val)
	{
		// return GetHash(std::string(val.Buffer()));
		const char* str = val.Buffer();
		usize hash = 5381;
		uint32 c;

		while (c = *str++)
			hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

		return hash;
	}

	FORCEINLINE static usize GetHash(const char* str)
	{
		usize hash = 5381;
		uint32 c;

		while (c = *str++)
			hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

		return hash;
	}
};

TRE_NS_END