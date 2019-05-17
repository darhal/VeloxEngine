#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Defines/Debug.hpp>

TRE_NS_START

class ReferenceCounter
{
public:
	FORCEINLINE ReferenceCounter() : m_refCounter(0) {};

	FORCEINLINE uint32 GetCount() const;
	FORCEINLINE ReferenceCounter& operator++(int);
	FORCEINLINE ReferenceCounter& operator--(int);
	FORCEINLINE bool operator==(const ReferenceCounter& o);
	FORCEINLINE operator uint32();
private:
	uint32 m_refCounter;
};

FORCEINLINE ReferenceCounter& ReferenceCounter::operator++(int)
{
	m_refCounter++;
	return *this;
}

FORCEINLINE ReferenceCounter& ReferenceCounter::operator--(int)
{
	m_refCounter--;
	return *this;
}

FORCEINLINE bool ReferenceCounter::operator==(const ReferenceCounter& o)
{
	return o.m_refCounter == m_refCounter;
}

FORCEINLINE ReferenceCounter::operator uint32()
{
	return m_refCounter;
}

FORCEINLINE uint32 ReferenceCounter::GetCount() const
{
	return m_refCounter;
}

typedef ReferenceCounter RefCounter;

TRE_NS_END