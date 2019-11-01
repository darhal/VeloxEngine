#pragma once

#include <initializer_list>
#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Defines/Debug.hpp>
#include <Core/Memory/Utils/Utils.hpp>
#include <type_traits>

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
	template<typename... Args>
	Array(Args&&... args);

	Array(const Array<T, S>& other);
	Array<T, S>& operator=(const Array<T, S>& other);
	//Array(const std::initializer_list<T>& list);
	~Array();

	FORCEINLINE T& At(usize i);
	FORCEINLINE T& operator[](usize i);

	FORCEINLINE const T& At(usize i) const;
	FORCEINLINE const T& operator[](usize i) const;


	FORCEINLINE void Fill(const T& obj);
	template<typename... Args>
	FORCEINLINE void ConstructFill(Args&&... args);

	FORCEINLINE void PutAt(usize i, const T& obj);
	template<typename... Args>
	FORCEINLINE void ConstructAt(usize i, Args&&... args);

	FORCEINLINE void PushBack(const T& obj);

	template<typename... Args>
	FORCEINLINE void EmplaceBack(Args&&... args);

	FORCEINLINE bool PopBack();
	FORCEINLINE void Clear();

	FORCEINLINE usize Length() const;
	FORCEINLINE usize Size() const;
	FORCEINLINE usize Capacity() const;
	FORCEINLINE bool IsEmpty() const;

	FORCEINLINE Iterator begin() noexcept;
	FORCEINLINE Iterator end() noexcept;
private:
	CONSTEXPR static usize CAPACITY = S;

	T m_Data[S];
	usize m_Length;
};

namespace Details {
	template<typename T, usize S>
	static typename std::enable_if<!std::is_fundamental<T>::value>::type DestroyAll(const Array<T, S>& array) {
		for (usize i = 0; i < array.Length(); i++) {
			array[i].~T(); // call dtor on all elements
		}
	}

	template<typename T, usize S>
	static typename std::enable_if<std::is_fundamental<T>::value>::type DestroyAll(const Array<T, S>& array) {} //Do nothing

	template<typename T>
	static typename std::enable_if<!std::is_fundamental<T>::value>::type Destroy(T& obj) {
		obj.~T();
	}

	template<typename T>
	static typename std::enable_if<std::is_fundamental<T>::value>::type Destroy(T& obj) {} //Do nothing
}

TRE_NS_END

TRE_NS_START

template<typename T, usize S>
Array<T, S>::Array() : m_Length(0)
{
}

/*template<typename T, usize S>
Array<T, S>::Array(const std::initializer_list<T>& list) : m_Length(list.size())
{
	//m_Data = list;
}*/

template<typename T, usize S>
template<typename... Args>
Array<T, S>::Array(Args&&... args) : m_Length(sizeof...(Args)), m_Data{ static_cast<T>(std::forward<Args>(args))... }
{
}

template<typename T, usize S>
Array<T, S>::Array(const Array<T, S>& other) : m_Length(other.m_Length)
{
	CopyRangeTo(other.m_Data, m_Data, m_Length);
}


template<typename T, usize S>
Array<T, S>& Array<T, S>::operator=(const Array<T, S>& other)
{
	m_Length = other.m_Length;
	CopyRangeTo(other.m_Data, m_Data, m_Length);
}

template<typename T, usize S>
Array<T, S>::~Array()
{
}

template<typename T, usize S>
template<typename... Args>
FORCEINLINE void Array<T, S>::ConstructFill(Args&& ...args)
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
	ASSERTF((i >= CAPACITY || i >= m_Length), "Usage of PutAt with bad parameter index out of bound.");
	m_Data[i] = T(std::forward<Args>(args)...);
}

template<typename T, usize S>
template<typename ...Args>
FORCEINLINE void Array<T, S>::EmplaceBack(Args&& ...args)
{
	ASSERTF((m_Length > CAPACITY), "Usage of PutAt with bad parameter index out of bound.");
	if (m_Length > CAPACITY) return;
	m_Data[m_Length++] = T(std::forward<Args>(args)...);
}


template<typename T, usize S>
FORCEINLINE bool Array<T, S>::PopBack() {
	if (m_Length <= 0) return false;
	m_Length--;
	Details::Destroy(m_Data[m_Length]); // Call dtor
	return true;
}

template<typename T, usize S>
FORCEINLINE void Array<T, S>::Clear() {
	if (m_Length == 0) return;
	Details::DestroyAll(*this);
	m_Length = 0;
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
	ASSERTF((i >= CAPACITY), "Usage of PutAt with bad parameter index out of bound.");
	m_Data[i] = obj;
}

template<typename T, usize S>
FORCEINLINE void Array<T, S>::PushBack(const T& obj)
{
	ASSERTF((m_Length > CAPACITY), "Usage of PutAt with bad parameter index out of bound.");
	if (m_Length > CAPACITY) return;
	m_Data[m_Length++] = obj;
}

template<typename T, usize S>
FORCEINLINE usize Array<T, S>::Length() const
{
	return m_Length;
}

template<typename T, usize S>
FORCEINLINE usize Array<T, S>::Size() const
{
	return this->Length();
}

template<typename T, usize S>
FORCEINLINE usize Array<T, S>::Capacity() const
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

template<typename T, usize S>
FORCEINLINE T& Array<T, S>::At(usize i)
{
	ASSERTF((i >= CAPACITY || i >= m_Length), "Usage of PutAt with bad parameter index out of bound.");
	return m_Data[i];
}

template<typename T, usize S>
FORCEINLINE T& Array<T, S>::operator[](usize i)
{
	return this->At(i);
}

template<typename T, usize S>
FORCEINLINE const T& Array<T, S>::At(usize i) const
{
	ASSERTF((i >= CAPACITY || i >= m_Length), "Usage of PutAt with bad parameter index out of bound.");
	return m_Data[i];
}

template<typename T, usize S>
FORCEINLINE const T& Array<T, S>::operator[](usize i) const
{
	return this->At(i);
}

TRE_NS_END