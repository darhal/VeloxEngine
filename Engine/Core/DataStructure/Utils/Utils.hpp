#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/DataStructure/Bitset/Bitset.hpp>
#include <Core/DataStructure/String/String.hpp>

TRE_NS_START

struct Utils
{
	FORCEINLINE static String ToString(const Bitset& bitset)
	{
		uint32 len = (uint32) bitset.Length();
		String res(len + 1);

		for (uint32 i = 0; i < len; i++) {
			res[i] = (bitset[i] ? '1' : '0');
		}

		return res;
	}
};

TRE_NS_END