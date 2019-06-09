#pragma once

#include <Core/Misc/Defines/Common.hpp>

TRE_NS_START

template<typename T1, typename T2>
struct Pair
{
	Pair(const T1& f, const T2& s) : first(f), second(s)
	{}

	Pair()
	{}

	Pair(T1&& f, T2&& s) : first(std::forward<T1>(f)), second(std::forward<T2>(s))
	{}

	T1 first;
	T2 second;
};

TRE_NS_END