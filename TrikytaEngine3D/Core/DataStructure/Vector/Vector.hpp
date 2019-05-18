#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Defines/Debug.hpp>
#include <Core/Memory/Utils/Utils.hpp>
#include <initializer_list>

TRE_NS_START

template<typename T>
class Vector
{
public:
	FORCEINLINE Vector();
	FORCEINLINE Vector(usize sz);
	Vector(const std::initializer_list<T>& list);
	template<usize S>
	Vector(T(&arr)[S]);

	~Vector();

	template<typename... Args>
	FORCEINLINE const T* EmplaceBack(Args&&... args);
	FORCEINLINE const T* PushBack(const T& obj);
	FORCEINLINE bool PopBack();

	FORCEINLINE const T* Insert(usize i, const T& obj);
	template<typename... Args>
	FORCEINLINE const T* Emplace(usize i, Args&&... args);

	FORCEINLINE void Erease(usize start, usize end);

	FORCEINLINE usize Capacity() const;
	FORCEINLINE usize Length() const;
	FORCEINLINE usize Size() const;
	FORCEINLINE const T& Back() const;
	FORCEINLINE const T& Front() const;

	FORCEINLINE bool Reserve(usize sz);

	FORCEINLINE const T* At(usize i);
	FORCEINLINE const T& At(usize i) const;
	FORCEINLINE const T* operator[](usize i);
	FORCEINLINE const T& operator[](usize i) const;
private:
	FORCEINLINE void Reallocate(usize nCap);
private:
	CONSTEXPR static usize DEFAULT_CAPACITY	 = 8;
	CONSTEXPR static usize DEFAULT_GROW_SIZE = 2;

	T* m_Data;
	usize m_Length;
	usize m_Capacity;
};

template<typename T>
template<usize S>
Vector<T>::Vector(T(&arr)[S]) : m_Length(S), m_Capacity(S)
{
	m_Data = Allocate<T>(m_Capacity);
	T* ptr = m_Data;
	for (const T& obj : arr) {
		new (ptr) T(obj);
		ptr++;
	}
}

template<typename T>
FORCEINLINE Vector<T>::Vector() : m_Length(0), m_Capacity(DEFAULT_CAPACITY)
{
	m_Data = Allocate<T>(DEFAULT_CAPACITY);
}

template<typename T>
FORCEINLINE Vector<T>::Vector(usize sz) : m_Length(0), m_Capacity(sz < 2 ? DEFAULT_CAPACITY : sz)
{
	m_Data = Allocate<T>(m_Capacity);
}

template<typename T>
Vector<T>::Vector(const std::initializer_list<T>& list) : m_Length(list.size()), m_Capacity(list.size())
{
	m_Data = Allocate<T>(m_Capacity);
	T* ptr = m_Data;
	for (const T& obj : list) {
		new (ptr) T(obj);
		ptr++;
	}
}

template<typename T>
Vector<T>::~Vector()
{
	Deallocate<T>(m_Data, m_Length);
}

template<typename T>
template<typename ...Args>
FORCEINLINE const T* Vector<T>::EmplaceBack(Args&&... args)
{
	Reserve(m_Length + 1);
	new (m_Data + m_Length) T(std::forward<Args>(args)...);
	return (m_Data + (m_Length++));
}

template<typename T>
FORCEINLINE const T* Vector<T>::PushBack(const T& obj)
{
	Reserve(m_Length + 1);
	new (m_Data + m_Length) T(obj);
	m_Length++;
	return (m_Data + (m_Length++));
}

template<typename T>
FORCEINLINE bool Vector<T>::PopBack()
{
	if (m_Length <= 0) return false;
	m_Data[m_Length - 1].~T();
	return m_Length--;
}

template<typename T>
FORCEINLINE bool Vector<T>::Reserve(usize sz)
{
	if (sz <= m_Capacity) return true;
	Reallocate(sz * DEFAULT_GROW_SIZE);
	return false;
}

