#pragma once

#include <Legacy/Misc/Defines/Common.hpp>

TRE_NS_START

class GUID
{
public:
	GUID() {};

	template<uint32 S>
	GUID(const uint8(&bytes)[S]) : m_Data(bytes)
	{};

	FORCEINLINE GUID& operator++();

	FORCEINLINE GUID& operator++(int32);

	FORCEINLINE uint8& operator[](uint32 index);
private:
	CONSTEXPR static uint32 GUID_SIZE = 16;
	uint8 m_Data[GUID_SIZE];
};


FORCEINLINE GUID& GUID::operator++()
{
	for (uint32 i = 0; i < GUID_SIZE; i++) {
		if (m_Data[i] < 255) {
			m_Data[i]++;
			return *this;
		}
	}
}

FORCEINLINE GUID& GUID::operator++(int32)
{
	return ++*this;
}

FORCEINLINE uint8& GUID::operator[](uint32 index)
{
	return m_Data[index]; 
}

TRE_NS_END