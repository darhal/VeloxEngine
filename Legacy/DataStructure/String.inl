#include "String.hpp"

TRE_NS_START

template<typename T>
BasicString2<T>::BasicString2()
{
	m_Data[0] = T(0);
	SetSmallLength(1);
}

template<typename T>
BasicString2<T>::BasicString2(usize capacity, usize len)
{
	if (capacity <= SSO_SIZE) {
		m_Data[0] = T(0);
		SetSmallLength(len);
	}else{
		usize real_cap = capacity * SPARE_RATE;
		m_Buffer    = (T*) operator new (sizeof(T) * real_cap); // allocate empty storage
		m_Buffer[0] = T(0); // init to 0
		m_Capacity = capacity;
		SetNormalLength(len);
	}
}

template<typename T>
BasicString2<T>::~BasicString2()
{
	if (!this->IsSmall() && m_Buffer != NULL) {
		::operator delete(m_Buffer);
	}
}

template<typename T>
INLINE const T* BasicString2<T>::Buffer() const
{
	return this->IsSmall() ? m_Data : m_Buffer;
}

template<typename T>
INLINE usize BasicString2<T>::Size() const
{
	return this->Length() - 1;
}

template<typename T>
INLINE usize BasicString2<T>::Capacity() const
{
	return this->IsSmall() ? SSO_SIZE : m_Capacity;
}

template<typename T>
INLINE bool BasicString2<T>::IsEmpty() const
{
	return this->Size() == 0;
}

template<typename T>
INLINE typename BasicString2<T>::Iterator BasicString2<T>::begin() noexcept
{ 
	return this->EditableBuffer(); 
}

template<typename T>
INLINE typename BasicString2<T>::Iterator BasicString2<T>::end() noexcept
{
	return this->EditableBuffer() + this->Size(); 
}

template<typename T>
INLINE void BasicString2<T>::Reserve(usize s)
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
		::operator delete(m_Buffer);
	}else{
		SetNormalLength(len);
	}
	m_Buffer = temp_buffer;
	this->SetCapacity(s); // we can use this because its no longer small string at this point.
}

template<typename T>
void BasicString2<T>::Resize(usize s)
{
	usize len = this->Length();
	if (s == len) return;
	bool isSmall = this->IsSmall();
	if (s < len) {
		ASSERTF(len - s < 0, "String::Resize function is used with bad parameter.");
		if (isSmall) {
			usize nlen = (len - s); // new len remmber that len does represent whats rest in the buffer (when SSO is enabled)
			SetSmallLength(nlen);
			m_Data[nlen] = T(0);
		}else{
			SetNormalLength(len - s);
			m_Buffer[len - s] = T(0);
		}
	}else if (s > this->Capacity()){
		T* temp_buffer = (T*) operator new (sizeof(T)*s);
		if (isSmall) { // String was small and now it gonna convert to normal.
			for (usize i = 0; i < len; i++) {
				temp_buffer[i] = m_Data[i];
			}
			m_Buffer = temp_buffer;
			m_Capacity = s;
		}else{
			for (usize i = 0; i < len; i++) {
				temp_buffer[i] = m_Buffer[i];
			}
			::operator delete(m_Buffer);
			m_Buffer = temp_buffer;
			m_Capacity = s;
		}
		m_Buffer[s - 1] = T(0);
		SetNormalLength(s);
	}
}

template<typename T>
INLINE void BasicString2<T>::Clear()
{
	bool isSmall = this->IsSmall();
	if (isSmall) {
		m_Data[0] = T(0);
		SetSmallLength(1);
	}else{
		m_Buffer[0] = T(0);
		SetNormalLength(1);
	}
	
}

