#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Defines/Debug.hpp>
#include <atomic>

TRE_NS_START

template<bool IS_MT>
class ReferenceCounter {
public:
	FORCEINLINE ReferenceCounter& operator++(int);
	FORCEINLINE ReferenceCounter& operator--(int);
	FORCEINLINE bool operator==(const ReferenceCounter& o);
	FORCEINLINE uint32 GetCount() const;
	FORCEINLINE operator uint32() const;
};

template<> 
class ReferenceCounter<true>
{
public:
	FORCEINLINE ReferenceCounter() : m_refCounter(0) {};
	FORCEINLINE ReferenceCounter(uint x) : m_refCounter(x) {};

	FORCEINLINE ReferenceCounter& operator++(int);
	FORCEINLINE ReferenceCounter& operator--(int);
	FORCEINLINE bool operator==(const ReferenceCounter& o);

	FORCEINLINE uint32 GetCount() const;
	FORCEINLINE operator uint32() const;

	CONSTEXPR static bool IsThreadSafe = true;
private:
	std::atomic<uint32> m_refCounter;
};

template<>
class ReferenceCounter<false>
{
public:
	FORCEINLINE ReferenceCounter() : m_refCounter(0) {};
	FORCEINLINE ReferenceCounter(uint x) : m_refCounter(x) {};

	FORCEINLINE ReferenceCounter& operator++(int);
	FORCEINLINE ReferenceCounter& operator--(int);
	FORCEINLINE bool operator==(const ReferenceCounter& o);

	FORCEINLINE uint32 GetCount() const;
	FORCEINLINE operator uint32() const;

	CONSTEXPR static bool IsThreadSafe = false;
private:
	uint32 m_refCounter;
};

/**********************THREAD SAFE************************/
FORCEINLINE ReferenceCounter<true>& ReferenceCounter<true>::operator++(int) { m_refCounter++; return *this; }
FORCEINLINE ReferenceCounter<true>& ReferenceCounter<true>::operator--(int) { m_refCounter--; return *this; }
FORCEINLINE bool ReferenceCounter<true>::operator==(const ReferenceCounter<true>& o) { return o.m_refCounter == m_refCounter; }
FORCEINLINE ReferenceCounter<true>::operator uint32() const { return m_refCounter; }
FORCEINLINE uint32 ReferenceCounter<true>::GetCount() const{ return m_refCounter; }

/**********************NON THREAD SAFE************************/
FORCEINLINE ReferenceCounter<false>& ReferenceCounter<false>::operator++(int) { m_refCounter++; return *this; }
FORCEINLINE ReferenceCounter<false>& ReferenceCounter<false>::operator--(int) { m_refCounter--; return *this; }
FORCEINLINE bool ReferenceCounter<false>::operator==(const ReferenceCounter<false>& o) { return o.m_refCounter == m_refCounter; }
FORCEINLINE ReferenceCounter<false>::operator uint32() const { return m_refCounter; }
FORCEINLINE uint32 ReferenceCounter<false>::GetCount() const { return m_refCounter; }


template<bool IS_MT = false>
using RefCounter = ReferenceCounter<IS_MT>;

//A non thread safe but faster version of RefCounter
typedef ReferenceCounter<false> RefCount;

TRE_NS_END