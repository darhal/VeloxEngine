#include "StaticBitset.hpp"
#include <Core/Misc/Defines/Debug.hpp>

TRE_NS_START

template<uint32 N>
void StaticBitset<N>::Fill(bool bit)
{
	uint8 bit_len = N;
	uint32 nb_of_bytes_to_set = Math::DivFloor(bit_len, sizeof(uint8) * BITS_PER_BYTE);
	memset(m_Bits, bit ? std::numeric_limits<uint8>::max() : 0, nb_of_bytes_to_set * sizeof(uint8));
	int32 rest_of_bits = (bit_len - nb_of_bytes_to_set * sizeof(uint8) * BITS_PER_BYTE);

	if (rest_of_bits > 0) {
		m_Bits[nb_of_bytes_to_set] |= (uint16(1) << (rest_of_bits)) - 1;
	}
}

template<uint32 N>
typename StaticBitset<N>::BitsBlock StaticBitset<N>::operator[](uint32 index)
{
	ASSERTF(index >= N, "StaticBitset<N> index out of range");
	uint32 nb_byte_to_look_at = Math::DivFloor(index, sizeof(uint8) * BITS_PER_BYTE);
	int32 rest_of_bits = (index - nb_byte_to_look_at * sizeof(uint8) * BITS_PER_BYTE);
	return BitsBlock(m_Bits[nb_byte_to_look_at], rest_of_bits);
}

template<uint32 N>
bool StaticBitset<N>::Get(uint32 index) const
{
	ASSERTF(index >= N, "Get() index out of range");
	uint32 nb_byte_to_look_at = Math::DivFloor(index, sizeof(uint8) * BITS_PER_BYTE);
	int32 rest_of_bits = (index - nb_byte_to_look_at * sizeof(uint8) * BITS_PER_BYTE);
	return m_Bits[nb_byte_to_look_at] & (uint16(1) << (rest_of_bits));
}

template<uint32 N>
void StaticBitset<N>::Set(uint32 index, bool value) // TODO!
{
	ASSERTF((index >= N), "StaticBitset<N> index out of range");

	uint32 nb_byte_to_look_at = Math::DivFloor(index, sizeof(uint8) * BITS_PER_BYTE);
	int32 rest_of_bits = (index - nb_byte_to_look_at * sizeof(uint8) * BITS_PER_BYTE);
	m_Bits[nb_byte_to_look_at] ^= (-value ^ m_Bits[nb_byte_to_look_at]) & (uint8(1) << rest_of_bits);
	return;
}

template<uint32 N>
void StaticBitset<N>::Toggle(uint32 index)
{
	ASSERTF((index >= N), "StaticBitset<N> index out of range");

	uint32 nb_byte_to_look_at = Math::DivFloor(index, sizeof(uint8) * BITS_PER_BYTE);
	int32 rest_of_bits = (index - nb_byte_to_look_at * sizeof(uint8) * BITS_PER_BYTE);
	m_Bits[nb_byte_to_look_at] ^= uint8(1) << rest_of_bits;
	return;
}

template<uint32 N>
StaticBitset<N>& StaticBitset<N>::operator|=(const StaticBitset<N>& other)
{
	uint32 nb_bytes = Math::DivCeil(N, sizeof(uint8) * BITS_PER_BYTE);

	for (uint32 i = 0; i < nb_bytes; i++) {
		m_Bits[i] |= other.m_Bits[i];
	}
	return *this;
}

template<uint32 N>
StaticBitset<N>& StaticBitset<N>::operator&=(const StaticBitset<N>& other)
{
	uint32 nb_bytes = Math::DivCeil(N, sizeof(uint8) * BITS_PER_BYTE);

	for (uint32 i = 0; i < nb_bytes; i++) {
		m_Bits[i] &= other.m_Bits[i];
	}
	return *this;
}

template<uint32 N>
StaticBitset<N>& StaticBitset<N>::operator^=(const StaticBitset<N>& other)
{
	uint32 nb_bytes = Math::DivCeil(N, sizeof(uint8) * BITS_PER_BYTE);

	for (uint32 i = 0; i < nb_bytes; i++) {
		m_Bits[i] ^= other.m_Bits[i];
	}
	return *this;
}

template<uint32 N>
StaticBitset<N>& StaticBitset<N>::operator~()
{
	uint32 nb_bytes = Math::DivFloor(N, sizeof(uint8) * BITS_PER_BYTE);

	for (uint32 i = 0; i < nb_bytes; i++) {
		m_Bits[i] = ~m_Bits[i];
	}

	for (uint32 i = nb_bytes * sizeof(uint8); i < N; i++) {
		int32 rest_of_bits = (i - nb_bytes * sizeof(uint8) * BITS_PER_BYTE);
		m_Bits[nb_bytes] ^= uint8(1) << rest_of_bits;
	}

	return *this;
}

