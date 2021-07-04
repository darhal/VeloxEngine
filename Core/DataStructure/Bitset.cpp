#include "Bitset.hpp"
#include <string.h>
#include <Core/Misc/Defines/Debug.hpp>

TRE_NS_START

void Bitset::Reserve(usize ncap) // fix this!
{
	if (ncap <= NB_BITS_SMALL)
		return;

	bool is_small = this->IsSmall();
	uint32 nb_of_usize = (uint32)Math::Ceil((double)ncap / (double)(sizeof(usize) * BITS_PER_BYTE));
	uint32 curr_nb_of_usize = (uint32)Math::Ceil((double)this->GetNormalLength() / (double)(sizeof(usize) * BITS_PER_BYTE));

	if (is_small) {
		usize* new_bits = new usize[nb_of_usize];
		constexpr uint32 bytes_to_copy = sizeof(uint8) * (LAST_INDEX); // copy all bytes execpt the last one
		memcpy(new_bits, m_Data, bytes_to_copy);
		memset((uint8*)new_bits + bytes_to_copy, 0, nb_of_usize * sizeof(usize) - bytes_to_copy); // set what remains to 0
		m_Bits = new_bits;
		this->SetNormal((usize)this->GetSmallLength()); // because it was small, but now its normal
	} else if (!is_small && nb_of_usize > curr_nb_of_usize) {
		usize* new_bits = new usize[nb_of_usize];
		uint32 bytes_to_copy = (uint32)Math::Ceil((double)this->GetNormalLength() / (double)(sizeof(uint8) * BITS_PER_BYTE));
		memcpy(new_bits, m_Bits, bytes_to_copy * sizeof(uint8)); // copy all bytes execpt the last one
		memset((uint8*)new_bits + bytes_to_copy, 0, nb_of_usize * sizeof(usize) - bytes_to_copy); // set what remains to 0
		delete[] m_Bits;
		m_Bits = new_bits;
	}
}


void Bitset::Fill(bool bit)
{
	bool is_small = this->IsSmall();

	if (is_small) {
		uint8 bit_len = this->GetSmallLength();
		uint32 nb_of_bytes_to_set = (uint32)Math::Floor((double)bit_len / (double)(sizeof(uint8) * BITS_PER_BYTE));
		memset(m_Data, bit ? std::numeric_limits<uint8>::max() : 0, nb_of_bytes_to_set * sizeof(uint8));
		int32 rest_of_bits = (bit_len - nb_of_bytes_to_set * sizeof(uint8) * BITS_PER_BYTE);
		if (rest_of_bits > 0) {
			m_Data[nb_of_bytes_to_set] |= (uint16(1) << (rest_of_bits)) - 1;
		}
	}else {
		usize bit_len = this->GetNormalLength();
		uint32 nb_of_usize_to_set = (uint32)Math::Floor((double)bit_len / (double)(sizeof(usize) * BITS_PER_BYTE));
		memset(m_Bits, bit ? std::numeric_limits<uint32>::max() : 0, (usize)nb_of_usize_to_set * sizeof(usize));
		int32 rest_of_bits = int32(bit_len - nb_of_usize_to_set * sizeof(usize) * BITS_PER_BYTE);
		if (rest_of_bits > 0) {
			m_Bits[nb_of_usize_to_set] |= (usize(1) << (rest_of_bits)) - 1;
		}
	}
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
		int32 rest_of_bits = ((uint32)len - nb_byte_to_look_at * sizeof(uint8) * BITS_PER_BYTE);
		m_Data[nb_byte_to_look_at] ^= (-(int8)bit ^ m_Data[nb_byte_to_look_at]) & (uint8(1) << rest_of_bits);
		return;
	}

	uint32 nb_of_usize_to_look_at = (uint32)Math::Floor((double)len / (double)(sizeof(usize) * BITS_PER_BYTE));
	int32 rest_of_bits = int32(len - nb_of_usize_to_look_at * sizeof(usize) * BITS_PER_BYTE);
	m_Bits[nb_of_usize_to_look_at] ^= (-(int8)bit ^ m_Bits[nb_of_usize_to_look_at]) & (usize(1) << rest_of_bits);
}

