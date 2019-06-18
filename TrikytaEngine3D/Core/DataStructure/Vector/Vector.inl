#include "Vector.hpp"

template<typename T>
FORCEINLINE Vector<T>::Vector() : m_Length(0), m_Capacity(DEFAULT_CAPACITY), m_Data(NULL)
{
	//m_Data = Allocate<T>(m_Capacity);
}

template<typename T>
FORCEINLINE Vector<T>::Vector(usize sz) : m_Length(0), m_Capacity(sz), m_Data(NULL)
{
	//m_Data = Allocate<T>(m_Capacity);
}

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
	if (m_Data != NULL) {
		Deallocate<T>(m_Data, m_Length);
		m_Data = NULL;
	}
}

template<typename T>
template<typename ...Args>
FORCEINLINE T& Vector<T>::EmplaceBack(Args&&... args)
{
	Reserve(m_Length + 1);
	new (m_Data + m_Length) T(std::forward<Args>(args)...);
	return *(m_Data + (m_Length++));
}

template<typename T>
FORCEINLINE T& Vector<T>::PushBack(const T& obj)
{
	Reserve(m_Length + 1);
	new (m_Data + m_Length) T(obj);
	m_Length++;
	return *(m_Data + (m_Length++));
}

template<typename T>
FORCEINLINE bool Vector<T>::PopBack()
{
	if (m_Length <= 0) return false;
	m_Data[m_Length - 1].~T();
	return m_Length--;
}

template<typename T>
FORCEINLINE bool Vector<T>::PopFront()
{
	if (m_Length < 0) return false;
	m_Data[0].~T();
	MoveRange(m_Data, m_Data, m_Length - 1);
	return m_Length--;
}

template<typename T>
FORCEINLINE bool Vector<T>::Reserve(usize sz)
{
	if (m_Data == NULL)	{ m_Data = Allocate<T>(m_Capacity); }
	if (sz <= m_Capacity) return true;
	Reallocate(sz * DEFAULT_GROW_SIZE);
	return false;
}

template<typename T>
FORCEINLINE T& Vector<T>::Insert(usize i, const T& obj)
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
		return *(dest);
	}else{
		T* dest = m_Data + index;
		MoveRange(dest, m_Data + i, m_Length - i + 1); // shift all of this to keep place for the new element
		new (dest) T(obj);
		m_Length++;
		return *(dest);
	}
}

template<typename T>
FORCEINLINE T& Vector<T>::PushFront(const T& obj)
{
	this->Insert(0, obj);
}

template<typename T>
template<typename... Args>
FORCEINLINE T& Vector<T>::Emplace(usize i, Args&&... args)
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
		return *(dest);
	}else{
		T* dest = m_Data + index;
		MoveRange(dest, m_Data + i, m_Length - i + 1); // shift all of this to keep place for the new element
		new (dest) T(std::forward<Args>(args)...);
		m_Length++;
		return *(dest);
	}
}

template<typename T>
template<typename ...Args>
FORCEINLINE T& Vector<T>::EmplaceFront(Args&&... args)
{
	return this->Emplace(0, std::forward<Args>(args)...);
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
	} while (start_ptr <= end_ptr);
	MoveRange(m_Data + end + 1, m_Data + start, m_Length - end);
	m_Length -= size + 1;
}

template<typename T>
FORCEINLINE void Vector<T>::Erease(Iterator itr)
{
	ASSERTF(!(itr >= m_Data + m_Length || itr < m_Data), "The given iterator doesn't belong to the Vector.");
	(*itr).~T();
	usize start = usize(itr - m_Data);
	usize end = usize(m_Data + m_Length - itr);
	MoveRange(m_Data + end + 1, m_Data + start, m_Length - end);
	m_Length -= 1;
}

template<typename T>
FORCEINLINE void Vector<T>::Clear()
{
	DestroyObjects(m_Data, m_Length);
	m_Length = 0;
}

template<typename T>
FORCEINLINE bool Vector<T>::IsEmpty() const
{
	return this->Size() == 0;
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
	return (m_Length == 0) ? 0 : m_Length - 1;
}

template<typename T>
FORCEINLINE T* Vector<T>::Back() const
{
	return &m_Data[Size()];
}

template<typename T>
FORCEINLINE T* Vector<T>::Front() const
{
	return &m_Data[0];
}

template<typename T>
FORCEINLINE const T* Vector<T>::At(usize i)
{
	ASSERTF(!(i >= m_Length), "Bad usage of vector function At index out of bounds");
	return &m_Data[i];
}

template<typename T>
FORCEINLINE const T& Vector<T>::At(usize i) const
{
	ASSERTF(!(i >= m_Length), "Bad usage of vector function At index out of bounds");
	return m_Data[i];
}

template<typename T>
FORCEINLINE const T* Vector<T>::operator[](usize i)
{
	if (i >= m_Length) return NULL;
	return At(i);
}

template<typename T>
FORCEINLINE const T& Vector<T>::operator[](usize i) const
{
	return At(i);
}

template<typename T>
FORCEINLINE typename Vector<T>::Iterator Vector<T>::begin() noexcept
{
	return m_Data;
}

template<typename T>
FORCEINLINE typename Vector<T>::Iterator Vector<T>::end() noexcept
{
	return m_Data + m_Length;
}


template<typename T>
FORCEINLINE Vector<T>::Vector(const Vector<T>& other) : m_Length(other.m_Length), m_Capacity(other.m_Capacity)
{
	m_Data = Allocate<T>(other.m_Length);
	CopyRangeTo(m_Data, other.m_Data, other.m_Length);
}

template<typename T>
FORCEINLINE Vector<T>& Vector<T>::operator=(const Vector<T>& other)
{
	m_Length = other.m_Length;
	m_Capacity = other.m_Capacity;
	m_Data = Allocate<T>(other.m_Length);
	CopyRangeTo(m_Data, other.m_Data, other.m_Length);
	return *this;
}

template<typename T>
FORCEINLINE Vector<T>::Vector(Vector<T>&& other) : m_Length(other.m_Length), m_Capacity(other.m_Capacity), m_Data(other.m_Data)
{
	other.m_Data = NULL;
}

template<typename T>
FORCEINLINE Vector<T>& Vector<T>::operator=(Vector<T>&& other)
{
	m_Length = other.m_Length;
	m_Capacity = other.m_Capacity;
	m_Data = other.m_Data;
	other.m_Data = NULL;
}