template<uint32 N>
StaticBitset<N>& StaticBitset<N>::operator<<=(usize n)
{
	usize len = N;

	for (int  i = 0, j = n; i < len - n; i++, j++) {
		this->Set(i, this->Get(j));
	}

	for (int j = len - n; j < len; j++) {
		this->Set(j, 0);
	}

	return *this;
}

template<uint32 N>
StaticBitset<N>& StaticBitset<N>::operator>>=(usize n)
{
	usize len = N;
	StaticBitset<N> copy(*this);

	for (int i = 0, j = n; i < len - n; i++, j++) {
		this->Set(j, copy.Get(i));
	}

	for (int j = 0; j < n; j++) {
		this->Set(j, 0);
	}

	return *this;
}

template<uint32 N>
bool StaticBitset<N>::operator==(const StaticBitset<N>& other) const
{
	uint32 nb_bytes = Math::DivFloor(N, sizeof(uint8) * BITS_PER_BYTE);

	for (uint32 i = 0; i < nb_bytes; i++) {
		if (m_Bits[i] != other.m_Bits[i])
			return false;
	}

	for (uint32 i = nb_bytes * sizeof(uint8); i < N; i++) {
		if (this->Get(i) != other.Get(i))
			return false;
	}

	return true;
}

template<uint32 N>
StaticBitset<N>::StaticBitset(const StaticBitset<N>& other)
{
	memcpy(m_Bits, other.m_Bits, sizeof(other.m_Bits));
}

template<uint32 N>
StaticBitset<N>::StaticBitset(StaticBitset<N>&& other)
{
	memcpy(m_Bits, other.m_Bits, sizeof(other.m_Bits));
}

template<uint32 N>
StaticBitset<N>& StaticBitset<N>::operator=(const StaticBitset<N>& other)
{
	memcpy(m_Bits, other.m_Bits, sizeof(other.m_Bits));
	return *this;
}

template<uint32 N>
StaticBitset<N>& StaticBitset<N>::operator=(StaticBitset<N>&& other)
{
	memcpy(m_Bits, other.m_Bits, sizeof(other.m_Bits));
	return *this;
}

template<uint32 N>
StaticBitset<N>& StaticBitset<N>::operator+=(StaticBitset<N> other)
{
	StaticBitset<N> carry;

	while (other != StaticBitset<N>(false)) {
		carry = *this & other;
		*this ^= other;
		other = carry >> (usize)1;
	}

	return *this;
}

template<uint32 N>
usize StaticBitset<N>::GetHash() const
{
	size_t hash = 0;
	const usize* bits = (usize*)m_Bits;
	uint32 nb_usize_bytes = Math::DivCeil(N, sizeof(usize) * BITS_PER_BYTE);

	for (usize i = 0; i < nb_usize_bytes; i++)
		hash ^= bits[i];

	return hash;
}

template<uint32 N>
bool StaticBitset<N>::operator>(const StaticBitset<N>& other) const
{
	uint32 nb_bytes = Math::DivFloor(N, sizeof(uint8) * BITS_PER_BYTE);

	for (uint32 i = nb_bytes * sizeof(uint8); i < N; i++) {
		if (this->Get(i) && !other.Get(i))
			return true;
	}

	for (uint32 i = 0; i < nb_bytes; i++) {
		if (m_Bits[i] > other.m_Bits[i])
			return true;
	}

	return false;
}

template<uint32 N>
bool StaticBitset<N>::operator<=(const StaticBitset<N>& other) const
{
	return !(*this > other);
}

template<uint32 N>
bool StaticBitset<N>::operator<(const StaticBitset<N>& other) const
{
	return other > *this;
}

template<uint32 N>
bool StaticBitset<N>::operator>=(const StaticBitset<N>& other) const
{
	return !(*this < other);
}

template<uint32 N>
StaticBitset<N>::operator bool()
{
	usize len = N;

	if (len == 0)
		return false;

	uint32 nb_bytes = Math::DivFloor(len, sizeof(uint8) * BITS_PER_BYTE);

	for (uint32 i = 0; i < nb_bytes; i++) {
		if (m_Bits[i])
			return true;
	}

	for (uint32 i = nb_bytes * sizeof(uint8); i < N; i++) {
		if (this->Get(i))
			return true;
	}

	return false;
}

TRE_NS_END