Bitset::BitsBlock Bitset::operator[](usize index)
{
	bool is_small = this->IsSmall();

	uint32 nb_byte_to_look_at = (uint32)Math::Floor((double)index / (double)(sizeof(uint8) * BITS_PER_BYTE));
	int32 rest_of_bits = ((uint32)index - nb_byte_to_look_at * sizeof(uint8) * BITS_PER_BYTE);

	if (is_small) {
		ASSERTF((index >= this->GetSmallLength()), "Bitset index out of range");
		return BitsBlock(m_Data[nb_byte_to_look_at], rest_of_bits);
	}

	ASSERTF((index >= this->GetNormalLength()), "Bitset index out of range");
	return BitsBlock(*((uint8*)m_Bits + nb_byte_to_look_at), rest_of_bits);
}

bool Bitset::Get(usize index) const
{
	bool is_small = this->IsSmall();

	if (is_small) {
		ASSERTF((index >= this->GetSmallLength()), "Bitset index out of range");

		uint32 nb_byte_to_look_at = (uint32)Math::Floor((double)index / (double)(sizeof(uint8) * BITS_PER_BYTE));
		int32 rest_of_bits = ((uint32)index - nb_byte_to_look_at * sizeof(uint8) * BITS_PER_BYTE);
		return m_Data[nb_byte_to_look_at] & (uint16(1) << (rest_of_bits));
	}

	ASSERTF((index >= this->GetNormalLength()), "Bitset index out of range");

	uint32 nb_of_usize_to_look_at = (uint32)Math::Floor((double)index / (double)(sizeof(usize) * BITS_PER_BYTE));
	int32 rest_of_bits = int32(index - nb_of_usize_to_look_at * sizeof(usize) * BITS_PER_BYTE);
	return m_Bits[nb_of_usize_to_look_at] & (usize(1) << (rest_of_bits));
}


void Bitset::Set(usize index, bool value) // TODO!
{
	bool is_small = this->IsSmall();

	if (is_small) {
		ASSERTF((index >= this->GetSmallLength()), "Bitset index out of range");

		uint32 nb_byte_to_look_at = (uint32)Math::Floor((double)index / (double)(sizeof(uint8) * BITS_PER_BYTE));
		int32 rest_of_bits = ((uint32)index - nb_byte_to_look_at * sizeof(uint8) * BITS_PER_BYTE);
		m_Data[nb_byte_to_look_at] ^= (-(int8)value ^ m_Data[nb_byte_to_look_at]) & (uint8(1) << rest_of_bits);
		return;
	}

	ASSERTF((index >= this->GetNormalLength()), "Bitset index out of range");

	uint32 nb_of_usize_to_look_at = (uint32)Math::Floor((double)index / (double)(sizeof(usize) * BITS_PER_BYTE));
	int32 rest_of_bits = int32(index - nb_of_usize_to_look_at * sizeof(usize) * BITS_PER_BYTE);
	m_Bits[nb_of_usize_to_look_at] ^= (-(int8)value ^ m_Bits[nb_of_usize_to_look_at]) & (usize(1) << rest_of_bits);
}

void Bitset::Toggle(usize index)
{
	bool is_small = this->IsSmall();

	if (is_small) {
		ASSERTF((index >= this->GetSmallLength()), "Bitset index out of range");

		uint32 nb_byte_to_look_at = (uint32)Math::Floor((double)index / (double)(sizeof(uint8) * BITS_PER_BYTE));
		int32 rest_of_bits = ((uint32)index - nb_byte_to_look_at * sizeof(uint8) * BITS_PER_BYTE);
		m_Data[nb_byte_to_look_at] ^= uint8(1) << rest_of_bits;
		return;
	}

	ASSERTF((index >= this->GetNormalLength()), "Bitset index out of range");

	uint32 nb_of_usize_to_look_at = (uint32)Math::Floor((double)index / (double)(sizeof(usize) * BITS_PER_BYTE));
	int32 rest_of_bits = int32(index - nb_of_usize_to_look_at * sizeof(usize) * BITS_PER_BYTE);
	m_Bits[nb_of_usize_to_look_at] ^= (usize(1) << (rest_of_bits));
}

