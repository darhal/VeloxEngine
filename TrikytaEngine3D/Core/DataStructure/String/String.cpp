#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Defines/Debug.hpp>
#include "String.hpp"
#include <cstdio>

#include <iostream>
#include <bitset>

TRE_NS_START

template<typename T>
BasicString<T>::BasicString()
{
	m_Data[0] = T(0);
	SetSmallLength(1);
	printf("Default ctor\n");
}

template<typename T>
BasicString<T>::BasicString(usize capacity)
{
	if (capacity <= SSO_SIZE) {
		m_Data[0] = T(0);
		SetSmallLength(1);
	}else {
		m_Buffer    = (T*) operator new (sizeof(T) * capacity); // allocate empty storage
		m_Buffer[0] = T(0); // init to 0
		m_Capacity = capacity;
		SetNormalLength(1);
	}
}

//Copy ctor copy assignement
template<typename T>
BasicString<T>::BasicString(const BasicString<T>& other)
{
	bool IsOtherSmall = other.IsSmall();
	usize len = other.Length();
	if (IsOtherSmall) {
		for (usize i = 0; i < len; i++) {
			m_Data[i] = other.m_Data[i];
		}
		SetSmallLength(len);
	}else{
		usize cap = other.m_Capacity;
		m_Buffer = (T*) operator new (sizeof(T) * cap); // allocate empty storage
		for (usize i = 0; i < len; i++) {
			m_Buffer[i] = other.m_Buffer[i];
		}
		m_Capacity = cap;
		SetNormalLength(len);
	}
	printf("Copy ctor of %s (NORMAL)\n", Buffer());
}

template<typename T>
BasicString<T>& BasicString<T>::operator=(const BasicString<T>& other)
{
	bool IsOtherSmall = other.IsSmall();
	usize cap = other.Capacity();
	usize len = other.Length();
	if (IsOtherSmall && this->Capacity() <= cap) {
		for (usize i = 0; i < len; i++) {
			m_Data[i] = other.m_Data[i];
		}
		SetSmallLength(len);
	}else{
		if (this->IsSmall()) {
			m_Buffer = (T*) operator new (sizeof(T) * cap); // allocate empty storage
		}
		const T* other_buffer = other.Buffer();
		for (usize i = 0; i < len; i++) {
			m_Buffer[i] = other_buffer[i];
		}
		SetLength(len);
	}
	printf("Copy = of %s\n", other.Buffer());
	return *this;
}

//Move ctor move assignement
template<typename T>
BasicString<T>::BasicString(BasicString<T>&& other)
{
	bool IsOtherSmall = other.IsSmall();
	usize len = other.Length();
	if (IsOtherSmall) {
		for (usize i = 0; i < len; i++) {
			m_Data[i] = other.m_Data[i];
		}
		SetSmallLength(len);
		
	}else{
		m_Buffer = other.m_Buffer;
		m_Capacity = other.m_Capacity;
		SetNormalLength(len);
		other.m_Buffer = NULL; // Pervent the other string from deleting when it go out of scope
	}
	printf("Using mov ctor of small string\n");
}

template<typename T>
BasicString<T>& BasicString<T>::operator=(BasicString<T>&& other)
{
	bool IsOtherSmall = other.IsSmall();
	usize len = other.Length();
	if (!this->IsSmall() && m_Buffer != NULL) {
		delete[] m_Buffer;
	}
	if (IsOtherSmall) {
		for (usize i = 0; i < len; i++) {
			m_Data[i] = other.m_Data[i];
		}
		SetSmallLength(len);
	}else {
		m_Buffer = other.m_Buffer;
		m_Capacity = other.m_Capacity;
		SetNormalLength(len);
		other.m_Buffer = NULL; // Pervent the other string from deleting when it go out of scope
	}
	printf("Move = of %s\n", other.Buffer());
	return *this;
}

template<typename T>
FORCEINLINE BasicString<T>& BasicString<T>::operator+=(const BasicString<T>& other)
{
	this->Append(other);
	return *this;
}

template<typename T>
BasicString<T>::~BasicString()
{
	if (!this->IsSmall()) {
		delete[] m_Buffer;
	}
}

template<typename T>
FORCEINLINE const T* BasicString<T>::Buffer() const
{
	return this->IsSmall() ? m_Data : m_Buffer;
}

template<typename T>
FORCEINLINE const usize BasicString<T>::Length() const
{
	return this->IsSmall() ? SSO_SIZE - (T)(m_Data[SSO_MI] >> 1) : m_Length >> 1;
}

