#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/DataStructure/HashMap/HashFunction.hpp>
#include <cstring>

TRE_NS_START

template<typename T, usize N>
class FixedString
{
public:
	FixedString(const T(&str)[N]);

	FixedString(const T* str);

	usize GetHash() const { return m_Hash; }

	const T* GetString() const { return m_Data; }

	T operator[](uint32 i) const { return m_Data[i]; }

private:
	T m_Data[N];
	usize m_Hash;
};

namespace FString
{
	template<typename T, usize N>
	FORCEINLINE FixedString<T, N> MakeFString(const T(&str)[N])
	{
		return FixedString<T, N>((const char*)str);
	}
}


template<typename T, usize N>
FixedString<T, N>::FixedString(const T(&str)[N]) : m_Hash(Hash::GetHash(str))
{
	memcpy(m_Data, str, (N + 1) * sizeof(T));
}

template<typename T, usize N>
FixedString<T, N>::FixedString(const T* str) : m_Hash(Hash::GetHash(str))
{
	memcpy(m_Data, str, (strlen(str) + 1) * sizeof(T));
}



TRE_NS_END