Bitset& Bitset::operator|=(const Bitset& other)
{
	bool is_small = this->IsSmall();
	bool is_other_small = other.IsSmall();

	if (is_small) {
		usize len = this->GetSmallLength();
		uint32 nb_bytes = (uint32)Math::Ceil((double)len / (double)(sizeof(uint8) * BITS_PER_BYTE));

		if (is_other_small) {
			usize other_len = other.GetSmallLength();
			uint32 other_nb_bytes = (uint32)Math::Ceil((double)other_len / (double)(sizeof(uint8) * BITS_PER_BYTE));

			for (uint32 i = 0; i < Math::Min(nb_bytes, other_nb_bytes); i++) {
				m_Data[i] |= other.m_Data[i];
			}
		}else {
			usize other_len = other.GetNormalLength();
			uint32 other_nb_bytes = (uint32)Math::Ceil((double)other_len / (double)(sizeof(uint8) * BITS_PER_BYTE));

			for (uint32 i = 0; i < Math::Min(nb_bytes, other_nb_bytes); i++) {
				m_Data[i] |= *((uint8*)other.m_Bits + i);
			}
		}
	}else {
		usize len = this->GetNormalLength();
		uint32 nb_bytes = (uint32)Math::Ceil((double)len / (double)(sizeof(uint8) * BITS_PER_BYTE));

		if (is_other_small) {
			usize other_len = other.GetSmallLength();
			uint32 other_nb_bytes = (uint32)Math::Ceil((double)other_len / (double)(sizeof(uint8) * BITS_PER_BYTE));

			for (uint32 i = 0; i < Math::Min(nb_bytes, other_nb_bytes); i++) {
				*((uint8*)m_Bits + i) |= other.m_Data[i];
			}
		}else {
			usize other_len = other.GetNormalLength();
			uint32 other_nb_bytes = (uint32)Math::Ceil((double)other_len / (double)(sizeof(uint8) * BITS_PER_BYTE));

			for (uint32 i = 0; i < Math::Min(nb_bytes, other_nb_bytes); i++) {
				*((uint8*)m_Bits + i) |= *((uint8*)other.m_Bits + i);
			}
		}
	}

	return *this;
}

Bitset& Bitset::operator&=(const Bitset& other)
{
	bool is_small = this->IsSmall();
	bool is_other_small = other.IsSmall();

	if (is_small) {
		usize len = this->GetSmallLength();
		uint32 nb_bytes = (uint32)Math::Ceil((double)len / (double)(sizeof(uint8) * BITS_PER_BYTE));

		if (is_other_small) {
			usize other_len = other.GetSmallLength();
			uint32 other_nb_bytes = (uint32)Math::Ceil((double)other_len / (double)(sizeof(uint8) * BITS_PER_BYTE));

			for (uint32 i = 0; i < Math::Min(nb_bytes, other_nb_bytes); i++) {
				m_Data[i] &= other.m_Data[i];
			}
		}else {
			usize other_len = other.GetNormalLength();
			uint32 other_nb_bytes = (uint32)Math::Ceil((double)other_len / (double)(sizeof(uint8) * BITS_PER_BYTE));

			for (uint32 i = 0; i < Math::Min(nb_bytes, other_nb_bytes); i++) {
				m_Data[i] &= *((uint8*)other.m_Bits + i);
			}
		}
	}
	else {
		usize len = this->GetNormalLength();
		uint32 nb_bytes = (uint32)Math::Ceil((double)len / (double)(sizeof(uint8) * BITS_PER_BYTE));

		if (is_other_small) {
			usize other_len = other.GetSmallLength();
			uint32 other_nb_bytes = (uint32)Math::Ceil((double)other_len / (double)(sizeof(uint8) * BITS_PER_BYTE));

			for (uint32 i = 0; i < Math::Min(nb_bytes, other_nb_bytes); i++) {
				*((uint8*)m_Bits + i) &= other.m_Data[i];
			}
		}else {
			usize other_len = other.GetNormalLength();
			uint32 other_nb_bytes = (uint32)Math::Ceil((double)other_len / (double)(sizeof(uint8) * BITS_PER_BYTE));

			for (uint32 i = 0; i < Math::Min(nb_bytes, other_nb_bytes); i++) {
				*((uint8*)m_Bits + i) &= *((uint8*)other.m_Bits + i);
			}
		}
	}

	return *this;
}

