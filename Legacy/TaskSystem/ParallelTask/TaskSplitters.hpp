#pragma once

#include <Legacy/Misc/Defines/Common.hpp>

TRE_NS_START

class CountSplitter
{
public:
	explicit CountSplitter(uint32 count)
		: m_Count(count)
	{
	}

	template <typename T>
	FORCEINLINE bool Split(uint32 count) const
	{
		return (count > m_Count);
	}

private:
	uint32 m_Count;
};

class DataSizeSplitter
{
public:
	explicit DataSizeSplitter(uint32 size)
		: m_Size(size)
	{
	}

	template <typename T>
	FORCEINLINE bool Split(uint32 count) const
	{
		return (count * sizeof(T) > m_Size);
	}

private:
	uint32 m_Size;
};

TRE_NS_END