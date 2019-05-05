#pragma once

#include <Core/Misc/Defines/Common.hpp>

TRE_NS_START

template<typename T>
class BasicString
{
public:
	BasicString();

	template<usize S>
	BasicString(const T(&str)[S]);

	~BasicString();

	FORCEINLINE bool Empty();
	FORCEINLINE void Reserve(usize s);
	FORCEINLINE void Resize(usize s);
	FORCEINLINE void Clear();
	

	void Append(const BasicString<T>& str);
	void PushBack(T c);
	void Erase(usize pos, usize offset);
	void Insert(usize pos, const BasicString<T>& str);
	void Copy(const BasicString<T>& str, usize pos, usize offset);
	/*template<usize S>
	void Insert(usize pos, const T(&str)[S]);
	template<usize S>
	void Insert(usize pos, const char* str, usize str_len);*/

	FORCEINLINE const T*	Buffer()   const;
	FORCEINLINE const usize	Length()   const;
	FORCEINLINE const usize	Capacity() const;
	FORCEINLINE T			At(usize i) const;
	FORCEINLINE T			Back()		const;
	FORCEINLINE T			Front()		const;

	friend bool operator== (BasicString<T>&& a, BasicString<T>&& b);
	friend bool operator!= (BasicString<T>&& a, BasicString<T>&& b);
	friend bool operator== (const BasicString<T>& a, const BasicString<T>& b);
	friend bool operator!= (const BasicString<T>& a, const BasicString<T>& b);
private:
	T*		m_Buffer;
	usize	m_Length;
	usize	m_Capacity;
};


template<typename T>
template<usize S>
BasicString<T>::BasicString(const T(&str)[S]) : m_Capacity(S), m_Length(S-1)
{
	m_Buffer = (T*) operator new (sizeof(T)*S); // allocate empty storage
	for (usize i = 0; i < S; i++) {
		m_Buffer[i] = str[i];
	}
}

template<typename T>
static bool operator==(BasicString<T>&& a, BasicString<T>&& b)
{
	if (a.m_Length != b.m_Length) return false;
	for (usize i = 0; i < a.m_Length; i++) {
		if (a.m_Buffer[i] != b.m_Buffer[i]) {
			return false;
		}
	}
	return true;
}

template<typename T>
static bool operator!=(BasicString<T>&& a, BasicString<T>&& b)
{
	return !(a == b);
}

template<typename T>
static bool operator==(const BasicString<T>& a, const BasicString<T>& b)
{
	if (a.m_Length != b.m_Length) return false;
	for (usize i = 0; i < a.m_Length; i++) {
		if (a.m_Buffer[i] != b.m_Buffer[i]) {
			return false;
		}
	}
	return true;
}

template<typename T>
static bool operator!=(const BasicString<T>& a, const BasicString<T>& b)
{
	return !(a == b);
}

typedef BasicString<char> String;

TRE_NS_END

