#pragma once

#include <Legacy/Misc/Defines/Common.hpp>
#include <utility>

TRE_NS_START

template<typename T1, typename T2>
struct Pair
{
	typedef T1 First;
	typedef T2 Second;

	Pair()
	{}

	Pair(const T1& f, const T2& s) : first(f), second(s)
	{}

	template<typename... Args>
	Pair(const T1& f, Args&&... args) : first(f), second(std::forward<Args>(args)...)
	{}

	template<typename... Args>
	Pair(T1&& f, Args&&... args) : first(std::forward<T1>(f)), second(std::forward<Args>(args)...)
	{}

	Pair(T1&& f, T2&& s) : first(std::forward<T1>(f)), second(std::forward<T2>(s))
	{}

	// Copy ctor
	Pair(const Pair<T1, T2>& other) : first(other.first), second(other.second)
	{}

	// Move ctor
	Pair(Pair<T1, T2>&& other) : first(std::move(other.first)), second(std::move(other.second))
	{}

	// Copy =
	Pair<T1, T2>& operator=(const Pair<T1, T2>& other)
	{
		first = other.first;
		second = other.second;
		return *this;
	}

	// Move =
	Pair<T1, T2>& operator=(Pair<T1, T2>&& other)
	{
		first = std::move(other.first);
		second = std::move(other.second);
		return *this;
	}


	FORCEINLINE ~Pair() {}
	
	union{
		struct{
			T1 first;
			T2 second;
		};
		struct{
			T1 key;
			T2 value;
		};
	};
	
};

TRE_NS_END