Bitset& Bitset::operator^=(const Bitset& other)
{
	bool is_small = this->IsSmall();
	bool is_other_small = other.IsSmall();

	if (is_small) {
		usize len = this->GetSmallLength();
		uint32 nb_bytes = (uint32)Math::Ceil((double)len / (double)(sizeof(uint8) * BITS_PER_BYTE));

		if (is_other_small) {
			usize other_len = other.GetSmallLength();
			uint32 other_nb_bytes = (uint32)Math::Ceil((double)other_len / (double)(sizeof(uint8) * BITS_PER_BYTE));

			for (uint32 i = 0; i < Math::Min(nb_bytes, other_nb_bytes); i++) {
				m_Data[i] ^= other.m_Data[i];
			}
		}else {
			usize other_len = other.GetNormalLength();
			uint32 other_nb_bytes = (uint32)Math::Ceil((double)other_len / (double)(sizeof(uint8) * BITS_PER_BYTE));

			for (uint32 i = 0; i < Math::Min(nb_bytes, other_nb_bytes); i++) {
				m_Data[i] ^= *((uint8*)other.m_Bits + i);
			}
		}
	}else {
		usize len = this->GetNormalLength();
		uint32 nb_bytes = (uint32)Math::Ceil((double)len / (double)(sizeof(uint8) * BITS_PER_BYTE));

		if (is_other_small) {
			usize other_len = other.GetSmallLength();
			uint32 other_nb_bytes = (uint32)Math::Ceil((double)other_len / (double)(sizeof(uint8) * BITS_PER_BYTE));

			for (uint32 i = 0; i < Math::Min(nb_bytes, other_nb_bytes); i++) {
				*((uint8*)m_Bits + i) ^= other.m_Data[i];
			}
		}else {
			usize other_len = other.GetNormalLength();
			uint32 other_nb_bytes = (uint32)Math::Ceil((double)other_len / (double)(sizeof(uint8) * BITS_PER_BYTE));

			for (uint32 i = 0; i < Math::Min(nb_bytes, other_nb_bytes); i++) {
				*((uint8*)m_Bits + i) ^= *((uint8*)other.m_Bits + i);
			}
		}
	}

	return *this;
}

Bitset& Bitset::operator~()
{
	bool is_small = this->IsSmall();

	if (is_small) {
		usize len = this->GetSmallLength();
		uint32 nb_bytes = (uint32)Math::Floor((double)len / (double)(sizeof(uint8) * BITS_PER_BYTE));

		for (uint32 i = 0; i < nb_bytes; i++) {
			m_Data[i] = ~m_Data[i];
		}

		for (uint32 i = nb_bytes * sizeof(uint8); i < len; i++) {
			int32 rest_of_bits = (i - nb_bytes * sizeof(uint8) * BITS_PER_BYTE);
			m_Data[nb_bytes] ^= uint8(1) << rest_of_bits;
		}
	}else {
		usize len = this->GetNormalLength();
		uint32 nb_bytes = (uint32)Math::Floor((double)len / (double)(sizeof(uint8) * BITS_PER_BYTE));

		for (uint32 i = 0; i < nb_bytes; i++) {
			*((uint8*)m_Bits + i) = ~*((uint8*)m_Bits + i);
		}

		for (uint32 i = nb_bytes * sizeof(uint8); i < len; i++) {
			int32 rest_of_bits = (i - nb_bytes * sizeof(uint8) * BITS_PER_BYTE);
			*((uint*)m_Bits + nb_bytes) ^= uint8(1) << rest_of_bits;
		}
	}
	return *this;
}

Bitset& Bitset::operator<<=(usize n)
{
	usize len = this->Length();

	for (usize  i = 0, j = n; i < len - n; i++, j++) {
		this->Set(i, this->Get(j));
	}

	for (usize j = len - n; j < len; j++) {
		this->Set(j, 0);
	}

	return *this;
}

Bitset& Bitset::operator>>=(usize n)
{
	usize len = this->Length();
	Bitset copy(*this);

	for (usize i = 0, j = n; i < len - n; i++, j++) {
		this->Set(j, copy.Get(i));
	}

	for (usize j = 0; j < n; j++) {
		this->Set(j, 0);
	}

	return *this;
}

