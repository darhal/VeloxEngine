#pragma once

#include "Core/Misc/Defines/Common.hpp"
#include "Core/DataStructure/String/String.hpp"

TRE_NS_START

template<typename T>
FORCEINLINE usize Hash(const T& val)
{
	return std::hash<T>{}(val);
}

template<>
FORCEINLINE usize Hash(const int32& val)
{
	return val;
}

template<>
FORCEINLINE usize Hash(const String& val)
{
	return Hash(std::string(val.Buffer()));
}

TRE_NS_END