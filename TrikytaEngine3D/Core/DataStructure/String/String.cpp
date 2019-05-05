#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Defines/Debug.hpp>
#include "String.hpp"

TRE_NS_START

template<typename T>
BasicString<T>::BasicString() : m_Capacity(1), m_Length(0)
{
	m_Buffer = (T*) operator new (sizeof(T) * 1); // allocate empty storage
	*m_Buffer = T(0); // init to 0 
}

template<typename T>
BasicString<T>::~BasicString()
{
	if (m_Capacity == 1) {
		delete m_Buffer;
	}
	delete[] m_Buffer;
}

template<typename T>
FORCEINLINE const T* BasicString<T>::Buffer() const
{
	return m_Buffer;
}

template<typename T>
FORCEINLINE const usize BasicString<T>::Length() const
{
	return m_Length;
}

template<typename T>
FORCEINLINE const usize BasicString<T>::Capacity() const
{
	return m_Capacity;
}

template<typename T>
FORCEINLINE bool BasicString<T>::Empty()
{
	return m_Length == 0;
}

template<typename T>
FORCEINLINE void BasicString<T>::Reserve(usize s)
{
	if (s <= m_Capacity) return;
	T* temp_buffer = (T*) operator new (sizeof(T)*s);
	for (usize i = 0; i < m_Length+1; i++) {
		temp_buffer[i] = m_Buffer[i];
	}
	delete[] m_Buffer;
	m_Buffer = temp_buffer;
	m_Capacity = s;
}

template<typename T>
FORCEINLINE void BasicString<T>::Resize(usize s)
{
	if (s <= m_Length) {
		ASSERTF(!(m_Length - s < 0), "String::Resize function is used with bad parameter.");
		m_Length -= s;
		m_Buffer[m_Length] = T(0);
	}else if (s > m_Capacity){
		T* temp_buffer = (T*) operator new (sizeof(T)*s);
		for (usize i = 0; i < m_Length+1; i++) {
			temp_buffer[i] = m_Buffer[i];
		}
		delete[] m_Buffer;
		m_Buffer = temp_buffer;
		m_Capacity = s;
	}
}

template<typename T>
FORCEINLINE void BasicString<T>::Clear()
{
	m_Buffer[0] = T(0);
	m_Length = 0;
}

template<typename T>
void BasicString<T>::Append(const BasicString<T>& str)
{
	if (str.m_Length + m_Length > m_Capacity) {
		usize new_size = this->m_Capacity + str.m_Capacity;
		T* temp_buffer = (T*) operator new (sizeof(T)*new_size);
		for (usize i = 0; i < m_Length + 1; i++) {
			temp_buffer[i] = m_Buffer[i];
		}
		delete[] m_Buffer;
		m_Buffer = temp_buffer;
		m_Capacity = new_size;
	}
	for (usize i = 0; i < str.m_Length + 1; i++) {
		m_Buffer[m_Length] = str.m_Buffer[i];
		m_Length++;
	}
	m_Length--;
}

template<typename T>
FORCEINLINE T BasicString<T>::At(usize i) const
{
	ASSERTF(!(i > m_Length), "Bad usage of String::At (index out of bound).");
	return m_Buffer[i];
}

template<typename T>
FORCEINLINE T BasicString<T>::Back() const
{
	return m_Buffer[m_Length];
}

template<typename T>
FORCEINLINE T BasicString<T>::Front() const
{
	return m_Buffer[0];
}

template<typename T>
void BasicString<T>::PushBack(T c)
{
	this->Reserve(this->m_Length + 2);
	m_Buffer[m_Length] = c;
	m_Buffer[m_Length] = T(0);
}


template<typename T>
void BasicString<T>::Erase(usize pos, usize offset)
{
	if (pos >= m_Length) return;
	if (offset == 0) return;
	usize end = pos + offset;
	if (pos + offset > m_Length) {
		end = m_Length;
	}
	for (usize i = pos, j = end; i < m_Length && j < m_Length; i++, j++) {
		m_Buffer[i] = m_Buffer[j];
	}
	m_Length -= offset;
	m_Buffer[m_Length] = T(0);
}

template<typename T>
void BasicString<T>::Insert(usize pos, const BasicString<T>& str)
{
	this->Resize(this->m_Capacity + str.m_Capacity);
	usize old_len = m_Length + 1;
	m_Length += str.m_Length;
	//Shift the last part to the end
	BasicString<T> temp_str;
	temp_str.Copy(*this, pos, old_len);
	for (usize i = pos, j = 0; j < str.m_Length; i++, j++) {
		m_Buffer[i] = str.m_Buffer[j];
	}
	for (usize i = pos + str.m_Length, j = 0; j < temp_str.m_Length; i++, j++) {
		m_Buffer[i] = temp_str.m_Buffer[j];
	}
}

template<typename T>
void BasicString<T>::Copy(const BasicString<T>& str, usize pos, usize offset)
{
	ASSERTF(!(offset > str.m_Length), "Bad usage of String::Copy() pos or offset is out of bound.");
	this->Reserve(offset);
	m_Length = offset;
	for (usize i = pos, j = 0; i <= offset; i++, j++) {
		m_Buffer[j] = str.m_Buffer[i];
	}
}

template class BasicString<char>;
template class BasicString<int8>;
template class BasicString<int16>;
template class BasicString<int32>;

TRE_NS_END