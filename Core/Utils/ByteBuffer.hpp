#pragma once

#include <climits>
#include <Core/Misc/Defines/Common.hpp>
#include "Common.hpp"

TRE_NS_START

class ByteReader
{
public:
	ByteReader(uint length, bool littleEndian) : buffer(new uint8[length]), length(length), ptr(0), littleEndian(littleEndian) {}
	~ByteReader() { delete[] buffer; }

	void Reuse(usize length)
	{
		delete[] buffer;
		buffer = new uint8[length];
		this->length = length;
		ptr = 0;
	}

	uint8* Data() { return buffer; }
	usize Length() { return length; }

	void Advance(usize count) { ptr += count; }
	void Move(usize location) { ptr = location; }

	bool Compare(usize location, usize length, const uint8* data)
	{
		if (location + length > this->length) return false;

		for (uint i = 0; i < length; i++)
			if (buffer[location + i] != data[i]) return false;

		return true;
	}

	uint8 PeekByte(uint offset = 0) { return buffer[ptr + offset]; }

	uint8 ReadUbyte()
	{
		uint8 val = buffer[ptr];
		ptr++;
		return val;
	}

	uint16 ReadUshort()
	{
		uint16 val;
		if (littleEndian) val = buffer[ptr + 0] + (buffer[ptr + 1] << 8);
		else val = buffer[ptr + 1] + (buffer[ptr + 0] << 8);
		ptr += 2;
		return val;
	}

	uint ReadUint()
	{
		uint val;
		if (littleEndian) val = buffer[ptr + 0] + (buffer[ptr + 1] << 8) + (buffer[ptr + 2] << 16) + (buffer[ptr + 3] << 24);
		else val = buffer[ptr + 3] + (buffer[ptr + 2] << 8) + (buffer[ptr + 1] << 16) + (buffer[ptr + 0] << 24);
		ptr += 4;
		return val;
	}

	int ReadInt()
	{
		uint val = ReadUint();
		if (val <= INT_MAX)
			return val;
		else
			return INT_MIN + val - INT_MAX - 1;
	}

	void Read(uint8* dest, usize length)
	{
		for (usize i = 0; i < length; i++)
			dest[i] = buffer[ptr + i];
		ptr += length;
	}

private:
	uint8* buffer;
	usize length;
	ssize ptr;
	bool littleEndian;

	ByteReader(const ByteReader&);
	const ByteReader& operator=(const ByteReader&);
};

class ByteWriter
{
public:
	ByteWriter(bool littleEndian) : littleEndian(littleEndian) {};

	uint8* Data() { return &buffer[0]; }
	ssize Length() { return buffer.size(); }

	void Pad(uint count)
	{
		for (uint i = 0; i < count; i++)
			buffer.push_back(0);
	}

	void WriteUbyte(uint8 val)
	{
		buffer.push_back(val);
	}

	void WriteUshort(uint16 val)
	{
		if (littleEndian) {
			buffer.push_back(val & 0x00FF);
			buffer.push_back((val & 0xFF00) >> 8);
		}
		else {
			buffer.push_back((val & 0xFF00) >> 8);
			buffer.push_back(val & 0x00FF);
		}
	}

	void WriteUint(uint val)
	{
		if (littleEndian) {
			buffer.push_back((uint8)(val & 0x000000FF));
			buffer.push_back((uint8)((val & 0x0000FF00) >> 8));
			buffer.push_back((uint8)((val & 0x00FF0000) >> 16));
			buffer.push_back((uint8)((val & 0xFF000000) >> 24));
		}
		else {
			buffer.push_back((uint8)((val & 0xFF000000) >> 24));
			buffer.push_back((uint8)((val & 0x00FF0000) >> 16));
			buffer.push_back((uint8)((val & 0x0000FF00) >> 8));
			buffer.push_back((uint8)(val & 0x000000FF));
		}
	}

	void WriteString(const char* str)
	{
		while (*str != 0)
			buffer.push_back(*(str++));
		buffer.push_back(0);
	}

private:
	::std::vector<uint8> buffer;
	bool littleEndian;

	ByteWriter(const ByteWriter&);
	const ByteWriter& operator=(const ByteWriter&);
};

TRE_NS_END