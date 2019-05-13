#include "Array.hpp"

TRE_NS_START

template<typename T, usize S>
Array<T, S>::Array() : m_Length(0)
{
}

template<typename T, usize S>
Array<T, S>::~Array()
{
}

template<typename T, usize S>
const T& Array<T, S>::At(usize i) const
{
	return m_Data[i];
}

template<typename T, usize S>
const T & Array<T, S>::operator[](usize i) const
{
	return this->At(i);
}

template<typename T, usize S>
const void Array<T, S>::Fill(const T& obj) const
{
	//TODO
}

template<typename T, usize S>
const void Array<T, S>::PutAt(const T & obj) const
{
	//TODO
}

TRE_NS_END