template<typename T>
FORCEINLINE const usize BasicString<T>::Size() const
{
	return this->Length() - 1;
}

template<typename T>
FORCEINLINE const usize BasicString<T>::Capacity() const
{
	return this->IsSmall() ? SSO_MI : m_Capacity;
}

template<typename T>
FORCEINLINE bool BasicString<T>::Empty()
{
	return this->Length() == 0;
}

template<typename T>
FORCEINLINE void BasicString<T>::Reserve(usize s)
{
	if (s < SSO_SIZE) return; // force using the local buffer!
	if (s <= this->Capacity()) return;
	const T* buffer_ptr = this->Buffer();
	T* temp_buffer = (T*) operator new (sizeof(T)*s);
	usize len = this->Length();
	for (usize i = 0; i < len; i++) {
		temp_buffer[i] = buffer_ptr[i];
	}
	if (!this->IsSmall() && m_Buffer != NULL) {
		delete[] m_Buffer;
	}else{
		SetNormalLength(len);
	}
	m_Buffer = temp_buffer;
	this->SetCapacity(s); // we can use this because its no longer small string at this point.
}

template<typename T>
FORCEINLINE void BasicString<T>::Resize(usize s)
{
	usize len = this->Length();
	bool isSmall = this->IsSmall();
	if (s <= len) {
		ASSERTF(!(len - s < 0), "String::Resize function is used with bad parameter.");
		if (isSmall) {
			usize nlen = (len - s); // new len remmber that len does represent whats rest in the buffer (when SSO is enabled)
			m_Data[SSO_MI] = T((SSO_SIZE - (nlen)) << 1);
			m_Data[nlen] = T(0);
		}else {
			m_Length -= s;
			m_Buffer[m_Length] = T(0);
		}
	}else if (s > this->Capacity()){
		if (isSmall) { // String was small and now it gonna convert to normal.
			T* temp_buffer = (T*) operator new (sizeof(T)*s);
			for (usize i = 0; i < len; i++) {
				temp_buffer[i] = m_Data[i];
			}
			m_Buffer = temp_buffer;
			m_Capacity = s;
			m_Length = len << 1 | 0x01;
		}else{
			T* temp_buffer = (T*) operator new (sizeof(T)*s);
			for (usize i = 0; i < len; i++) {
				temp_buffer[i] = m_Buffer[i];
			}
			delete[] m_Buffer;
			m_Buffer = temp_buffer;
			m_Capacity = s;
		}
	}
}

template<typename T>
FORCEINLINE void BasicString<T>::Clear()
{
	bool isSmall = this->IsSmall();
	if (isSmall) {
		m_Data[0] = T(0);
		m_Data[SSO_MI] = (SSO_SIZE) << 1;
	}else{
		m_Buffer[0] = T(0);
		m_Length = 0x01;
	}
	
}

template<typename T>
void BasicString<T>::Append(const BasicString<T>& str)
{
	usize tlen        = this->Length() - 1; // -1 is to remove the trailing null terminator
	usize slen	      = str.Length();
	usize finalLen	  = tlen + slen;
	const T* str_buffer_ptr = str.Buffer();
	if (finalLen - 1 > this->Capacity()) { // -1 is to remove the trailing null terminator
		this->Reserve(finalLen*SPARE_RATE);
		T* buffer_ptr = this->EditableBuffer();
		for (usize i = 0; i < slen; i++) {
			buffer_ptr[tlen++] = str_buffer_ptr[i];
		}
		this->SetNormalLength(tlen);
	}else{
		T* buffer_ptr = this->EditableBuffer();
		for (usize i = 0; i < slen; i++) {
			buffer_ptr[tlen++] = str_buffer_ptr[i];
		}
		this->SetLength(tlen);
	}
}

template<typename T>
FORCEINLINE T BasicString<T>::At(usize i) const
{
	ASSERTF(!(i > m_Length), "Bad usage of String::At (index out of bound).");
	return this->Buffer()[i];
}

template<typename T>
FORCEINLINE T BasicString<T>::Back() const
{
	return this->Buffer()[this->Length()-1];
}

template<typename T>
FORCEINLINE T BasicString<T>::Front() const
{
	return this->Buffer()[0];
}

template<typename T>
void BasicString<T>::PushBack(T c)
{
	usize len = this->Length();
	this->Reserve((len + 1*SPARE_RATE));
	T* buffer_ptr = this->EditableBuffer();
	buffer_ptr[len++] = c;
	buffer_ptr[len] = T(0);
	this->SetLength(len);
}

