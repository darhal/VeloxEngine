#include <Core/Misc/Defines/Common.hpp>
/*
TRE_NS_START

template<typename T>
class Vector;

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
	Deallocate(m_Data, m_Length);
}

template<typename T>
template<typename ...Args>
FORCEINLINE void Vector<T>::EmplaceBack(Args&&... args)
{
	Reserve(m_Length + 1);
	new (m_Data + m_Length) T(std::forward<Args>(args))...);
	m_Length++;
}

template<typename T>
inline void Vector<T>::PushBack(const T& obj)
{
	Reserve(m_Length + 1);
	new (m_Data + m_Length) T(obj);
	m_Length++;
}

template<typename T>
FORCEINLINE void Vector<T>::Reserve(usize sz)
{
	if (sz <= m_Capacity) return;
	Reallocate(sz);
}

template<typename T>
void Vector<T>::Reallocate(usize nCap) // I think this can be optimized! to just copy and dont delete the thing or use move ctor.
{
	T* newData = Allocate(nCap);
	CopyRangeTo(m_Data, m_Data + m_Length, newData);
	DeleteRange(m_Data, m_Data + m_Length);
	Free(m_Data);
	m_Data = newData;
	m_Capacity = nCap;
}

template<typename T>
FORCEINLINE usize Vector<T>::Capacity() const
{
	return m_Capacity;
}

template<typename T>
FORCEINLINE usize Vector<T>::Size() const
{
	return m_Length;
}

TRE_NS_END*/