bool Bitset::operator==(const Bitset& other) const
{
	bool is_small = this->IsSmall();
	bool is_other_small = other.IsSmall();

	if (is_small) {
		usize len = this->GetSmallLength();
		uint32 nb_bytes = (uint32)Math::Ceil((double)len / (double)(sizeof(uint8) * BITS_PER_BYTE));

		if (is_other_small) {
			usize other_len = other.GetSmallLength();

			if (other_len != len)
				return false;

			uint32 other_nb_bytes = (uint32)Math::Ceil((double)other_len / (double)(sizeof(uint8) * BITS_PER_BYTE));

			for (uint32 i = 0; i < Math::Min(nb_bytes, other_nb_bytes); i++) {
				if (m_Data[i] != other.m_Data[i])
					return false;
			}
		} else {
			usize other_len = other.GetNormalLength();
			

			if (other_len != len)
				return false;

			uint32 other_nb_bytes = (uint32)Math::Ceil((double)other_len / (double)(sizeof(uint8) * BITS_PER_BYTE));

			for (uint32 i = 0; i < Math::Min(nb_bytes, other_nb_bytes); i++) {
				if (m_Data[i] != *((uint8*)other.m_Bits + i))
					return false;
			}
		}
	} else {
		usize len = this->GetNormalLength();
		uint32 nb_bytes = (uint32)Math::Ceil((double)len / (double)(sizeof(uint8) * BITS_PER_BYTE));

		if (is_other_small) {
			usize other_len = other.GetSmallLength();

			if (other_len != len)
				return false;

			uint32 other_nb_bytes = (uint32)Math::Ceil((double)other_len / (double)(sizeof(uint8) * BITS_PER_BYTE));

			for (uint32 i = 0; i < Math::Min(nb_bytes, other_nb_bytes); i++) {
				if (*((uint8*)m_Bits + i) != other.m_Data[i])
					return false;
			}
		} else {
			usize other_len = other.GetNormalLength();

			if (other_len != len)
				return false;

			uint32 other_nb_bytes = (uint32)Math::Ceil((double)other_len / (double)(sizeof(uint8) * BITS_PER_BYTE));

			for (uint32 i = 0; i < Math::Min(nb_bytes, other_nb_bytes); i++) {
				if (*((uint8*)m_Bits + i) ^= *((uint8*)other.m_Bits + i))
					return false;
			}
		}
	}

	return true;
}

Bitset::Bitset(const Bitset& other)
{
	if (other.IsSmall()) {
		memcpy(m_Data, other.m_Data, sizeof(other.m_Data));
	} else {
		usize len = other.GetNormalLength();
		this->SetNormal(len);
		uint32 bytes_to_copy = (uint32)Math::Ceil((double)len / (double)(sizeof(uint8) * BITS_PER_BYTE));
		m_Bits = new usize[bytes_to_copy * sizeof(usize)];
		memcpy(m_Bits, other.m_Bits, bytes_to_copy * sizeof(uint8));
	}
}

Bitset::Bitset(Bitset&& other) noexcept
{
	if (other.IsSmall()) {
		memcpy(m_Data, other.m_Data, sizeof(m_Data));
	} else {
		usize len = other.GetNormalLength();
		this->SetNormal(len);
		m_Bits = other.m_Bits;
		other.m_Bits = NULL;
	}
}

Bitset& Bitset::operator=(const Bitset& other)
{
	this->Free();

	if (other.IsSmall()) {
		memcpy(m_Data, other.m_Data, sizeof(m_Data));
	} else {
		usize len = other.GetNormalLength();
		this->SetNormal(len);
		uint32 nb_of_usize = (uint32)Math::Ceil((double)len / (double)(sizeof(usize) * BITS_PER_BYTE));
		uint32 bytes_to_copy = (uint32)Math::Ceil((double)len / (double)(sizeof(uint8) * BITS_PER_BYTE));
		m_Bits = new usize[nb_of_usize];
		memcpy(m_Bits, other.m_Bits, bytes_to_copy * sizeof(uint8));
	}

	return *this;
}

Bitset& Bitset::operator=(Bitset&& other) noexcept
{
	this->Free();

	if (other.IsSmall()) {
		memcpy(m_Data, other.m_Data, sizeof(m_Data));
	} else {
		usize len = other.GetNormalLength();
		this->SetNormal(len);
		m_Bits = other.m_Bits;
		other.m_Bits = NULL;
	}

	return *this;
}

Bitset::~Bitset()
{
	this->Free();
}

Bitset& Bitset::operator+=(Bitset other)
{
	Bitset carry(other.Length() + 1);

	while (other != Bitset(this->Length(), false)) {
		carry = *this & other;
		*this ^= other;
		other = carry >> (usize)1;
	}

	return *this;
}