template<typename T>
void BasicString<T>::PopBack()
{
	usize len = this->Length();
	if (len <= 1) return;
	T* buffer_ptr = this->EditableBuffer();
	buffer_ptr[len - 1] = T(0);
	this->SetLength(len - 1);
}

template<typename T>
void BasicString<T>::Erase(usize pos, usize offset)
{
	usize len = this->Length();
	if (pos >= len) return;
	if (offset == 0) return;
	usize end = pos + offset;
	T* buffer_ptr = this->EditableBuffer();
	if (pos + offset >= len) {
		end = len;
		buffer_ptr[pos] = T(0);
		this->SetLength(len - offset);
		return;
	}
	for (usize i = pos, j = end; i < len && j < len; i++, j++) {
		buffer_ptr[i] = buffer_ptr[j];
	}
	this->SetLength(len - offset);
}

template<typename T>
void BasicString<T>::Insert(usize pos, const BasicString<T>& str)
{
	usize tlen = this->Length() - 1; // remove the trailing null
	usize slen = str.Length();
	this->Resize(tlen + slen - 1);
	//Shift the last part to the end
	BasicString<T> temp_str(tlen + 1);
	temp_str.Copy(*this, pos, tlen + 1);
	usize temp_len = temp_str.Length();
	T* this_buffer = this->EditableBuffer();
	const T* temp_buffer = temp_str.Buffer();
	const T* str_buffer = str.Buffer();
	for (usize i = pos, j = 0; j < slen; i++, j++) {
		this_buffer[i] = str_buffer[j];
	}
	for (usize i = pos + slen, j = 0; j < temp_len; i++, j++) {
		this_buffer[i] = temp_buffer[j];
	}
	this->SetLength(tlen + slen);
}

template<typename T>
void BasicString<T>::Copy(const BasicString<T>& str, usize pos, usize offset)
{
	usize slen = str.Length();
	ASSERTF(!(offset > slen), "Bad usage of String::Copy() pos or offset is out of bound.");
	const T* buffer_ptr = str.Buffer();
	if (offset < SSO_SIZE) {
		SetSmallLength(offset);
		for (usize i = pos, j = 0; i <= offset; i++, j++) {
			m_Data[j] = buffer_ptr[i];
		}
	}else{
		this->Reserve(offset);
		SetNormalLength(offset);
		for (usize i = pos, j = 0; i <= offset; i++, j++) {
			m_Buffer[j] = buffer_ptr[i];
		}
	}
}

template<typename T>
FORCEINLINE BasicString<T> BasicString<T>::SubString(usize pos, usize off) const
{
	BasicString<T> temp_str(off);
	temp_str.Copy(*this, pos, off);
	printf("SubStr = %s", temp_str.Buffer());
	return temp_str;
}

template<typename T>
FORCEINLINE ssize BasicString<T>::Find(const BasicString<T>& pattren) const
{
	return SearchBoyerMoore(*this, pattren);
}


/********** PRIVATE FUNCTIONS **********/

template<typename T>
FORCEINLINE bool BasicString<T>::IsSmall() const
{  
	return !(char)((m_Length & 0xFF) << (sizeof(T)*BITS_PER_BYTE - 1)); // if the bit is 0 then its small string otherwise its normal
}

template<typename T>
FORCEINLINE void BasicString<T>::SetLength(usize nlen)
{
	if (this->IsSmall() && nlen <= SSO_SIZE) {
		m_Data[SSO_MI] = T((SSO_SIZE - nlen) << 1);
	}else{
		m_Length = nlen << 1 | 0x01;
	}
}

template<typename T>
FORCEINLINE void BasicString<T>::SetSmallLength(usize nlen)
{
	m_Data[SSO_MI] = T((SSO_SIZE - nlen) << 1);
}

template<typename T>
FORCEINLINE void BasicString<T>::SetNormalLength(usize nlen)
{
	m_Length = nlen << 1 | 0x01;
}

template<typename T>
FORCEINLINE void BasicString<T>::SetCapacity(usize ncap)
{
	if (this->IsSmall()) return;
	m_Capacity = ncap;
}

template<typename T>
FORCEINLINE T* BasicString<T>::EditableBuffer()
{
	return this->IsSmall() ? m_Data : m_Buffer;
}

template class BasicString<char>;
template class BasicString<char16_t>;
template class BasicString<wchar_t>;
//template class BasicString<int>; //char32_t

TRE_NS_END