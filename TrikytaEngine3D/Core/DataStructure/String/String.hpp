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

typedef BasicString<char> String;

TRE_NS_END

