#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Maths/Maths.hpp>
//https://www.mathsisfun.com/binary-decimal-hexadecimal-converter.html
TRE_NS_START



class Bitset
{
public:
	typedef uint8 Block;

	struct BitsBlock
	{
		friend class Bitset;
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
	~Bitset();

	Bitset();

	Bitset(usize n);

	Bitset(usize n, bool init_val);

	Bitset(const Bitset& other);

	Bitset(Bitset&& other);

	Bitset& operator=(const Bitset& other);

	Bitset& operator=(Bitset&& other);

	void Append(bool bit);

	void Append(uint8 bit);

	bool Get(uint32 index) const;

	bool operator[](uint32 index) const;

	BitsBlock operator[](uint32 index);

	void Set(uint32 index, bool value);

	void Toggle(uint32 index);

	FORCEINLINE usize Length() const;

	Bitset& operator|= (const Bitset& other);

	Bitset& operator&= (const Bitset& other);

	Bitset& operator^= (const Bitset& other);

	Bitset& operator~(); // TODO: Needs more testing not sure if it work properly

	Bitset& operator<<=(usize n);

	Bitset& operator>>=(usize n);

	Bitset& operator+=(Bitset other);

	bool operator==(const Bitset& other);

	FORCEINLINE bool operator!=(const Bitset& other);

private:
	CONSTEXPR static uint32 NB_BYTES_SMALL = sizeof(usize*) + sizeof(usize);
	CONSTEXPR static uint32 NB_BITS_PER_ELEMENT = sizeof(usize) * BITS_PER_BYTE; // Number of bits before we move on to the next element and allocate it
	CONSTEXPR static uint32 NB_BITS_SMALL =  (sizeof(usize*) + sizeof(usize) - sizeof(uint8)) * BITS_PER_BYTE;
	CONSTEXPR static uint32 LAST_INDEX = NB_BYTES_SMALL - 1; //  for small endian (make sure it will work too on big endian)
	CONSTEXPR static uint32 IS_SMALL = 0;
	CONSTEXPR static usize SMALL_SIZE = NB_BYTES_SMALL * BITS_PER_BYTE;

#if ENDIANNESS == LITTLE_ENDIAN
	CONSTEXPR static usize hight_bit_mask_normal = usize(1) << ((sizeof(usize) * BITS_PER_BYTE) - 1);
#else
#endif

	FORCEINLINE uint8 GetSmallLength() const { return m_Data[LAST_INDEX]; };

	FORCEINLINE usize GetNormalLength() const { return (m_Length << 1) >> 1; };

	FORCEINLINE void SetSmall(usize nlen);

	FORCEINLINE void SetNormal(usize nlen);

	FORCEINLINE bool IsSmall() const;

	FORCEINLINE void SetLength(usize len);

	FORCEINLINE void Free();

	void Reserve(usize cap);

	void Fill(bool bit);

	union {
		struct {
			usize* m_Bits;
			usize m_Length;
		};
		
		uint8 m_Data[NB_BYTES_SMALL];
	};
	
};

// private members : 
FORCEINLINE void Bitset::SetSmall(usize nlen)
{
#if ENDIANNESS == LITTLE_ENDIAN
	m_Data[LAST_INDEX] = uint8(nlen); // Highest bit must be 0!
#else
	m_Data[SSO_MI] = (T(SSO_SIZE - nlen) << 1);
#endif
}

FORCEINLINE void Bitset::SetNormal(usize nlen)
{
	m_Length = nlen | hight_bit_mask_normal;
}

FORCEINLINE bool Bitset::IsSmall() const
{
	return !(m_Length & hight_bit_mask_normal); // if the bit is 0 then its small string otherwise its normal
}

FORCEINLINE void Bitset::SetLength(usize nlen)
{
	if (nlen > NB_BITS_SMALL) {
		this->SetNormal(nlen);
	}else {
		this->SetSmall(nlen);
	}
}

FORCEINLINE usize Bitset::Length() const
{
	return this->IsSmall() ? (usize)this->GetSmallLength() : this->GetNormalLength();
}

FORCEINLINE Bitset::Bitset()
{
}

FORCEINLINE Bitset::Bitset(usize n)
{
	this->Reserve(n);
	this->SetLength(n);
}

FORCEINLINE Bitset::Bitset(usize n, bool init_val)
{
	this->Reserve(n);
	this->SetLength(n);
	this->Fill(init_val);
}

FORCEINLINE void Bitset::Append(uint8 bit)
{
	return this->Append((bool)bit);
}

FORCEINLINE bool Bitset::operator[](uint32 index) const
{
	return this->Get(index);
}

FORCEINLINE void Bitset::Free()
{
	if (!this->IsSmall())
		delete[] m_Bits;
}

FORCEINLINE bool Bitset::operator!=(const Bitset& other)
{
	return !(*this == other);
}

FORCEINLINE Bitset operator|(const Bitset& a, const Bitset& b)
{
	Bitset res(a);
	res |= b;
	return res;
}

FORCEINLINE Bitset operator&(const Bitset& a, const Bitset& b)
{
	Bitset res(a);
	res &= b;
	return res;
}

FORCEINLINE Bitset operator^(const Bitset& a, const Bitset& b)
{
	Bitset res(a);
	res ^= b;
	return res;
}

FORCEINLINE Bitset operator<<(const Bitset& a, usize n)
{
	Bitset res(a);
	res <<= n;
	return res;
}

FORCEINLINE Bitset operator>>(const Bitset& a, usize n)
{
	Bitset res(a);
	res >>= n;
	return res;
}

FORCEINLINE Bitset operator+(const Bitset& a, const Bitset& b)
{
	Bitset res(a);
	res += b;
	return res;
}

TRE_NS_END

