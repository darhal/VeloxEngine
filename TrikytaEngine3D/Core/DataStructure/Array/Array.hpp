#pragma once

#include <initializer_list>
#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Defines/Debug.hpp>

TRE_NS_START

template<typename T, usize S>
class Array
{
public:
	typedef T* Iterator;
	typedef T& RefIterator;
	typedef const T* CIterator;
public:
	Array();
	Array(const std::initializer_list<T>& list);
	~Array();

	FORCEINLINE T& At(usize i);

	FORCEINLINE T& operator[](usize i);

	FORCEINLINE void Fill(const T& obj);
	template<typename... Args>
	FORCEINLINE void ConstructFill(Args&&... args);

	FORCEINLINE void PutAt(usize i, const T& obj);
	template<typename... Args>
	FORCEINLINE void ConstructAt(usize i, Args&&... args);

	FORCEINLINE void PushBack(const T& obj);

	template<typename... Args>
	FORCEINLINE void Emplaceback(Args&&... args);

	FORCEINLINE bool PopBack();

	FORCEINLINE void Clear();

	FORCEINLINE void Length() const;
	FORCEINLINE void Capacity() const;
	FORCEINLINE bool IsEmpty() const;

	FORCEINLINE Iterator begin() noexcept;
	FORCEINLINE Iterator end() noexcept;
private:
	CONSTEXPR static usize CAPACITY = S;

	T m_Data[S];
	usize m_Length;
};

TRE_NS_END

TRE_NS_START

template<typename T, usize S>
Array<T, S>::Array() : m_Length(0)
{
}

template<typename T, usize S>
Array<T, S>::Array(const std::initializer_list<T>& list)
{
	for (const T& e : list) {
		m_Data[m_Length++] = e;
	}
}

template<typename T, usize S>
Array<T, S>::~Array()
{
}

template<typename T, usize S>
template<typename ...Args>
FORCEINLINE void Array<T, S>::ConstructFill(Args && ...args)
{
	for (usize i = 0; i < CAPACITY; i++) {
		m_Data[i] = T(std::forward<Args>(args)...);
	}
	m_Length = CAPACITY;
}

template<typename T, usize S>
template<typename ...Args>
FORCEINLINE void Array<T, S>::ConstructAt(usize i, Args&& ...args)
{
	ASSERTF(!(i >= CAPACITY || i >= m_Length), "Usage of PutAt with bad parameter index out of bound.");
	m_Data[i] = T(std::forward<Args>(args)...);
}

template<typename T, usize S>
template<typename ...Args>
inline void Array<T, S>::Emplaceback(Args && ...args)
{
	ASSERTF(!(m_Length > CAPACITY), "Usage of PutAt with bad parameter index out of bound.");
	if (m_Length > CAPACITY) return;
	m_Data[m_Length++] = T(std::forward<Args>(args)...);
}

template<typename T, usize S>
FORCEINLINE T& Array<T, S>::At(usize i)
{
	ASSERTF(!(i >= CAPACITY || i >= m_Length), "Usage of PutAt with bad parameter index out of bound.");
	return m_Data[i];
}

template<typename T, usize S>
FORCEINLINE T& Array<T, S>::operator[](usize i)
{
	return this->At(i);
}

template<typename T, usize S>
FORCEINLINE void Array<T, S>::Fill(const T& obj)
{
	for (usize i = 0; i < CAPACITY; i++) {
		m_Data[i] = obj;
	}
	m_Length = CAPACITY;
}

template<typename T, usize S>
FORCEINLINE void Array<T, S>::PutAt(usize i, const T& obj)
{
	ASSERTF(!(i >= CAPACITY), "Usage of PutAt with bad parameter index out of bound.");
	m_Data[i] = obj;
}

template<typename T, usize S>
FORCEINLINE void Array<T, S>::PushBack(const T& obj)
{
	ASSERTF(!(m_Length > CAPACITY), "Usage of PutAt with bad parameter index out of bound.");
	if (m_Length > CAPACITY) return;
	m_Data[m_Length++] = obj;
}

template<typename T, usize S>
FORCEINLINE bool Array<T, S>::PopBack()
{
	if (m_Length > 0) return false;
	m_Length--;
	return true;
}

template<typename T, usize S>
FORCEINLINE void Array<T, S>::Clear()
{
	m_Length = 0;
}

template<typename T, usize S>
FORCEINLINE void Array<T, S>::Length() const
{
	return m_Length;
}

template<typename T, usize S>
FORCEINLINE void Array<T, S>::Capacity() const
{
	return CAPACITY;
}

template<typename T, usize S>
FORCEINLINE bool Array<T, S>::IsEmpty() const
{
	return Length() == 0;
}

template<typename T, usize S>
typename Array<T, S>::Iterator Array<T, S>::begin() noexcept
{
	return m_Data;
}

template<typename T, usize S>
typename Array<T, S>::Iterator Array<T, S>::end() noexcept
{
	return m_Data + m_Length;
}


TRE_NS_END