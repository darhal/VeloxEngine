#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Defines/Debug.hpp>

TRE_NS_START

template<typename T, usize S>
class Array
{
public:
	Array();
	~Array();

	const T& At(usize i) const;

	const T& operator[](usize i) const;

	const void Fill(const T& obj) const;
	template<typename... Args>
	const void ConstructFill(Args&&... args);

	const void PutAt(const T& obj) const;
	template<typename... Args>
	const void ConstructAt(Args&&... args);
private:
	CONSTEXPR static usize CAPACITY = S;

	T m_Data[S];
	usize m_Length;
};

template<typename T, usize S>
template<typename ...Args>
FORCEINLINE const void Array<T, S>::ConstructFill(Args && ...args)
{
	//TODO
}

template<typename T, usize S>
template<typename ...Args>
FORCEINLINE const void Array<T, S>::ConstructAt(Args && ...args)
{
	//TODO
}

TRE_NS_END