usize Bitset::GetHash() const
{
	size_t hash = 0;
	const usize* bits = (usize*)m_Data;
	usize len = this->GetSmallLength();

	if (!this->IsSmall()) {
		len = this->GetNormalLength();
		bits = m_Bits;
	}
	uint32 nb_usize_bytes = (uint32) Math::Ceil((double)len / (double)(sizeof(usize) * BITS_PER_BYTE));

	for (usize i = 0; i < nb_usize_bytes; i++)
		hash ^= bits[i];

	return hash;
}

bool Bitset::operator>(const Bitset& other) const
{
	const uint8* bytes_arr[2] = { other.GetBytesArray(), this->GetBytesArray() } ;
	uint32 nb_bytes[2] = { 
		(uint32)Math::Ceil((double)other.Length() / (double)(sizeof(uint8) * BITS_PER_BYTE)),
		(uint32)Math::Ceil((double)this->Length() / (double)(sizeof(uint8) * BITS_PER_BYTE)),
	};

	bool max_bytes_index = true;
	if (nb_bytes[!max_bytes_index] > nb_bytes[max_bytes_index]) {
		max_bytes_index = !max_bytes_index;
	}

	for (uint32 i = nb_bytes[max_bytes_index]; i > nb_bytes[!max_bytes_index]; i--) {
		if (bytes_arr[max_bytes_index][i])
			return max_bytes_index;
	}

	for (int32 i = nb_bytes[max_bytes_index]; i >= 0; i--) {
		if (bytes_arr[max_bytes_index][i] > bytes_arr[!max_bytes_index][i])
			return max_bytes_index;
		else if ((bytes_arr[max_bytes_index][i] < bytes_arr[!max_bytes_index][i])) 
			return !max_bytes_index;
	}

	return false;
}

bool Bitset::operator<=(const Bitset& other) const
{
	return !(*this > other);
}

bool Bitset::operator<(const Bitset& other) const
{
	const uint8* bytes_arr[2] = { this->GetBytesArray(), other.GetBytesArray() };
	uint32 nb_bytes[2] = {
		(uint32)Math::Ceil((double)this->Length() / (double)(sizeof(uint8) * BITS_PER_BYTE)),
		(uint32)Math::Ceil((double)other.Length() / (double)(sizeof(uint8) * BITS_PER_BYTE)),
	};

	bool max_bytes_index = false;
	if (nb_bytes[!max_bytes_index] > nb_bytes[max_bytes_index]) {
		max_bytes_index = !max_bytes_index;
	}

	for (uint32 i = nb_bytes[max_bytes_index]; i > nb_bytes[!max_bytes_index]; i--) {
		if (bytes_arr[max_bytes_index][i])
			return max_bytes_index;
	}

	for (int32 i = nb_bytes[!max_bytes_index]; i >= 0; i--) {
		if (bytes_arr[max_bytes_index][i] > bytes_arr[!max_bytes_index][i])
			return max_bytes_index;
		else if ((bytes_arr[max_bytes_index][i] < bytes_arr[!max_bytes_index][i]))
			return !max_bytes_index;
	}

	return false;
}

bool Bitset::operator>=(const Bitset& other) const
{
	return !(*this < other);
}

Bitset::operator bool()
{
	bool is_small = this->IsSmall();

	if (is_small) {
		usize len = this->GetSmallLength();

		if (len == 0)
			return false;

		uint32 nb_bytes = (uint32)Math::Ceil((double)len / (double)(sizeof(uint8) * BITS_PER_BYTE));

		for (uint32 i = 0; i < nb_bytes; i++) {
			if (m_Data[i])
				return true;
		}

		for (uint32 i = nb_bytes * sizeof(uint8); i < len; i++) {
			if (this->Get(i))
				return true;
		}

		return false;
	} 

	usize len = this->GetNormalLength();

	if (len == 0)
		return false;

	uint32 nb_bytes = (uint32)Math::Ceil((double)len / (double)(sizeof(uint8) * BITS_PER_BYTE));

	for (uint32 i = 0; i < nb_bytes; i++) {
		if (*((uint8*)m_Bits + i))
			return true;
	}

	for (uint32 i = nb_bytes * sizeof(uint8); i < len; i++) {
		if (*((uint8*)m_Bits + i))
			return true;
	}

	return false;
}

TRE_NS_END