template<typename T>
void BasicString2<T>::Append(const BasicString2<T>& str)
{
	usize tlen        = this->Length() - 1; // -1 is to remove the trailing null terminator
	usize slen	      = str.Length();
	usize finalLen	  = tlen + slen;
	const T* str_buffer_ptr = str.Buffer();
	
	if (finalLen > this->Capacity()) { // -1 is to remove the trailing null terminator
		this->Reserve(finalLen * SPARE_RATE);
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
void BasicString2<T>::Append(const char* str)
{
	usize tlen = this->Length() - 1; // -1 is to remove the trailing null terminator
	usize slen = strlen(str) + 1;
	usize finalLen = tlen + slen;
	const char* str_buffer_ptr = str;

	if (finalLen > this->Capacity()) { // -1 is to remove the trailing null terminator
		this->Reserve(finalLen * SPARE_RATE);
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
INLINE T BasicString2<T>::At(usize i) const
{
	ASSERTF((i > Length()), "Bad usage of String::At (index out of bound).");
	return this->Buffer()[i];
}

template<typename T>
INLINE T BasicString2<T>::Back() const
{
	return this->Buffer()[this->Size()-1];
}

template<typename T>
INLINE T BasicString2<T>::Front() const
{
	return this->Buffer()[0];
}

template<typename T>
void BasicString2<T>::PushBack(T c)
{
	usize len = this->Length();
	this->Reserve((len + 1*SPARE_RATE));
	T* buffer_ptr = this->EditableBuffer();
	buffer_ptr[len++] = c;
	buffer_ptr[len] = T(0);
	this->SetLength(len);
}

template<typename T>
void BasicString2<T>::PopBack()
{
	usize len = this->Length();
	if (len <= 1) return;
	T* buffer_ptr = this->EditableBuffer();
	buffer_ptr[len - 1] = T(0);
	this->SetLength(len - 1);
}

template<typename T>
void BasicString2<T>::Erase(usize pos, usize offset)
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
void BasicString2<T>::Insert(usize pos, const BasicString2<T>& str)
{
	usize tlen = this->Length(); // remove the trailing null
	usize slen = str.Length() - 1;
	this->Resize(tlen + slen);
	//Shift the last part to the end
    BasicString2<T> temp_str(tlen - pos);
	temp_str.Copy(*this, pos, tlen);
	T* this_buffer = this->EditableBuffer();
	const T* temp_buffer = temp_str.Buffer();
	const T* str_buffer = str.Buffer();

	for (usize i = pos, j = 0; j < slen; i++, j++) {
		this_buffer[i] = str_buffer[j];
	}

	for (usize i = pos + slen, j = 0; j < tlen - pos; i++, j++) {
		this_buffer[i] = temp_buffer[j];
	}

	this->SetLength(tlen + slen);
}

template<typename T>
void BasicString2<T>::Copy(const BasicString2<T>& str, usize pos, usize offset)
{
	usize slen = str.Length();
	//ASSERTF((offset > slen), "Bad usage of String::Copy() pos or offset is out of bound.");
	const T* buffer_ptr = str.Buffer();
	usize end = pos + offset;
	if (end >= slen) end = slen - 1;
	if (offset + 1 < SSO_SIZE) {
		SetSmallLength(offset + 1);
		usize i, j;

		for (i = pos, j = 0; i < end; i++, j++) {
			m_Data[j] = buffer_ptr[i];
		}

		m_Data[j] = '\0';
	}else{
		this->Reserve(offset + 1);
		SetNormalLength(offset + 1);

		usize i, j;
		for (i = pos, j = 0; i < end; i++, j++) {
			m_Buffer[j] = buffer_ptr[i];
		}

		m_Buffer[j] = '\0';
	}
}

template<typename T>
INLINE BasicString2<T> BasicString2<T>::SubString(usize pos, usize off) const
{
    BasicString2<T> temp_str(off + 1);
	temp_str.Copy(*this, pos, off);
	return temp_str;
}

template<typename T>
INLINE ssize BasicString2<T>::Find(const BasicString2<T>& pattren, uint32 start) const
{
	return SearchBoyerMoore(*this, pattren, start);
}

template<typename T>
INLINE Vector<BasicString2<T>> BasicString2<T>::Split(T delimter)
{
    Vector<BasicString2<T>> fragments;
	usize start_off = 0;
	T* str_buffer = this->EditableBuffer();
	T* pch = this->FindFirstHelper(str_buffer, delimter);
	T* last_pch = str_buffer - 1;

	if (pch != NULL) {
		while (pch != NULL) {
			fragments.EmplaceBack(std::move(this->SubString(start_off, pch - last_pch - 1)));
			start_off = pch - str_buffer + 1;
			last_pch = pch;
			pch = this->FindFirstHelper(pch + 1, delimter);
		}

		fragments.EmplaceBack(std::move(this->SubString(start_off, this->Length())));
	}

	return fragments;
}

template<typename T>
INLINE int32 BasicString2<T>::FindFirst(T delimter)
{
	T* str_buffer = this->EditableBuffer();
	T* pch = this->FindFirstHelper(str_buffer, delimter);

	if (pch)
		return (int32) (pch - str_buffer);

	return -1;
}

template<typename T>
INLINE int32 BasicString2<T>::FindLast(T delimter)
{
	T* str_buffer = this->EditableBuffer();
	T* pch = this->FindLastHelper(str_buffer, delimter, this->Length());

	if (pch)
		return (int32) (pch - str_buffer);

	return -1;
}

template<typename T>
INLINE void BasicString2<T>::EraseAfterTheLast(T delimter)
{
	T* str_buffer = this->EditableBuffer();
	T* pch = this->FindLastHelper(str_buffer, delimter, this->Length());
	*pch = 0;
	this->SetLength(pch - str_buffer);
}

template<typename T>
INLINE void BasicString2<T>::EraseAfterTheFirst(T delimter)
{
	T* str_buffer = this->EditableBuffer();
	T* pch = this->FindFirstHelper(str_buffer, delimter);
	*pch = 0;
	this->SetLength(pch - str_buffer);
}

template<typename T>
INLINE T* BasicString2<T>::FindLastHelper(T* buffer, T delimter, usize length)
{
	T* start = buffer + length;
	for (usize i = length; start != buffer; i--) {
		if (buffer[i] == delimter) {
			return buffer + i;
		}
	}

	return NULL;
}

template<typename T>
INLINE T* BasicString2<T>::FindFirstHelper(T* buffer, T delimter)
{
	usize len = this->Length();

	for (usize i = 0; i < len; i++) {
		if (buffer[i] == delimter) {
			return buffer + i;
		}
	}

	return NULL;
}

//Copy ctor copy assignement
template<typename T>
BasicString2<T>::BasicString2(const BasicString2<T>& other)
{
	bool IsOtherSmall = other.IsSmall();
	usize len = other.Length();
	if (IsOtherSmall) {
		for (usize i = 0; i < len; i++) {
			m_Data[i] = other.m_Data[i];
		}

		SetSmallLength(len);
	}else {
		usize cap = other.m_Capacity;
		m_Buffer = (T*) operator new (sizeof(T) * cap); // allocate empty storage
		
		for (usize i = 0; i < len; i++) {
			m_Buffer[i] = other.m_Buffer[i];
		}
		
		m_Capacity = cap;
		SetNormalLength(len);
	}
}

template<typename T>
BasicString2<T>& BasicString2<T>::operator=(const BasicString2<T>& other)
{
	if (this == &other) {
		return *this;
	}

	bool IsOtherSmall = other.IsSmall();
	usize cap = other.Capacity();
	usize len = other.Length();

	if (IsOtherSmall && Capacity() <= cap) {

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
		SetNormalLength(len);
		m_Capacity = cap;
	}

	return *this;
}

//Move ctor move assignement
template<typename T>
BasicString2<T>::BasicString2(BasicString2<T>&& other)
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
		other.SetSmallLength(1); // Pervent the other string from deleting when it go out of scope
	}
}

template<typename T>
BasicString2<T>& BasicString2<T>::operator=(BasicString2<T>&& other)
{
	bool IsOtherSmall = other.IsSmall();
	usize len = other.Length();
	if (!this->IsSmall() && m_Buffer != NULL) {
		::operator delete(m_Buffer);
	}
	if (IsOtherSmall) {
		for (usize i = 0; i < len; i++) {
			m_Data[i] = other.m_Data[i];
		}
		SetSmallLength(len);
	}else{
		m_Buffer = other.m_Buffer;
		m_Capacity = other.m_Capacity;
		SetNormalLength(len);
		other.SetSmallLength(1); // Pervent the other string from deleting when it go out of scope
	}
	return *this;
}

template<typename T>
INLINE BasicString2<T>& BasicString2<T>::operator+=(const BasicString2<T>& other)
{
	this->Append(other);
	return *this;
}

template<typename T>
INLINE BasicString2<T>& BasicString2<T>::operator+=(const char* other)
{
	this->Append(other);
	return *this;
}

template<typename T>
INLINE usize BasicString2<T>::Length() const
{
#if ENDIANNESS == LITTLE_ENDIAN
	return this->IsSmall() ? SSO_SIZE - m_Data[SSO_MI] : (m_Length << 1) >> 1;
#else
	return this->IsSmall() ? SSO_SIZE - (m_Data[SSO_MI] >> 1) : m_Length >> 1;
#endif
}

template<typename T>
INLINE usize BasicString2<T>::Length()
{
#if ENDIANNESS == LITTLE_ENDIAN
	return this->IsSmall() ? SSO_SIZE - m_Data[SSO_MI] : (m_Length << 1) >> 1;
#else
	return this->IsSmall() ? SSO_SIZE - (m_Data[SSO_MI] >> 1) : m_Length >> 1;
#endif
}

/********** PRIVATE FUNCTIONS **********/

template<typename T>
INLINE bool BasicString2<T>::IsSmall() const
{  
#if ENDIANNESS == LITTLE_ENDIAN
	return !(m_Data[SSO_MI] & hight_bit_mask_normal_str); // if the bit is 0 then its small string otherwise its normal
#else
	return !(m_Data[SSO_MI] & 0b1); // if the bit is 0 then its small string otherwise its normal
#endif
}

template<typename T>
INLINE void BasicString2<T>::SetLength(usize nlen)
{
	if (this->IsSmall() && nlen <= SSO_SIZE) {
		SetSmallLength(nlen);
	}else{
		SetNormalLength(nlen);
	}
}

template<typename T>
INLINE void BasicString2<T>::SetSmallLength(usize nlen)
{
#if ENDIANNESS == LITTLE_ENDIAN
	m_Data[SSO_MI] = T(SSO_SIZE - nlen); // Highest bit must be 0!
										 // We can use (T(SSO_SIZE - nlen) << 1) >> 1 to enforce it but it wont reach the maximum anyways!
#else
	m_Data[SSO_MI] = (T(SSO_SIZE - nlen) << 1);
#endif
}

template<typename T>
INLINE void BasicString2<T>::SetNormalLength(usize nlen)
{
#if ENDIANNESS == LITTLE_ENDIAN
	m_Length = nlen | hight_bit_mask_normal_str;
#else
	m_Length = (nlen << 1) | 0b1;
#endif
}

template<typename T>
INLINE void BasicString2<T>::SetCapacity(usize ncap)
{
	if (this->IsSmall()) return;
	m_Capacity = ncap;
}

template<typename T>
INLINE T* BasicString2<T>::EditableBuffer()
{
	return this->IsSmall() ? m_Data : m_Buffer;
}

template class BasicString2<char>;
template class BasicString2<char16_t>;
template class BasicString2<wchar_t>;
//template class BasicString2<int>; //char32_t

TRE_NS_END