template<typename T>
FORCEINLINE const T* Vector<T>::Insert(usize i, const T& obj)
{
	ASSERTF(!(i == 0 || i > m_Length + 1), "Given index is out of bound please choose from [1..%d].", m_Length + 1);
	usize index = i - 1;
	if (m_Length + 1 > m_Capacity) {
		usize nCap = (m_Length + 1)*DEFAULT_GROW_SIZE;
		T* newData = Allocate<T>(nCap);
		T* dest = newData + index;

		CopyRangeTo(m_Data, newData, index);
		CopyRangeTo(m_Data + index, newData + i, m_Length - i + 1);
		new (dest) T(obj);
		Free(m_Data);
		m_Data = newData;
		m_Capacity = nCap;
		m_Length++;
		return (dest);
	}else{
		T* dest = m_Data + index;
		MoveRange(dest, m_Data + m_Length - 1, m_Length - i);
		new (dest) T(obj);
		m_Length++;
		return (dest);
	}
}

template<typename T>
template<typename... Args>
FORCEINLINE const T* Vector<T>::Emplace(usize i, Args&&... args)
{
	ASSERTF(!(i == 0 || i > m_Length + 1), "Given index is out of bound please choose from [1..%d].", m_Length + 1);
	usize index = i - 1;
	if (m_Length + 1 > m_Capacity) {
		usize nCap = (m_Length + 1)*DEFAULT_GROW_SIZE;
		T* newData = Allocate<T>(nCap);
		T* dest = newData + index;

		CopyRangeTo(m_Data, newData, index);
		CopyRangeTo(m_Data + index, newData + i, m_Length - i + 1);
		new (dest) T(std::forward<Args>(args)...);
		Free(m_Data);
		m_Data = newData;
		m_Capacity = nCap;
		m_Length++;
		return (dest);
	}else{
		T* dest = m_Data + index;
		MoveRange(dest, m_Data + m_Length - 1, m_Length - i);
		new (dest) T(std::forward<Args>(args)...);
		m_Length++;
		return (dest);
	}
}

template<typename T>
FORCEINLINE void Vector<T>::Reallocate(usize nCap) // I think this can be optimized! to just copy and dont delete the thing or use move ctor.
{
	T* newData = Allocate<T>(nCap);
	CopyRangeTo(m_Data, newData, m_Length);
	//DeleteRange(m_Data, m_Data + m_Length);
	Free(m_Data);
	m_Data = newData;
	m_Capacity = nCap;
}

template<typename T>
FORCEINLINE void Vector<T>::Erease(usize start, usize end)
{
	ASSERTF(!(start > m_Length || end > m_Length), "[%d..%d] interval isn't included in the range [0..%d]", start, end, m_Length);
	usize size = Absolute(ssize(end) - ssize(start));
	T* start_ptr = m_Data + start;
	T* end_ptr = m_Data + end;
	do {
		(*start_ptr++).~T();
	} while (start_ptr != end_ptr);
	MoveRange(m_Data + end, m_Data + start, size);
	m_Length -= size;
}

template<typename T>
FORCEINLINE usize Vector<T>::Capacity() const
{
	return m_Capacity;
}

template<typename T>
FORCEINLINE usize Vector<T>::Length() const
{
	return m_Length;
}

template<typename T>
FORCEINLINE usize Vector<T>::Size() const
{
	return m_Length ? 0 : m_Length - 1;
}

template<typename T>
inline const T& Vector<T>::Back() const
{
	return m_Data[Size()];
}

template<typename T>
inline const T& Vector<T>::Front() const
{
	return m_Data[0];
}

template<typename T>
FORCEINLINE const T* Vector<T>::At(usize i)
{
	ASSERTF(!(i >= m_Length), "Bad usage of vector function At index out of bounds");
	return &m_Data[i];
}

template<typename T>
inline const T& Vector<T>::At(usize i) const
{
	ASSERTF(!(i >= m_Length), "Bad usage of vector function At index out of bounds");
	return m_Data[i];
}

template<typename T>
inline const T* Vector<T>::operator[](usize i)
{
	if (i >= m_Length) return NULL;
	return At(i);
}

template<typename T>
inline const T& Vector<T>::operator[](usize i) const
{
	return At(i);
}

TRE_NS_END