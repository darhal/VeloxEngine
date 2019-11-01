#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Maths/Maths.hpp>
//https://www.mathsisfun.com/binary-decimal-hexadecimal-converter.html
TRE_NS_START

class Bitset
{
public:
	Bitset();

	Bitset(usize n);

	Bitset(usize n, bool init_val);

	void Append(bool bit);

	void Append(uint8 bit);

	bool Get(uint32 index) const;

	bool operator[](uint32 index) const;

	void Set(uint32 index, bool value);

	void Toggle(uint32 index);

	FORCEINLINE usize Length() const;

	Bitset& operator|= (const Bitset& other);
	Bitset& operator&= (const Bitset& other);
	Bitset& operator^= (const Bitset& other);
	Bitset& operator~();
	Bitset& operator<<=(usize n);
	Bitset& operator>>=(usize n);

//private:
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

	FORCEINLINE void Reserve(usize cap);

	FORCEINLINE void Fill(bool bit);

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

FORCEINLINE void Bitset::Reserve(usize ncap) // fix this!
{
	if (ncap <= NB_BITS_SMALL)
		return;

	bool is_small = this->IsSmall();
	uint32 nb_of_usize = (uint32) Math::Ceil((double)ncap / (double)(sizeof(usize) * BITS_PER_BYTE));
	uint32 curr_nb_of_usize = (uint32)Math::Ceil((double)this->GetNormalLength() / (double)(sizeof(usize) * BITS_PER_BYTE));

	if (is_small) {
		usize* new_bits = new usize[nb_of_usize];
		uint32 bytes_to_copy = sizeof(uint8) * (LAST_INDEX); // copy all bytes execpt the last one
		memcpy(new_bits, m_Data, bytes_to_copy);
		memset((uint8*)new_bits + bytes_to_copy, 0, nb_of_usize * sizeof(usize) - bytes_to_copy); // set what remains to 0
		m_Bits = new_bits;
		this->SetNormal((usize)this->GetSmallLength()); // because it was small, but now its normal
	}else if(!is_small && nb_of_usize > curr_nb_of_usize) {
		usize* new_bits = new usize[nb_of_usize];
		uint32 bytes_to_copy = (uint32) Math::Ceil((double)this->GetNormalLength() / (double)(sizeof(uint8) * BITS_PER_BYTE));
		memcpy(new_bits, m_Bits, bytes_to_copy * sizeof(uint8)); // copy all bytes execpt the last one
		memset((uint8*)new_bits + bytes_to_copy, 0, nb_of_usize * sizeof(usize) - bytes_to_copy); // set what remains to 0
		delete[] m_Bits;
		m_Bits = new_bits;
	}
}


FORCEINLINE void Bitset::Fill(bool bit)
{
	bool is_small = this->IsSmall();

	if (is_small) {
		uint8 bit_len = this->GetSmallLength();
		uint32 nb_of_bytes_to_set = (uint32) Math::Floor((double)bit_len / (double)(sizeof(uint8) * BITS_PER_BYTE));
		memset(m_Data, bit ? std::numeric_limits<uint8>::max() : 0, nb_of_bytes_to_set * sizeof(uint8));
		int32 rest_of_bits = (bit_len - nb_of_bytes_to_set * sizeof(uint8) * BITS_PER_BYTE);
		if (rest_of_bits > 0) {
			m_Data[nb_of_bytes_to_set] |= (uint16(1) << (rest_of_bits)) - 1;
		}
	}else{
		usize bit_len = this->GetNormalLength();
		uint32 nb_of_usize_to_set = (uint32) Math::Floor((double)bit_len / (double)(sizeof(usize) * BITS_PER_BYTE));
		memset(m_Bits, bit ? std::numeric_limits<usize>::max() : 0ULL, nb_of_usize_to_set * sizeof(usize));
		int32 rest_of_bits = int32(bit_len - nb_of_usize_to_set * sizeof(usize) * BITS_PER_BYTE);
		if (rest_of_bits > 0) {
			m_Bits[nb_of_usize_to_set] |= (usize(1) << (rest_of_bits)) - 1;
		}
	}
}

Bitset::Bitset()
{
}

Bitset::Bitset(usize n)
{
	this->Reserve(n);
	this->SetLength(n);
}

Bitset::Bitset(usize n, bool init_val)
{
	this->Reserve(n);
	this->SetLength(n);
	this->Fill(init_val);
}

void Bitset::Append(bool bit)
{
	usize len = this->Length();
	this->Reserve(len + 1);
	this->SetLength(len + 1);
	bool is_small = this->IsSmall();

	if (!bit)
		return;

	if (is_small) {

		uint32 nb_byte_to_look_at = (uint32)Math::Floor((double)len / (double)(sizeof(uint8) * BITS_PER_BYTE));
		int32 rest_of_bits = (len - nb_byte_to_look_at * sizeof(uint8) * BITS_PER_BYTE);
		m_Data[nb_byte_to_look_at] ^= (-bit ^ m_Data[nb_byte_to_look_at]) & (uint8(1) << rest_of_bits);
		return;
	}

	uint32 nb_of_usize_to_look_at = (uint32)Math::Floor((double)len / (double)(sizeof(usize) * BITS_PER_BYTE));
	int32 rest_of_bits = int32(len - nb_of_usize_to_look_at * sizeof(usize) * BITS_PER_BYTE);
	m_Bits[nb_of_usize_to_look_at] ^= (-bit ^ m_Bits[nb_of_usize_to_look_at]) & (usize(1) << rest_of_bits);
}

void Bitset::Append(uint8 bit)
{
	return this->Append(bit);
}

bool Bitset::Get(uint32 index) const
{
	bool is_small = this->IsSmall();
	
	if (is_small) {
		ASSERTF((index >= this->GetSmallLength()), "Bitset index out of range");

		uint32 nb_byte_to_look_at = (uint32)Math::Floor((double)index / (double)(sizeof(uint8) * BITS_PER_BYTE));
		int32 rest_of_bits = (index - nb_byte_to_look_at * sizeof(uint8) * BITS_PER_BYTE);
		return m_Data[nb_byte_to_look_at] & (uint16(1) << (rest_of_bits));
	}

	ASSERTF((index >= this->GetNormalLength()), "Bitset index out of range");

	uint32 nb_of_usize_to_look_at = (uint32)Math::Floor((double)index / (double)(sizeof(usize) * BITS_PER_BYTE));
	int32 rest_of_bits = int32(index - nb_of_usize_to_look_at * sizeof(usize) * BITS_PER_BYTE);
	return m_Bits[nb_of_usize_to_look_at] & (usize(1) << (rest_of_bits));
}

bool Bitset::operator[](uint32 index) const
{
	return this->Get(index);
}

void Bitset::Set(uint32 index, bool value) // TODO!
{
	bool is_small = this->IsSmall();

	if (is_small) {
		ASSERTF((index >= this->GetSmallLength()), "Bitset index out of range");

		uint32 nb_byte_to_look_at = (uint32)Math::Floor((double)index / (double)(sizeof(uint8) * BITS_PER_BYTE));
		int32 rest_of_bits = (index - nb_byte_to_look_at * sizeof(uint8) * BITS_PER_BYTE);
		m_Data[nb_byte_to_look_at] ^= (-value ^ m_Data[nb_byte_to_look_at]) & (uint8(1) << rest_of_bits);
		return;
	}

	ASSERTF((index >= this->GetNormalLength()), "Bitset index out of range");

	uint32 nb_of_usize_to_look_at = (uint32)Math::Floor((double)index / (double)(sizeof(usize) * BITS_PER_BYTE));
	int32 rest_of_bits = int32(index - nb_of_usize_to_look_at * sizeof(usize) * BITS_PER_BYTE);
	m_Bits[nb_of_usize_to_look_at] ^= (-value ^ m_Bits[nb_of_usize_to_look_at]) & (usize(1) << rest_of_bits);
}

FORCEINLINE void Bitset::Toggle(uint32 index)
{
	bool is_small = this->IsSmall();

	if (is_small) {
		ASSERTF((index >= this->GetSmallLength()), "Bitset index out of range");

		uint32 nb_byte_to_look_at = (uint32)Math::Floor((double)index / (double)(sizeof(uint8) * BITS_PER_BYTE));
		int32 rest_of_bits = (index - nb_byte_to_look_at * sizeof(uint8) * BITS_PER_BYTE);
		m_Data[nb_byte_to_look_at] ^= uint8(1) << rest_of_bits;
		return;
	}

	ASSERTF((index >= this->GetNormalLength()), "Bitset index out of range");

	uint32 nb_of_usize_to_look_at = (uint32)Math::Floor((double)index / (double)(sizeof(usize) * BITS_PER_BYTE));
	int32 rest_of_bits = int32(index - nb_of_usize_to_look_at * sizeof(usize) * BITS_PER_BYTE);
	m_Bits[nb_of_usize_to_look_at] ^= (usize(1) << (rest_of_bits));
}

TRE_NS_END

