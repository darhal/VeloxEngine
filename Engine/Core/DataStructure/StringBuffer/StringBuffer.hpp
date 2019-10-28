#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <cstring>
#include <stdio.h>

// https://dev.w3.org/libwww/Library/src/vms/getline.c

TRE_NS_START

template<typename T>
class StringBuffer
{
public:
	StringBuffer();
	
	explicit StringBuffer(const T* str);

	explicit StringBuffer(uint32 cap);

	~StringBuffer() { delete[] m_Buffer; }

	template<typename U>
	StringBuffer& AppendObject(const U& obj);

	StringBuffer& Append(const T* str, uint32 n = 0);
	
	StringBuffer& Append(T x);

	StringBuffer& Append(int16 x);

	StringBuffer& Append(uint16 x);

	StringBuffer& Append(int32 x);

	StringBuffer& Append(uint32 x);

	StringBuffer& Append(int64 x);

	StringBuffer& Append(uint64 x);

	StringBuffer& Append(float x);

	StringBuffer& Append(double x);

	StringBuffer& Append(const StringBuffer& str);

	FORCEINLINE uint32 Length() const { return m_Length; }

	FORCEINLINE uint32 Capacity() const { return m_Capacity; }

	FORCEINLINE char* ToString() { return m_Buffer; }

	FORCEINLINE T& operator[](uint32 i) { return m_Buffer[i]; }

	FORCEINLINE const T& operator[](uint32 i) const { ASSERTF((i >= m_Length), "The given index point outside the buffer"); return m_Buffer[i]; }

	FORCEINLINE StringBuffer<T>& operator+=(const StringBuffer<T>& str) { return this->Append(str); };

	FORCEINLINE StringBuffer<T>& operator+=(const T* str) { return this->Append(str); };

	/*
	*@param: const T* of the string
	*@return: uint32 the length of str without counting the NULL terminator
	*/
	static uint32 SbLen(const T* str);
private:
	T* m_Buffer;
	uint32 m_Length;
	uint32 m_Capacity;
};

template<typename T>
uint32 StringBuffer<T>::SbLen(const T* str)
{
	uint32 len = 0;

	while (*(str++)) {
		len++;
	}

	return len;
}

template<typename T>
StringBuffer<T>::StringBuffer() : m_Buffer(NULL), m_Length(0), m_Capacity(0)
{
}

template<typename T>
StringBuffer<T>::StringBuffer(uint32 cap) : m_Buffer(new T[cap]), m_Length(0), m_Capacity(cap)
{
	m_Buffer[0] = '\0';
}

template<typename T>
StringBuffer<T>::StringBuffer(const T* str) : m_Length(SbLen(str))
{
	m_Capacity = (3 / 2) * m_Length;
	m_Buffer = new T[m_Capacity];
	memcpy(m_Buffer, str, m_Length * sizeof(T));
	m_Buffer[m_Length] = '\0';
}

template<typename T>
StringBuffer<T>& StringBuffer<T>::Append(const T* str, uint32 n)
{
	uint32 len = n;

	if (!len)
		len = SbLen(str);

	if (m_Capacity - m_Length >= len + 1) {
		memcpy(m_Buffer + m_Length, str, len * sizeof(T));
	}else {
		m_Capacity += uint32((3.f / 2.f) * len);
		
		T* n_buff = new T[m_Capacity];

		if (m_Buffer) {
			memcpy(n_buff, m_Buffer, m_Length * sizeof(T));
			delete[] m_Buffer;
		}

		m_Buffer = n_buff;
		memcpy(m_Buffer + m_Length, str, len * sizeof(T));
	}

	m_Length += len;
	m_Buffer[m_Length] = '\0';
	return *this;
}

template<typename T>
StringBuffer<T>& StringBuffer<T>::Append(const StringBuffer& str)
{
	return this->Append(str.m_Buffer, str.m_Length);
}

template<typename T>
template<typename U>
StringBuffer<T>& StringBuffer<T>::AppendObject(const U& obj)
{
	return this->Append(obj.ToString());
}

template<typename T>
StringBuffer<T>& StringBuffer<T>::Append(T x)
{
	return this->Append(&x, 1);
}

template<typename T>
StringBuffer<T>& StringBuffer<T>::Append(int16 x)
{
	T buff[8];
	sprintf(buff, "%d", x);
	return this->Append(buff);
}

template<typename T>
StringBuffer<T>& StringBuffer<T>::Append(uint16 x)
{
	T buff[8];
	sprintf(buff, "%u", x);
	return this->Append(buff);
}

template<typename T>
StringBuffer<T>& StringBuffer<T>::Append(int32 x)
{
	T buff[16];
	sprintf(buff, "%d", x);
	return this->Append(buff);
}

template<typename T>
StringBuffer<T>& StringBuffer<T>::Append(uint32 x)
{
	T buff[16];
	sprintf(buff, "%u", x);
	return this->Append(buff);
}

template<typename T>
StringBuffer<T>& StringBuffer<T>::Append(int64 x)
{
	T buff[32];
	sprintf(buff, "%lld", x);
	return this->Append(buff);
}

template<typename T>
StringBuffer<T>& StringBuffer<T>::Append(uint64 x)
{
	T buff[32];
	sprintf(buff, "%llu", x);
	return this->Append(buff);
}

template<typename T>
StringBuffer<T>& StringBuffer<T>::Append(float x)
{
	T buff[16];
	sprintf(buff, "%f", x);
	return this->Append(buff);
}

template<typename T>
StringBuffer<T>& StringBuffer<T>::Append(double x)
{
	T buff[32];
	sprintf(buff, "%lf", x);
	return this->Append(buff);
}


TRE_NS_END


