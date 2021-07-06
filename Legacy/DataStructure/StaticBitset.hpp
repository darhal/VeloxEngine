/*#pragma once

#include <Legacy/Misc/Defines/Common.hpp>
#include <Legacy/Math/Maths.hpp>

// https://www.mathsisfun.com/binary-decimal-hexadecimal-converter.html

TRE_NS_START

template<uint32 N>
class StaticBitset
{
public:
	typedef uint8 Block;

	struct BitsBlock
	{
		friend class StaticBitset<N>;
	public:

		FORCEINLINE BitsBlock(Block& value, uint32 pos) : m_Block(value), m_Mask(Block(1) << pos) {}

		FORCEINLINE operator bool() const { return (m_Block & m_Mask) != 0; }
		FORCEINLINE bool operator~() const { return (m_Block & m_Mask) == 0; }

		FORCEINLINE BitsBlock& Flip() { this->Flip(); return *this; }

		FORCEINLINE BitsBlock& operator=(bool x) { this->Assign(x);   return *this; } // for b[i] = x
		FORCEINLINE BitsBlock& operator=(const BitsBlock& rhs) { this->Assign(rhs); return *this; } // for b[i] = b[j]

		FORCEINLINE BitsBlock& operator|=(bool x) { if (x) DoSet();   return *this; }
		FORCEINLINE BitsBlock& operator&=(bool x) { if (!x) DoReset(); return *this; }
		FORCEINLINE BitsBlock& operator^=(bool x) { if (x) DoFlip();  return *this; }
		FORCEINLINE BitsBlock& operator-=(bool x) { if (x) DoReset(); return *this; }

	private:
		Block& m_Block;
		const Block m_Mask;

		FORCEINLINE void DoSet() { m_Block |= m_Mask; }
		FORCEINLINE void DoReset() { m_Block &= ~m_Mask; }
		FORCEINLINE void DoFlip() { m_Block ^= m_Mask; }
		FORCEINLINE void Assign(bool x) { x ? DoSet() : DoReset(); }
	};
public:
	StaticBitset();

	StaticBitset(usize n);

	StaticBitset(bool init_val);

	StaticBitset(const StaticBitset& other);

	StaticBitset(StaticBitset&& other);

	StaticBitset& operator=(const StaticBitset& other);

	StaticBitset& operator=(StaticBitset&& other);

	bool Get(uint32 index) const;

	bool operator[](uint32 index) const;

	BitsBlock operator[](uint32 index);

	void Set(uint32 index, bool value);

	void Toggle(uint32 index);

	FORCEINLINE CONSTEXPR usize Length() const { return N; }

	usize GetHash() const;

	StaticBitset& operator|= (const StaticBitset& other);

	StaticBitset& operator&= (const StaticBitset& other);

	StaticBitset& operator^= (const StaticBitset& other);

	StaticBitset& operator~(); // TODO: Needs more testing not sure if it work properly

	StaticBitset& operator<<=(usize n);

	StaticBitset& operator>>=(usize n);

	StaticBitset& operator+=(StaticBitset other);

	bool operator==(const StaticBitset& other) const;

	FORCEINLINE bool operator!=(const StaticBitset& other) const;

	bool operator>(const StaticBitset& other) const;

	bool operator<=(const StaticBitset& other) const;

	bool operator<(const StaticBitset& other) const;

	bool operator>=(const StaticBitset& other) const;

	operator bool();
private:
	CONSTEXPR static uint32 NB_BYTES_SMALL = 1 + ((N - 1) / BITS_PER_BYTE);

	void Reserve(usize cap);

	void Fill(bool bit);

	uint8 m_Bits[NB_BYTES_SMALL];
};


template<uint32 N>
FORCEINLINE StaticBitset<N>::StaticBitset()
{
	memset(m_Bits, 0, sizeof(uint8) * NB_BYTES_SMALL);
}

template<uint32 N>
FORCEINLINE StaticBitset<N>::StaticBitset(bool init_val)
{
	memset(m_Bits, init_val ? 255 : 0, sizeof(uint8) * NB_BYTES_SMALL);
}

template<uint32 N>
FORCEINLINE bool StaticBitset<N>::operator[](uint32 index) const
{
	return this->Get(index);
}

template<uint32 N>
FORCEINLINE bool StaticBitset<N>::operator!=(const StaticBitset<N>& other) const
{
	return !(*this == other);
}

template<uint32 N>
FORCEINLINE StaticBitset<N> operator|(const StaticBitset<N>& a, const StaticBitset<N>& b)
{
	StaticBitset<N> res(a);
	res |= b;
	return res;
}

template<uint32 N>
FORCEINLINE StaticBitset<N> operator&(const StaticBitset<N>& a, const StaticBitset<N>& b)
{
	StaticBitset<N> res(a);
	res &= b;
	return res;
}

template<uint32 N>
FORCEINLINE StaticBitset<N> operator^(const StaticBitset<N>& a, const StaticBitset<N>& b)
{
	StaticBitset<N> res(a);
	res ^= b;
	return res;
}

template<uint32 N>
FORCEINLINE StaticBitset<N> operator<<(const StaticBitset<N>& a, usize n)
{
	StaticBitset<N> res(a);
	res <<= n;
	return res;
}

template<uint32 N>
FORCEINLINE StaticBitset<N> operator>>(const StaticBitset<N>& a, usize n)
{
	StaticBitset<N> res(a);
	res >>= n;
	return res;
}

template<uint32 N>
FORCEINLINE StaticBitset<N> operator+(const StaticBitset<N>& a, const StaticBitset<N>& b)
{
	StaticBitset<N> res(a);
	res += b;
	return res;
}

TRE_NS_END

*/