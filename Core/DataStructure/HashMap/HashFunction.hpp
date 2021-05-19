#pragma once

#include "Core/Misc/Defines/Common.hpp"
#include "Core/DataStructure/String/String.hpp"
#include <Core/Math/Maths.hpp>

// http://www.cse.yorku.ca/~oz/hash.html

TRE_NS_START

struct Hash {
	template<typename T, typename std::enable_if<std::is_class<T>::value || std::is_union<T>::value, bool>::type = true>
	FORCEINLINE static usize GetHash(const T& val) { return val.GetHash(); }

	template<typename T>
	FORCEINLINE static usize GetHash(const T* val) { return (usize)val; }

	template<typename T, typename std::enable_if<std::is_enum<T>::value, bool>::type = true>
	FORCEINLINE static usize GetHash(const T& enm) { return (usize)enm; }

	template<typename = void>
	FORCEINLINE static usize GetHash(const float& val) { return Math::HashFloat(val); }

	template<typename = void>
	FORCEINLINE static usize GetHash(const double& val) { return Math::HashDouble(val); }

	template<typename = void>
	FORCEINLINE static usize GetHash(const uint8& val) { return val; }

	template<typename = void>
	FORCEINLINE static usize GetHash(const uint16& val) { return val; }

	template<typename = void>
	FORCEINLINE static usize GetHash(const uint32& val) { return val; }

	template<typename = void>
	FORCEINLINE static usize GetHash(const uint64& val) { return val; }

	template<typename = void>
	FORCEINLINE static usize GetHash(const int8& val) { return val; }

	template<typename = void>
	FORCEINLINE static usize GetHash(const int16& val) { return val; }

	template<typename = void>
	FORCEINLINE static usize GetHash(const int32& val) { return val; }

	template<typename = void>
	FORCEINLINE static usize GetHash(const int64& val) { return val; }

	template<typename = void>
	FORCEINLINE static usize GetHash(const char* str)
	{
		usize hash = 5381;
		uint32 c;

		while ((c = *str++))
			hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

		return hash;
	}
};

TRE_NS_END